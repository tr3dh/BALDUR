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
    const std::string scriptPath = argc > 1 ? argv[1] : "Import/0_0_0_testEnv.bld";

    //
    std::string cmd = ".\\build\\Baldur" + fSuffix + " execute " + scriptPath;

    //
    streamWinCommand(cmd, [&](const char* callback){
        LOG << callback;
    });

    //
    duration runTime = chrono::now() - procStart;
    LOG << "Program took " << runTime.count() << " seconds to run" << endln;

    return 0;
}