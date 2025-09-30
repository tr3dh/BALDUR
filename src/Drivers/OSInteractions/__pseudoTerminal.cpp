#include "__pseudoTerminal.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>

bool PseudoTerminal::constructedMemberThisExecution = false;

struct PseudoTerminal::Impl {
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    HANDLE hChildStd_ERR_Rd = NULL;
    HANDLE hChildStd_ERR_Wr = NULL;
    
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    
    std::thread outputThread;
    std::thread errorThread;
    std::mutex outputMutex;
    std::string outputBuffer;
    bool isRunning = false;
    bool shouldStop = false;
    
    void readOutputThread() {
        char buffer[4096];
        DWORD bytesRead;
        
        while (!shouldStop && isRunning) {
            if (PeekNamedPipe(hChildStd_OUT_Rd, NULL, 0, NULL, &bytesRead, NULL) && bytesRead > 0) {
                if (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    
                    std::lock_guard<std::mutex> lock(outputMutex);
                    outputBuffer += std::string(buffer, bytesRead);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void readErrorThread() {
        char buffer[4096];
        DWORD bytesRead;
        
        while (!shouldStop && isRunning) {
            if (PeekNamedPipe(hChildStd_ERR_Rd, NULL, 0, NULL, &bytesRead, NULL) && bytesRead > 0) {
                if (ReadFile(hChildStd_ERR_Rd, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    
                    std::lock_guard<std::mutex> lock(outputMutex);
                    outputBuffer += std::string(buffer, bytesRead);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

PseudoTerminal::PseudoTerminal() : pImpl(new Impl()) {

    constructedMemberThisExecution = true;

    ZeroMemory(&pImpl->piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&pImpl->siStartInfo, sizeof(STARTUPINFO));
}

PseudoTerminal::~PseudoTerminal() {
    stop();
    delete pImpl;
}

bool PseudoTerminal::start(const std::string& shellCmd, int cols, int rows) {
    if (pImpl->isRunning) {
        return false;
    }
    
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    
    // Create pipes for stdout
    if (!CreatePipe(&pImpl->hChildStd_OUT_Rd, &pImpl->hChildStd_OUT_Wr, &saAttr, 0)) {
        return false;
    }
    if (!SetHandleInformation(pImpl->hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        return false;
    }
    
    // Create pipes for stderr
    if (!CreatePipe(&pImpl->hChildStd_ERR_Rd, &pImpl->hChildStd_ERR_Wr, &saAttr, 0)) {
        return false;
    }
    if (!SetHandleInformation(pImpl->hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) {
        return false;
    }
    
    // Create pipes for stdin
    if (!CreatePipe(&pImpl->hChildStd_IN_Rd, &pImpl->hChildStd_IN_Wr, &saAttr, 0)) {
        return false;
    }
    if (!SetHandleInformation(pImpl->hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
        return false;
    }
    
    // Setup startup info
    pImpl->siStartInfo.cb = sizeof(STARTUPINFO);
    pImpl->siStartInfo.hStdError = pImpl->hChildStd_ERR_Wr;
    pImpl->siStartInfo.hStdOutput = pImpl->hChildStd_OUT_Wr;
    pImpl->siStartInfo.hStdInput = pImpl->hChildStd_IN_Rd;
    pImpl->siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    pImpl->siStartInfo.dwFlags |= STARTF_USESHOWWINDOW;
    pImpl->siStartInfo.wShowWindow = SW_HIDE; // Verstecktes Fenster
    
    // Create process
    std::string cmdLine = shellCmd;
    BOOL bSuccess = CreateProcessA(
        NULL,
        const_cast<char*>(cmdLine.c_str()),
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW, // Kein Konsolenfenster
        NULL,
        NULL,
        &pImpl->siStartInfo,
        &pImpl->piProcInfo
    );
    
    if (!bSuccess) {
        DWORD error = GetLastError();
        std::cerr << "CreateProcess failed with error: " << error << std::endl;
        return false;
    }
    
    // Close handles to the pipe ends that are now owned by the child process
    CloseHandle(pImpl->hChildStd_OUT_Wr);
    CloseHandle(pImpl->hChildStd_ERR_Wr);
    CloseHandle(pImpl->hChildStd_IN_Rd);
    
    pImpl->isRunning = true;
    pImpl->shouldStop = false;
    
    // Start reader threads
    pImpl->outputThread = std::thread(&Impl::readOutputThread, pImpl);
    pImpl->errorThread = std::thread(&Impl::readErrorThread, pImpl);
    
    return true;
}

void PseudoTerminal::sendCommand(const std::string& cmd) {
    if (!pImpl->isRunning) {
        return;
    }
    
    std::string fullCmd = cmd + "\r\n";
    DWORD bytesWritten;
    
    if (!WriteFile(pImpl->hChildStd_IN_Wr, fullCmd.c_str(), fullCmd.length(), &bytesWritten, NULL)) {
        DWORD error = GetLastError();
        std::lock_guard<std::mutex> lock(pImpl->outputMutex);
        pImpl->outputBuffer += "Error writing to terminal: " + std::to_string(error) + "\n";
    }
    
    FlushFileBuffers(pImpl->hChildStd_IN_Wr);
}

void PseudoTerminal::sendCtrlC() {
    if (!pImpl->isRunning) {
        return;
    }
    
    // Sende Ctrl+C (ASCII 3)
    char ctrlC = 3;
    DWORD bytesWritten;
    WriteFile(pImpl->hChildStd_IN_Wr, &ctrlC, 1, &bytesWritten, NULL);
    FlushFileBuffers(pImpl->hChildStd_IN_Wr);
}

void PseudoTerminal::sendCtrlZ() {
    if (!pImpl->isRunning) {
        return;
    }
    
    // Sende Ctrl+Z (ASCII 26)
    char ctrlZ = 26;
    DWORD bytesWritten;
    WriteFile(pImpl->hChildStd_IN_Wr, &ctrlZ, 1, &bytesWritten, NULL);
    FlushFileBuffers(pImpl->hChildStd_IN_Wr);
}

void PseudoTerminal::sendKey(char key) {
    if (!pImpl->isRunning) {
        return;
    }
    
    DWORD bytesWritten;
    WriteFile(pImpl->hChildStd_IN_Wr, &key, 1, &bytesWritten, NULL);
    FlushFileBuffers(pImpl->hChildStd_IN_Wr);
}

std::string PseudoTerminal::getOutput() {
    std::lock_guard<std::mutex> lock(pImpl->outputMutex);
    std::string result = pImpl->outputBuffer;
    pImpl->outputBuffer.clear();
    return result;
}

void PseudoTerminal::stop() {
    if (!pImpl->isRunning) {
        return;
    }
    
    pImpl->shouldStop = true;
    
    // Terminate the child process
    if (pImpl->piProcInfo.hProcess) {
        TerminateProcess(pImpl->piProcInfo.hProcess, 0);
        WaitForSingleObject(pImpl->piProcInfo.hProcess, 2000); // Wait up to 2 seconds
        CloseHandle(pImpl->piProcInfo.hProcess);
        CloseHandle(pImpl->piProcInfo.hThread);
    }
    
    // Wait for threads to finish
    if (pImpl->outputThread.joinable()) {
        pImpl->outputThread.join();
    }
    if (pImpl->errorThread.joinable()) {
        pImpl->errorThread.join();
    }
    
    // Close remaining handles
    if (pImpl->hChildStd_OUT_Rd) CloseHandle(pImpl->hChildStd_OUT_Rd);
    if (pImpl->hChildStd_ERR_Rd) CloseHandle(pImpl->hChildStd_ERR_Rd);
    if (pImpl->hChildStd_IN_Wr) CloseHandle(pImpl->hChildStd_IN_Wr);
    
    pImpl->isRunning = false;
}

bool PseudoTerminal::isReady() const {
    return pImpl->isRunning;
}

void PseudoTerminal::clearOutput() {
    std::lock_guard<std::mutex> lock(pImpl->outputMutex);
    pImpl->outputBuffer.clear();
}

// Neue Funktion: Befehl ohne Echo senden
void PseudoTerminal::sendSilentCommand(const std::string& cmd) {
    if (!pImpl->isRunning) {
        return;
    }
    
    std::string fullCmd = "@" + cmd + "\r\n";
    DWORD bytesWritten;
    
    if (!WriteFile(pImpl->hChildStd_IN_Wr, fullCmd.c_str(), fullCmd.length(), &bytesWritten, NULL)) {
        DWORD error = GetLastError();
        std::lock_guard<std::mutex> lock(pImpl->outputMutex);
        pImpl->outputBuffer += "Error writing to terminal: " + std::to_string(error) + "\n";
    }
    
    FlushFileBuffers(pImpl->hChildStd_IN_Wr);
}

// Neue Funktion: Terminal zurücksetzen
void PseudoTerminal::resetTerminal() {
    if (!pImpl->isRunning) {
        return;
    }
    
    // Mehrfache Ctrl+C senden
    for (int i = 0; i < 3; i++) {
        sendCtrlC();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Enter senden um eventuelle Prompts zu bestätigen
    sendKey('\r');
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Output leeren
    clearOutput();
}

// Neue Funktion: Automatisch aktuellen Kindprozess killen
void PseudoTerminal::killActiveChildProcess() {
    if (!pImpl->isRunning || pImpl->piProcInfo.dwProcessId == 0) {
        return;
    }
    
    std::cout << "Killing active child process automatically..." << std::endl;
    
    // Methode 1: Ctrl+C Signal an die Console Process Group
    if (!GenerateConsoleCtrlEvent(CTRL_C_EVENT, pImpl->piProcInfo.dwProcessId)) {
        std::cout << "Ctrl+C failed, trying break..." << std::endl;
        GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pImpl->piProcInfo.dwProcessId);
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Enter für "beliebige Taste" Prompts
    sendKey('\r');
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Falls das nicht reicht: Process Tree killen (nur Kinder, nicht cmd.exe)
    std::string cmd = "wmic process where \"ParentProcessId=" + 
                     std::to_string(pImpl->piProcInfo.dwProcessId) + 
                     "\" call terminate >nul 2>&1";
    system(cmd.c_str());
    
    clearOutput();
}

// Neue Funktion: Smart Kill - versucht verschiedene Methoden
void PseudoTerminal::smartKill() {
    if (!pImpl->isRunning) {
        return;
    }
    
    std::cout << "Smart killing current process..." << std::endl;
    
    // Schritt 1: Höflich mit Ctrl+C
    sendCtrlC();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    // Schritt 2: Enter für Prompts  
    sendKey('\r');
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Schritt 3: Noch ein Ctrl+C
    sendCtrlC();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Schritt 4: Escape versuchen
    sendKey(27); // ESC
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Schritt 5: Alle Kindprozesse per WMIC killen
    if (pImpl->piProcInfo.dwProcessId != 0) {
        std::string killChildren = "for /f \"tokens=2\" %i in ('wmic process where \"ParentProcessId=" +
                                 std::to_string(pImpl->piProcInfo.dwProcessId) +
                                 "\" get ProcessId /format:csv') do @taskkill /F /PID %i >nul 2>&1";
        system(killChildren.c_str());
    }
    
    clearOutput();
    std::cout << "Smart kill completed" << std::endl;

    // 
    sendKey('\r');
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Überarbeitete killCurrentProcess - automatisch ohne Namen
void PseudoTerminal::killCurrentProcess() {
    smartKill(); // Verwende die smarte Methode
}

// Neue Funktion: Interrupt senden (wie Ctrl+C aber programmatisch)
void PseudoTerminal::interruptCurrentProcess() {
    if (!pImpl->isRunning) {
        return;
    }
    
    // Sende CTRL_C_EVENT an die gesamte Process Group
    if (pImpl->piProcInfo.dwProcessId != 0) {
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0); // 0 = current process group
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Enter für eventuelle Prompts
        sendKey('\r');
        clearOutput();
    }
}

// Neue Funktion: Sofortiger Reset ohne Warten
void PseudoTerminal::hardReset() {
    if (!pImpl->isRunning) {
        return;
    }
    
    // 1. Process sofort killen
    if (pImpl->piProcInfo.hProcess) {
        TerminateProcess(pImpl->piProcInfo.hProcess, 9);
    }
    
    // 2. Terminal Input Buffer leeren
    FlushFileBuffers(pImpl->hChildStd_IN_Wr);
    
    // 3. Output Buffer leeren
    clearOutput();
    
    // 4. Neuen Prompt erzwingen
    sendCommand(""); // Leerer Befehl um Prompt zu bekommen
    
    std::cout << "Hard reset completed!" << std::endl;
}

// Optimierte terminateCurrentProcess
void PseudoTerminal::terminateCurrentProcess() {
    if (!pImpl->isRunning) {
        return;
    }
    
    // Schnelle Variante: Direkt force kill
    if (pImpl->piProcInfo.hProcess) {
        TerminateProcess(pImpl->piProcInfo.hProcess, 1);
        WaitForSingleObject(pImpl->piProcInfo.hProcess, 500); // Nur 0.5 Sekunden warten
    }
    
    clearOutput();
    
    // Neuen Prompt erzwingen
    sendKey('\r');
}

// Neue Funktion: Prüfen ob ein Kindprozess läuft
bool PseudoTerminal::hasActiveChildProcess() const {
    if (!pImpl->isRunning || !pImpl->piProcInfo.hProcess) {
        return false;
    }
    
    DWORD exitCode;
    if (GetExitCodeProcess(pImpl->piProcInfo.hProcess, &exitCode)) {
        return exitCode == STILL_ACTIVE;
    }
    return false;
}