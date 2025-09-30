#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class BackgroundShell {
public:

    BackgroundShell() = default;
    ~BackgroundShell();

    // Startet die Shell im Hintergrund (default: cmd.exe)
    bool start(const std::string& shellCmd = "cmd.exe");

    // Sendet einen Command an die Shell
    void sendCommand(const std::string& cmd);

    // Gibt den aktuellen Output zurück und leert den internen Buffer
    std::string getOutput();

    // Stoppt die Shell und schließt alle Handles
    void stop();

private:
    std::thread readerThread;
    std::atomic<bool> stopFlag{ false };
    std::mutex mtx;
    std::string outputBuffer;

    void readerLoop();

    // WinAPI-spezifische Handles (nur in CPP definiert)
    struct Impl;
    Impl* pImpl = nullptr;
};