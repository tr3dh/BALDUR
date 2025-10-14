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

        "enva, envb, envc",
        "int[mv1, mv2, mv3] = 1,2,3",
        "mv <- mv1",
        "int ii; int ref yy",
        "int a = 40",
        "yy << a",
        "for({}, a < 20, {}){",
        "   ii = a",
        "   log(a)",
        "   a++",
        "}",
        "log([4,6,8 % 3,5,6])",
        "for(int i = 0, i < 1000000, i++){",
        "   {}",
        "}",
        "yy << a;"
        "int i = 0",
        "while(i < 20){",
        "   //sleep(0.1)",
        "   log(\"while loop : frame\", i)",
        "   i++",
        "}",
        "nIf([1, 3 == 1, 2]){",
        "   ",
        "   log(\"If Statement wird ausgeführt\")",
        "}",
        "else If(1 == 1){",
        "   log(\"Else If Statement wird ausgeführt\")",
        "   If(1 == 1){",
        "       log(\"Verschachteltes If Statement ausgeführt\")",
        "   }",
        "}",
        "else{",
        "   int a = 2; int b = 3;"
        "   log(\"Else Statement wird ausgeführt\", b)",
        "};",

        "xIf(1 == 1, 1 == 1){ log(\"2. If Statement wird ausgeführt\") };",
        "else{ log(\"2. Else Statement wird ausgeführt\") };",

        "log(\"...\");"
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
    executeScript("../bin/temp.proc");

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