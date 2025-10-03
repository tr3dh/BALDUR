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

    Str() = default;

    Str(const std::string& str) : INativeObject<Str, std::string>(str){
        
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

struct Variable;

struct Variable{

    std::unique_ptr<IObject> ownedObject = nullptr;
    IObject* referencedObject = nullptr;

    Variable() = default;

    void constructByObject(IObject* object) {

        ownedObject.reset(object);              // Ownership übernehmen
        referenceSelf();
    }

    // selbst Referenzierung
    // in Folge können Referenzen und Owner in gleicher Art und Weise auf den owned Value (unique_ptr) zugreifen
    // macht Auswertung einfach konsistenter
    void referenceSelf(){

        RETURNING_ASSERT(isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        RETURNING_ASSERT(ownedObject != nullptr, "Selbstreferenzierung ohne Ownership eines Wertes versucht",);
        referencedObject = ownedObject.get();
    }

    void dereferenceSelf(){

        referencedObject = nullptr;
    }

    void move(Variable& other){
        
        RETURNING_ASSERT(other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        RETURNING_ASSERT(!isReference() && !other.isReference(), "Move von Referenz Objekten versucht",);
        ownedObject = std::move(other.ownedObject);

        other.dereferenceSelf();
        referenceSelf();
    }

    void swap(Variable& other){
        
        RETURNING_ASSERT(isValid() && other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        RETURNING_ASSERT(!isReference() && !other.isReference(), "Move von Referenz Objekten versucht",);
        std::swap(ownedObject, other.ownedObject);

        referenceSelf();
        other.referenceSelf();
    }

    void clone(const Variable& other){

        RETURNING_ASSERT(other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        ownedObject = other.getData()->clone();
        referenceSelf();
    }

    void reference(const Variable& other){
        
        RETURNING_ASSERT(other.isValid(), "MM Semantik für Invalide Variable aufgerufen",);
        referencedObject = other.getData();
    }

    bool isValid() const {

        return !(ownedObject == nullptr && referencedObject == nullptr);
    }

    void inValidate(){

        ownedObject.reset(nullptr);
    }

    bool isReference() const {
        
        if(!isValid()){
            return false;
        }

        return ownedObject == nullptr;
    }

    IObject* getData() const{

        // nicht nötig aufgrund von Eigenref
        // if(!isReference()){
        //     return ownedObject.get();
        // }

        return referencedObject;
    }

    friend std::ostream& operator<<(std::ostream& os, const Variable& var){

        if(!var.isValid()){ 

            LOG << "INVALID_VARIABLE";
            return os;
        }

        LOG << "Type : " << var.referencedObject->getTypeIndex() << " ";

        if(var.isReference()){

            LOG << "Ref ";
        }
        else{

            LOG << "    ";
        }

        LOG << "| Value '";
        var.getData()->print();
        LOG << "'";

        LOG << " | Validcheck : " << (var.isValid() ? "true" : "false"); 

        return os;
    }
};


class Vec : public INativeObject<Vec, std::vector<Variable>>{

public:

    static bool initialized;

    static bool setUpClass(){

        // register in TypeRegister
        if(!init("vec", [](){ return new Vec(); })){
            return false;
        }

        // function Register calls

        return true;
    }

    Vec() = default;

    // virtual ist redundant, die print bleibt überscheibbar
    void print() const override{
        LOG << member;
    }

    std::unique_ptr<IObject> clone() const override{

        std::unique_ptr<Vec> v = std::make_unique<Vec>();

        v->getMember().resize(getMember().size());

        for(size_t i = 0; i < v->getMember().size(); i++){
            v->getMember()[i].clone(getMember()[i]);
        }

        // static_cast<const Vec&>(*this)
        return v;
    };

    void ping(){
        LOG << "pong" << endl;
    }
};

bool Vec::initialized = setUpClass();

int main(){

    std::map<std::string, Variable> variableTable;

    variableTable["a"] = Variable();
    variableTable["b"] = Variable();
    variableTable["c"] = Variable();
    variableTable["d"] = Variable();
    variableTable["e"] = Variable();
    variableTable["f"] = Variable();

    variableTable["a"].constructByObject(new Str("StrA"));
    variableTable["f"].reference(variableTable["a"]);
    variableTable["c"].constructByObject(new Str("StrB"));

    variableTable["b"].move(variableTable["c"]);
    variableTable["b"].swap(variableTable["a"]);
    variableTable["c"].clone(variableTable["a"]);

    static_cast<Str*>(variableTable["c"].getData())->getMember() += " cloned Instance";

    Vec* cvec = new Vec();
    cvec->getMember().resize(2);
    cvec->getMember()[0].reference(variableTable["a"]);

    variableTable["d"].constructByObject(cvec);
    variableTable["e"].clone(variableTable["d"]);

    static_cast<Vec*>(variableTable["e"].getData())->getMember()[1].constructByObject(new Str("Diff"));

    LOG << variableTable << endl;

    return 0;

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