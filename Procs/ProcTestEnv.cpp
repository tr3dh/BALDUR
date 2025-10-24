#include "templateDecls.h"

int main(){

    //
    LOG << "ProcTestEnv exec Skript" << endl;

    // // Bspl Skript
    // std::vector<std::string> scriptLines = {

    //     "// Beispielhaftes Skript",
    // };

    // //
    // std::ofstream outfile("../bin/temp.proc");
    // if (!outfile) {
    //     std::cerr << "Fehler: Datei konnte nicht erstellt werden!" << endl;
    //     return 1;
    // }

    // // Zeilenweise in Datei schreiben
    // for (const auto& line : scriptLines) {
    //     outfile << line << "\n";
    // }

    // outfile.close();

    //
    const std::string scriptPath = "../Import/exp.proc";

    //
    auto results = executeScript(scriptPath);
    
    //
    LOG << "Skript Returns: " << endl;
    LOG << results << endl;

    //
    return 0;

    std::string cmd = ".\\ProcLang_d execute ../bin/temp.proc";

    LOG << "calling Interpreter" << endl;
    LOG << endl;

    streamWinCommand(cmd, [&](const char* callback){
        LOG << callback;
    });

    return 0;
}