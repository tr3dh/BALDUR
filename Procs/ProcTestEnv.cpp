#include "templateDecls.h"

int main(){

    //
    const std::string scriptPath = "../Import/exp.bld";

    std::string cmd = ".\\ProcLang_d execute " + scriptPath;

    streamWinCommand(cmd, [&](const char* callback){
        LOG << callback;
    });

    return 0;
}