#include "templateDecls.h"

// class Vec : public INativeObject<Vec, std::vector<Variable>>{

// public:

//     static bool initialized;

//     static bool setUpClass(){

//         // register in TypeRegister
//         if(!init("vec", [](){ return new Vec(); })){
//             return false;
//         }

//         // function Register calls

//         return true;
//     }

//     Vec() = default;

//     // virtual ist redundant, die print bleibt überscheibbar
//     void print() const override{
//         LOG << member;
//     }

//     std::unique_ptr<IObject> clone() const override{

//         std::unique_ptr<Vec> v = std::make_unique<Vec>();

//         v->getMember().resize(getMember().size());

//         for(size_t i = 0; i < v->getMember().size(); i++){
//             v->getMember()[i].clone(getMember()[i]);
//         }

//         // static_cast<const Vec&>(*this)
//         return v;
//     };

//     void ping(){
//         LOG << "pong" << endl;
//     }
// };

// bool Vec::initialized = setUpClass();

int main(){

    LOG << "ProcTestEnv exec Skript" << endl;

    // Bspl Skript
    std::vector<std::string> scriptLines = {

        "// Beispielhaftes Skript",
        "int[a,b,c,d] = 0,1,2,3",
        "int ref[ar, br, cr, dr] << a, b, c, d",
        "",
        "if(__ScriptCalledAs__ == __MainProc__){",
        "   ",
        "   // Section wird ausgeführt wenn Skript mit argument execute aufgerufen wird",
        "   log(\"Starte Skript :\", __script__, \" mit Args : [\", __args__, \"]\")",
        "   ",
        "   if(true){",
        "       int x = 0",
        "       ar << x",
        "   }",
        "   if(true){",
        "       if(true){",
        "           return ar",
        "       }",
        "   }",
        "}",
    };

    //
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

    //
    auto results = executeScript("../bin/temp.proc");
    
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