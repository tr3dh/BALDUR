#include "Logging.h"

void openLogFile(){

    FreeConsole();

    g_logFile = std::ofstream("../bin/.LOG");
    std::cout.rdbuf(g_logFile.rdbuf());

    #undef LOG
    #define LOG std::cout << "[" << getTimestamp() << "] : "

    #undef _ERROR
    #define _ERROR std::cerr << "[" << getTimestamp() << "] : " << "!! <ERROR> !! -> "
}

void closeLogFile(){

    // Release build
    g_logFile.close();
}