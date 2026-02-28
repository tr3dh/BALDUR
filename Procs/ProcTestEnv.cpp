#include "templateDecls.h"

#ifdef DEBUG
std::string fSuffix = "_d";
#else
std::string fSuffix = "";
#endif

int main(int argc, const char* argv[]){

    //
    timePoint procStart = chrono::now();

    //
    const std::string scriptPath = argc > 1 ? argv[1] : "../Import/exp.bld";

    //
    std::string cmd = ".\\ProcLang" + fSuffix + " execute " + scriptPath;

    //
    streamWinCommand(cmd, [&](const char* callback){
        LOG << callback;
    });

    //
    duration runTime = chrono::now() - procStart;
    LOG << "Programm took " << runTime.count() << " seconds to run" << endl;

    return 0;
}