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

    // Bspl Skript
    std::vector<std::string> scriptLines = {
        "int d = 8;",
        "ref y << d;"
        "ref z << y;",
        "z = true;",
        "int g = bool();",
        "double a = 4.2345;"
        //"ref g << d;",
        //"g = 10;",
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

    executeScript("../bin/temp.proc");

    // std::string cmd = ".\\ProcLang_d execute ../bin/temp.proc";

    // LOG << "calling Interpreter" << endl;
    // LOG << endl;

    // streamWinCommand(cmd, [&](const char* callback){
    //     LOG << callback;
    // });

    // std::cin.get();

    return 0;
}