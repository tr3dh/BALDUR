#include "__backgroundShell.h"

#include <windows.h>
#include <chrono>

struct BackgroundShell::Impl {
    HANDLE hStdInWrite = nullptr;
    HANDLE hStdOutRead = nullptr;
    PROCESS_INFORMATION pi{};
};

BackgroundShell::~BackgroundShell() {
    stop();
    if (pImpl) delete pImpl;
}

bool BackgroundShell::start(const std::string& shellCmd) {
    
    stop();

    if (!pImpl) pImpl = new Impl();
    stopFlag.store(false);

    SECURITY_ATTRIBUTES sa{ sizeof(sa), nullptr, TRUE };
    HANDLE hStdOutWrite, hStdInRead;

    if (!CreatePipe(&pImpl->hStdOutRead, &hStdOutWrite, &sa, 0)) return false;
    if (!CreatePipe(&hStdInRead, &pImpl->hStdInWrite, &sa, 0)) {
        CloseHandle(pImpl->hStdOutRead);
        CloseHandle(hStdOutWrite);
        return false;
    }

    STARTUPINFO si{};
    si.cb = sizeof(si);
    si.hStdInput = hStdInRead;
    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdOutWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcess(nullptr, (LPSTR)shellCmd.c_str(),
        nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pImpl->pi)) {
        CloseHandle(hStdInRead);
        CloseHandle(pImpl->hStdInWrite);
        CloseHandle(pImpl->hStdOutRead);
        CloseHandle(hStdOutWrite);
        return false;
    }

    CloseHandle(hStdInRead);
    CloseHandle(hStdOutWrite);

    readerThread = std::thread(&BackgroundShell::readerLoop, this);

    return true;
}

void BackgroundShell::readerLoop() {
    char buf[512];
    DWORD nRead;

    while (!stopFlag.load() && pImpl) {
        BOOL success = ReadFile(pImpl->hStdOutRead, buf, sizeof(buf) - 1, &nRead, nullptr);
        if (success && nRead > 0) {
            buf[nRead] = '\0';
            {
                std::lock_guard<std::mutex> lk(mtx);
                outputBuffer += buf;  // direkt in den übergebenen String
            }
        } else {
            // Leichte Pause, um CPU nicht zu blockieren
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void BackgroundShell::sendCommand(const std::string& cmd) {
    if (!pImpl || !pImpl->hStdInWrite) return;
    std::string line = cmd + "\r\n";
    DWORD written;
    WriteFile(pImpl->hStdInWrite, line.c_str(), (DWORD)line.size(), &written, nullptr);
}

std::string BackgroundShell::getOutput() {
    std::lock_guard<std::mutex> lk(mtx);
    std::string out = outputBuffer;
    outputBuffer.clear();
    return out;
}

void BackgroundShell::stop() {
    stopFlag.store(true);
    if (readerThread.joinable()) readerThread.join();

    if (pImpl) {
        if (pImpl->pi.hProcess) TerminateProcess(pImpl->pi.hProcess, 0);
        if (pImpl->pi.hProcess) CloseHandle(pImpl->pi.hProcess);
        if (pImpl->pi.hThread) CloseHandle(pImpl->pi.hThread);
        if (pImpl->hStdInWrite) CloseHandle(pImpl->hStdInWrite);
        if (pImpl->hStdOutRead) CloseHandle(pImpl->hStdOutRead);

        pImpl->hStdInWrite = nullptr;
        pImpl->hStdOutRead = nullptr;
    }
}