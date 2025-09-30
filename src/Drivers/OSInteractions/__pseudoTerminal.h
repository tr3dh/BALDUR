#pragma once

#include <string>

class PseudoTerminal {
public:

    PseudoTerminal();
    ~PseudoTerminal();

    static bool constructedMemberThisExecution;

    // Startet das Terminal mit dem gegebenen Shell-Kommando
    bool start(const std::string& shellCmd, int cols = 80, int rows = 25);
    
    // Sendet ein Kommando an das Terminal
    void sendCommand(const std::string& cmd);
    
    // Sendet Control-Signale
    void sendCtrlC();  // Strg+C (Prozess abbrechen)
    void sendCtrlZ();  // Strg+Z (Prozess pausieren)
    void sendKey(char key);  // Einzelnes Zeichen senden
    
    // Holt die Ausgabe des Terminals (und leert den Buffer)
    std::string getOutput();
    
    // Stoppt das Terminal
    void stop();
    
    // Zusätzliche Hilfsfunktionen
    bool isReady() const;
    void clearOutput();

    // In PseudoTerminal.h hinzufügen:
    void sendSilentCommand(const std::string& cmd);
    // In PseudoTerminal.h hinzufügen:
    void resetTerminal();
    void terminateCurrentProcess();  // Sauberes Beenden
    bool hasActiveChildProcess() const;  // Prüfen ob Prozess läuft

    // In PseudoTerminal.h hinzufügen:
    void killCurrentProcess();        // Sofortiges Kill
    void hardReset();               // Kompletter Reset ohne Warten

    // In PseudoTerminal.h hinzufügen:
    void killActiveChildProcess();    // Automatisch aktuellen Kindprozess killen
    void smartKill();                // Intelligenter Kill mit mehreren Methoden  
    void interruptCurrentProcess();   // Interrupt senden

private:
    struct Impl;
    Impl* pImpl;
    
    // Non-copyable
    PseudoTerminal(const PseudoTerminal&) = delete;
    PseudoTerminal& operator=(const PseudoTerminal&) = delete;
};