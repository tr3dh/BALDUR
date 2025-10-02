#include "templateDecls.h"

class Str : public INativeObject<Str, std::string>{

public:

    static bool initialized;

    static bool setUpClass(){

        // register in TypeRegister
        if(!init("str", [](){ return new Str(); })){
            return false;
        }

        // function Register calls

        return true;
    }

    void ping(){
        LOG << "pong" << endl;
    }
};

bool Str::initialized = setUpClass();

class Str2 : public INativeObject<Str2, std::string>{

public:

    static bool initialized;

    void ping(){
        LOG << "pong" << endl;
    }
};

bool Str2::initialized = init("str2", [](){ return new Str2(); });

int main(){

    registerFunction("Add", {Str::typeIndex, Str::typeIndex},
        [__functionLabel__ = "AddStrStr", __numArgs__ = 2](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            // Casts
            Str* str1 = static_cast<Str*>(inputs[0]);
            Str* str2 = static_cast<Str*>(inputs[1]);

            //
            str1->ping();
            LOG << endl;
    });

    registerMemberFunction(Str::typeIndex, "ping", {},
        [__functionLabel__ = "pingStr", __numArgs__ = 0](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

            // Asserts
            ASSERT_IS_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            // Casts
            Str* castedMember = static_cast<Str*>(member);

            //
            LOG << "pinging Member" << endl;

            //
            castedMember->ping();
            LOG << endl;
    });

    registerStaticFunction(Str::typeIndex, "getIdx", {},
        [__functionLabel__ = "getIdxStr", __numArgs__ = 0](std::vector<IObject*>& returns, const std::vector<IObject*>& inputs, IObject* member){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);

            //
            LOG << "pinging Class" << endl;
            LOG << Str::typeIndex << endl;
            
            LOG << endl;
    });

    LOG << g_TypeRegister << endl;
    LOG << g_FunctionRegister << endl;
    LOG << g_MemberFunctionRegisters << endl;
    LOG << g_StaticFunctionRegisters << endl;

    Str a, b, c;
    Str2 aa, bb, cc;
    std::vector<IObject*> rets;

    callFunction("Add", rets, {&a,&b});
    callFunction("Add", rets, {&a,&bb});
    
    callMemberFunction("ping", rets, {}, &a);

    callStaticFunction("str", "getIdx", rets, {});
    callStaticFunction(1, "getIdx", rets, {});

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