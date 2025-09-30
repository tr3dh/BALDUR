#include "templateDecls.h"

class Str : public IObject{

public:
    TypeIndex getTypeIndex() override{
        return -1;
    }

    void ping(){
        LOG << "pong" << endl;
    }
};

int main(){

    g_FunctionRegister.registerFunction("add", {Str().getTypeIndex(), Str().getTypeIndex()},
        [&](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

            // Asserts
            RETURNING_ASSERT(member == nullptr, "Funktion Add erwartet keinen validen Member",);
            RETURNING_ASSERT(inputs.size() == 2, "Add Funktion akzeptiert nur genau zwei input Parameter",);

            // Casts
            Str* str1 = static_cast<Str*>(inputs[0]);
            Str* str2 = static_cast<Str*>(inputs[1]);

            //
            str1->ping();
    });

    Str a, b, c;
    std::vector<IObject*> rets;

    g_FunctionRegister.callFunction("add", rets, {&a,&b});
    
    return 0;

    // Bspl Skript
    std::vector<std::string> scriptLines = {
        "// Das ist ein BeispielSkript",
        "   db = 0.3;",
        "   db2 = 0.5;",
        "   explicit INT i = 1.5;",
        "   // exp = xxx;",
        "   exp = db - db2 - \"hallo\" + 1;",
        "   // exp = db - i;",
        "   worldStr = \"Welt\";",
        "   STRING str = worldStr + \" hallo\";",
        "   strstrstr = str - \"lo\";"
        "   strstr = \"34\";",
    };

    std::ofstream outfile("../bin/temp.proc");
    if (!outfile) {
        std::cerr << "Fehler: Datei konnte nicht erstellt werden!" << endl;
        return 1;
    }

    // Zeilenweise in Datei schreiben
    for (const auto& line : scriptLines) {
        outfile << line << "\n";
    }

    outfile.close();

    // executeScript("../bin/temp.proc");

    // std::string cmd = ".\\ProcLang_d execute ../bin/temp.proc";

    // LOG << "calling Interpreter" << endl;
    // LOG << endl;

    // streamWinCommand(cmd, [&](const char* callback){
    //     LOG << callback;
    // });

    // std::cin.get();

    return 0;
}