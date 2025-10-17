#include "IdentifyNodeCases.h"

//
const std::vector<std::string> g_ifKonjunctions = {"if", "If", "nIf", "rIf", "xIf"};

//
bool IsIfStatement(const ASTNode& node){

    if(node.children.size() < 3){ return false; }

    // für [ifkonjunktion params section] || [else ifkonjunktion params section] 
    if(node.children.size() != 3 && node.children.size() != 4){
        return false;
    }

    //
    const ASTNode ifKonjunctionNode = node.children[node.children.size() - 3];

    //
    if(ifKonjunctionNode.Relation != TkType::Argument){
        return false;
    }

    //
    return std::find(g_ifKonjunctions.begin(), g_ifKonjunctions.end(), ifKonjunctionNode.argument) != g_ifKonjunctions.end();
}

bool IsElseSection(const ASTNode& node){
    
    if(node.children.size() < 2){ return false; }

    if(node.children.size() != 2){
        return false;
    }

    if(node.children[0].Relation != TkType::Argument || node.children[1].Relation != TkType::Section){
        return false;
    }
    
    if(node.children[0].argument == "else"){
        return true;
    }

    return false;
}

//
bool IsWhileLoop(const ASTNode& node){

    if(node.children.size() < 3){ return false; }

    if(node.children[0].argument != "while"){
        return false;
    }

    RETURNING_ASSERT(node.children.size() == 3, "a", false);
    RETURNING_ASSERT(node.children[0].Relation == TkType::Argument &&
        node.children[1].Relation == TkType::Params && node.children[2].Relation == TkType::Section, "b", false);

    return true;
}

//
bool IsForLoop(const ASTNode& node){

    if(node.children.size() < 3){ return false; }

    if(node.children[0].argument != "for"){
        return false;
    }

    RETURNING_ASSERT(node.children.size() == 3, "a", false);
    RETURNING_ASSERT(node.children[0].Relation == TkType::Argument &&
        node.children[1].Relation == TkType::Params && node.children[2].Relation == TkType::Section, "b", false);

    RETURNING_ASSERT(node.children[1].children.size() == 3, "c", false);

    return true;
}

bool IsStaticSection(const ASTNode& node){
    
    if(node.children.size() < 2){ return false; }

    if(node.children.size() != 2){
        return false;
    }

    if(node.children[0].Relation != TkType::Argument || node.children[1].Relation != TkType::Section){
        return false;
    }
    
    if(node.children[0].argument == "static"){
        return true;
    }

    return false;
}

//
bool IsConstructionCall(const ASTNode& node){

    if(node.children.size() < 2){
        return false;
    }

    // -3  -2  -1
    // int ref a 
    //      ^----- gechecktes Keyword
    if(node.children[node.children.size() - 2].Relation != TkType::Argument){
        return false;
    }

    if(typeForKeywordExists(node.children[node.children.size() - 2].argument) || node.children[node.children.size() - 2].argument == "ref"){
        return true;
    }

    return false;
}

//
bool IsFunctionCall(const ASTNode& node){

    if(node.children.size() < 2){ return false; }

    return node.children.size() == 2 &&
        node.children[0].Relation == TkType::Argument && node.children[1].Relation == TkType::Params;
}