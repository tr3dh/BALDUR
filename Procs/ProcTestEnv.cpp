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

    //
    LOG << "ProcTestEnv exec Skript" << endl;

    // Bspl Skript
    std::vector<std::string> scriptLines = {

        "// Beispielhaftes Skript",
        "",
        "struct expObject{",
        "",
        "   static{",
        "       int numInstances = 1",
        "   }",
        "   int intMember = 15",
        "   bool boolMember = true",
        "   int ref numInst << numInstances",
        "}",
        "",
        "decl returnRefX(int ref a, int ref b, int ref c, int chooseX){",
        "   ",
        "   if(chooseX == 1){return a}",
        "   else if(chooseX == 2){return b}",
        "   else if(chooseX == 3){return c}",
        "}",
        // Definition von Bool Addition
        "decl __addAssign__(bool ref a, bool ref b){",
        "   a x|= b",
        "}",
        "",
        "int [g_fibonacciCalls, g_facultyCalls]",
        "",
        "decl faculty(int ref member){",
        "   ",
        "   g_facultyCalls++",
        "   ",
        "   ",
        "   xIf(member == 0, member == 1){ return 1 }",
        "   return [member * faculty(member - 1)]",
        "}",
        "",
        "decl fibonacci(int ref member){",
        "   ",
        "   g_fibonacciCalls++",
        "   ",
        "   if(member <= 1){ return member }",
        "   return [fibonacci(member - 1) + fibonacci(member - 2)]",
        "}",
        "   ",
        "decl test(ref member){",
        "   ",
        "   return [member]",
        "}",
        "",
        "decl main(){",
        "   ",
        "   // Fakultäten",
        "   for([int [i, numFaculty] = 0,10], i < numFaculty, i++){",
        "       //",
        "       log(\"Faculty(\", i,\") =\", faculty(i))",
        "   }",
        "   ",
        "   log(\" \")",
        "   // Fibonacci",
        "   for([int [i, numFibonacci] = 0,20], i < numFibonacci, i++){",
        "       //",
        "       log(\"Fibonacci[\", i,\"] :\", fibonacci(i))",
        "   }",
        "   ",
        "   log(\" \")",
        "   log(\"faculty called :\", g_facultyCalls, \"times\")",
        "   log(\"fibonacci called :\", g_fibonacciCalls, \"times\")",
        "   ",
        // Testen von im Skript definierter Bool Addition
        "   log(\" \")",
        "   log(\"Bool Addition Result :\", true + true)",
        //
        "   log(\" \")",
        "   int [a,b,c] = 0,1,2",
        "   log(\"Returned Ref :\"); logRes(returnRefX(a, b, c, 2))",
        "   ",
        "   return [0]",
        "}",
        "if(calledAs(__MainProc__)){",
        "   ",
        "   // Section wird ausgeführt wenn Skript mit argument execute aufgerufen wird",
        "   log(\"Starte Skript :\", __script__, \" mit Args : [\", __args__, \"]\")",
        "   ",
        "   expObject obj",
        "   log(obj)",
        "   return 0// [main()]",
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