#include "templateDecls.h"

int main(){

    //
    const std::string scriptPath = "../Import/exp.bld";
    auto results = executeProgram(scriptPath);
    
    //
    // LOG << "Skript Returns: " << endl;
    // LOG << results << endl;

    //
    return 0;

    std::string cmd = ".\\ProcLang_d execute ../bin/temp.bld";

    LOG << "calling Interpreter" << endl;
    LOG << endl;

    streamWinCommand(cmd, [&](const char* callback){
        LOG << callback;
    });

    return 0;
}