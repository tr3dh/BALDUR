#include "AST.h"

std::ostream& operator<<(std::ostream& os, const ASTNode& obj) {

    for(size_t counter = 0; counter < obj.depth; counter++){
        os << " | ";
    }

    os << "[ASTNode] " << obj.begin  << " - " << obj.end << ", type= " << magic_enum::enum_name(obj.Relation);

    if(obj.Relation == TkType::Operator){
        os << ", Ops= '" << ((obj.Operator < g_numLexerOperators) ? g_lengthSortedLexerOperators[obj.Operator] : "None") << "'";
    }
    else if(obj.Relation == TkType::Argument){
        os << ", Argument= '" << obj.argument << "'";
    }
    else if(obj.Relation == TkType::Constant){
        os << ", Value= '" << obj.value << "'";
    }

    os << ", Args= [";

    for(const auto& child : obj.children){
        os << child.begin << "-" << child.end << ", ";
    }
    os << "]";

    os << endl;

    //
    for(const auto& child : obj.children){
        os << child;
    }

    return os;
}

void convertTokensToAST(ASTNode& Expr, const std::vector<Token>& tokens, const std::string& expressionStr){

    //
    RETURNING_ASSERT(tokens.size() > 0, "An ASTConverter übergebene Token Liste ist leer, übergebe valide Tokenliste",);

    //
    // RETURNING_ASSERT(Expr.begin != Expr.end, "AST Node [" + std::to_string(Expr.begin) + " " + std::to_string(Expr.begin) + "] ist Dead End ",);

    //
    bool singleArgumentNode = (Expr.end - Expr.begin) <= 1;

    // Nur ein argument drin
    if(singleArgumentNode){

        auto& token = tokens[Expr.begin];

        if(tokens[Expr.begin].type == TkType::Paren){
            Expr.Relation = TkType::Params;
        }
        else if(tokens[Expr.begin].type == TkType::Brace && Expr.Relation == TkType::None){
            Expr.Relation = TkType::Section;
        }
        else if(tokens[Expr.begin].type == TkType::Bracket && Expr.Relation == TkType::None){
            Expr.Relation = TkType::Listing;
        }

        if(tokens[Expr.begin].type == TkType::Paren || tokens[Expr.begin].type == TkType::Brace ||
            tokens[Expr.begin].type == TkType::Bracket){

            return;
        }

        // RETURNING_ASSERT(
        //     token.type == TkType::Argument || token.type == TkType::Constant,
        //     "Token einer Ein-Token Node ist weder Operator noch Argument",);

        Expr.Relation = token.type;

        if(Expr.Relation == TkType::Argument || token.type == TkType::Argument){

            Expr.Relation = TkType::Argument;
            Expr.argument = expressionStr.substr(token.position, token.length);
        }
        else if(Expr.Relation == TkType::String){
            
            Expr.argument = expressionStr.substr(token.position, token.length);
        }
        else if(Expr.Relation == TkType::Constant || token.type == TkType::Constant){

            std::string numStr = expressionStr.substr(token.position, token.length);

            if(!numStr.contains('.')){

                Expr.constantType = ConstantType::SIZE;
            }
            else if(numStr.size() - numStr.find('.') < 7){

                Expr.constantType = ConstantType::FLOAT;
            }
            else{

                Expr.constantType = ConstantType::DOUBLE;
            }

            Expr.Relation = TkType::Constant;
            Expr.value = string::convert<double>(expressionStr.substr(token.position, token.length));
        }
    }
    else {

        //
        size_t minHierarchie = tokens[Expr.begin].hierarchie;

        //
        for(size_t idx = Expr.begin; idx < Expr.end; idx++){

            //
            auto& token = tokens[idx];

            //
            if(token.hierarchie < minHierarchie){

                minHierarchie = token.hierarchie;
            }
        }

        //
        size_t primOperator = g_numLexerOperators;

        //
        for(size_t idx = Expr.begin; idx < Expr.end; idx++){

            //
            auto& token = tokens[idx];

            //
            if(token.hierarchie == minHierarchie && token.Operator < g_numLexerOperators &&
                g_hierarchieOfLengthSortedLexerOperators[token.Operator] <
                ((primOperator < g_numLexerOperators) ? g_hierarchieOfLengthSortedLexerOperators[primOperator] : g_numLexerOperators)){

                primOperator = token.Operator;
            }
        }

        //
        // LOG << "Hierarch " << minHierarchie << " Prim Ops " <<
        //     ((primOperator < g_numLexerOperators) ? g_lengthSortedLexerOperators[primOperator] : "None") << endl;

        auto isMatchingBrace = [&](size_t begin, size_t end) {

            // zwischen begin und end - 1 dürfen keine anderen brackets der gleichen hierarchie liegen
            TkType beginType = tokens[begin].type;
            
            //
            if(beginType != TkType::Paren && beginType != TkType::Brace && beginType != TkType::Bracket){

                return false;
            }

            TkType endType = tokens[end-1].type;

            if(!((tokens[begin].type == TkType::Brace && tokens[end-1].type == TkType::CLBrace) ||
                (tokens[begin].type == TkType::Paren && tokens[end-1].type == TkType::CLParen) ||
                (tokens[begin].type == TkType::Bracket && tokens[end-1].type == TkType::CLBracket))){

                return false;
            }

            Token firstToken = tokens[begin];
            Token lastToken = tokens[end-1];

            if(firstToken.hierarchie != lastToken.hierarchie){

                return false;
            }

            size_t hierarch = firstToken.hierarchie;

            if(begin + 1 >= end - 1){
                return true;
            }

            for(size_t tokenIdx = begin + 1; tokenIdx < end - 1; tokenIdx++){

                if((tokens[tokenIdx].type == beginType || tokens[tokenIdx].type == endType) &&
                    tokens[tokenIdx].hierarchie == hierarch){

                        return false;
                    }
                }

            return true;
        };

        // LOG << Expr.begin << " " << Expr.end << endl;

        //
        if(primOperator < g_numLexerOperators){

            //
            Expr.Relation = TkType::Operator;
            Expr.Operator = primOperator;
        }
        else if(primOperator == g_numLexerOperators &&
            tokens[Expr.begin].hierarchie == minHierarchie &&
            (tokens[Expr.begin].type == TkType::Paren || tokens[Expr.begin].type == TkType::Brace || tokens[Expr.begin].type == TkType::Bracket) &&
            (Expr.begin - Expr.end > 2 || tokens[Expr.begin + 1].hierarchie > minHierarchie) &&
            isMatchingBrace(Expr.begin, Expr.end) &&
            tokens[Expr.begin].hierarchie == tokens[Expr.end-1].hierarchie){

            if(tokens[Expr.begin].type == TkType::Paren){
                Expr.Relation = TkType::Params;
                primOperator = g_kommaOperatorIndex;
            }
            else if(tokens[Expr.begin].type == TkType::Brace){
                Expr.Relation = TkType::Section;
            }
            else if(tokens[Expr.begin].type == TkType::Bracket && Expr.Relation == TkType::None){
                Expr.Relation = TkType::Listing;
            }

            if(Expr.end - Expr.begin < 1){
                return;
            }
            
            Expr.end -= 1;
            minHierarchie = tokens[Expr.begin].hierarchie + 1;
            // primOperator = Expr.Relation == TkType::Section ? g_colonOperatorIndex : g_kommaOperatorIndex;
        }
        else{

            //
            Expr.Relation = TkType::Chain;

            //
            size_t argBegin = 0;
            bool isolateHierarchArg = false;

            //
            for(size_t tkIdx = Expr.begin; tkIdx < Expr.end; tkIdx++){

                const auto& tk = tokens[tkIdx];

                if (tk.type == TkType::CLBrace || tk.type == TkType::CLBracket || tk.type == TkType::CLParen) {
                    continue;
                }

                if((tokens[tkIdx].hierarchie == minHierarchie || tkIdx >= Expr.end - 1)
                    && isolateHierarchArg){
                    
                    isolateHierarchArg = false;

                    Expr.children.emplace_back();

                    auto& node = Expr.children.back();
                    node.begin = argBegin;
                    node.end = tkIdx;
                }

                if(tokens[tkIdx].hierarchie == minHierarchie && tokens[tkIdx].type != TkType::Paren &&
                    tokens[tkIdx].type != TkType::Brace && tokens[tkIdx].type != TkType::Bracket){

                    Expr.children.emplace_back();

                    auto& node = Expr.children.back();

                    node.begin = tkIdx;
                    node.end = tkIdx + 1;
                }
                else if(!isolateHierarchArg){

                    isolateHierarchArg = true;
                    argBegin = tkIdx;
                }
            }

            // offene Blöcke
            if (isolateHierarchArg) {

                Expr.children.emplace_back();
                auto& node = Expr.children.back();
                node.begin = argBegin;
                node.end = Expr.end;
            }
        }

        // ja nachdem wie Syntax aussehen soll
        if(Expr.Relation == TkType::Operator
            
            // zeile auskommentieren wenn section und params nicht automatisch nach , / ; gesplittet werden sollen
            || Expr.Relation == TkType::Params // || Expr.Relation == TkType::Listing || Expr.Relation == TkType::Section
        ){

            size_t beginWithToken = Expr.Relation == TkType::Operator ? Expr.begin : Expr.begin + 1;
            size_t argBegin = beginWithToken;

            //
            for(size_t idx = beginWithToken; idx < Expr.end; idx++){

                auto& token = tokens[idx];

                if(token.hierarchie == minHierarchie && token.Operator == primOperator){

                    if(argBegin == idx){

                        argBegin = idx + 1;
                        continue;
                    }

                    //
                    Expr.children.emplace_back();

                    auto& node = Expr.children.back();
                    node.begin = argBegin;
                    node.end = idx;

                    argBegin = idx + 1;
                }
            }

            //
            if(argBegin < Expr.end){

                // hinteres argument
                Expr.children.emplace_back();

                auto& node = Expr.children.back();
                node.begin = argBegin;
                node.end = Expr.end;
            }
        }
        else if(Expr.Relation == TkType::Params || Expr.Relation == TkType::Listing || Expr.Relation == TkType::Section){

            // Leere Listings, Sections werden sonst mit dem CLBracket Token befüllt
            if(Expr.end - Expr.begin <= 1){
                return;
            }

            //
            Expr.children.emplace_back();

            auto& node = Expr.children.back();
            node.begin = Expr.begin + 1;
            node.end = Expr.end;
        }
    }

    //
    for(auto& child : Expr.children){
        child.depth = Expr.depth + 1;
    }

    //
    for(auto& child : Expr.children){
        convertTokensToAST(child, tokens, expressionStr);
    }

    if(/* Expr.Relation == TkType::Params || */ Expr.Relation == TkType::Listing || Expr.Relation == TkType::Section){

        // Leere Listings, Sections werden sonst mit dem CLBracket Token befüllt
        if(Expr.children.size() != 1){
            return;
        }

        const ASTNode node = Expr.children.back();
        
        if(node.Relation == TkType::Operator && node.Operator == g_numLexerOperators){
            Expr.children.clear();
        }
    }

    return;
}