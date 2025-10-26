#include "templateDecls.h"

int main(){

    //
    LOG << "ProcTestEnv exec Skript" << endl;

    //
    const std::string scriptPath = "../Import/exp.bld";

    //
    Scope programCache;
    
    // std::string OSstring = "_WIN32";

    // //
    // auto env = programCache.constructAndReturnVariable("OS");
    // env->constructByObject(new types::STRING(&OSstring));

    auto results = executeScript(scriptPath, &programCache);
    
    //
    LOG << "Skript Returns: " << endl;
    LOG << results << endl;

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