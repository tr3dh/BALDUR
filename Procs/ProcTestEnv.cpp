#include "templateDecls.h"

int main(int argc, const char* argv[]){

    //
    timePoint procStart = chrono::now();

    //
    const std::string scriptPath = argc > 1 ? argv[1] : "../Import/exp.bld";

    //
    std::string cmd = ".\\ProcLang_d execute " + scriptPath;

    //
    streamWinCommand(cmd, [&](const char* callback){
        LOG << callback;
    });

    //
    duration runTime = chrono::now() - procStart;
    LOG << "Programm took " << runTime.count() << " seconds to run" << endl;

    return 0;
}