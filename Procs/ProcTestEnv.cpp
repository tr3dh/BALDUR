#include "templateDecls.h"

int main(){

    //
    TensorExpression A("A", 4), B("B", 4), sc("scalar",0), sc2("scalar",0), tmp("tmp", 4);

    tmp.subAssign(A);
    tmp.subAssign(B);
    tmp.subAssign(B);
    tmp.subAssign(B);
    tmp.subAssign(tmp);

    sc.mulAssign(sc);
    sc.mulAssign(sc);
    sc2.mulAssign(A);
    
    sc.mulAssign(sc2);

    LOG << sc.toString() << endl;

    //
    return 0;

    //
    LOG << "ProcTestEnv exec Skript" << endl;

    //
    const std::string scriptPath = "../Import/exp.bld";

    auto results = executeScript(scriptPath, nullptr);
    
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