#pragma once

#include "../SemanticAnalysis/AST.h"

#include "InstanceIncludes.h"
#include "BOOL.h"
#include "INT.h"
#include "DOUBLE.h"
#include "STRING.h"

typedef size_t NotationIndex;

enum class IndexNotationOperator{

    Addition,
    Subtraction,
    Multiplication,
};

extern std::map<IndexNotationOperator, std::string> IndexNotationOperatorStrings;

struct TensorExpression{

    //
    static NotationIndex NotationIndexCounter;

    // Inhalt
    std::string label = NULLSTR;
    int tensorOrder = -1;
    std::vector<NotationIndex> notatedIndices;

    //
    TkType Relation;
    IndexNotationOperator Operator;
    std::vector<TensorExpression> children;

    TensorExpression() = default;
    
    // Konstruktion einer Arg node
    TensorExpression(const std::string& labelIn, int tensorOrderIn) : label(labelIn), tensorOrder(tensorOrderIn){

        Relation = TkType::Argument;

        notatedIndices.reserve(tensorOrder);

        for(int i = 0; i < tensorOrder; i++){

            //
            notatedIndices.emplace_back(NotationIndexCounter++);
        }
    }

    std::string toString(size_t depth = 0) const {

        //
        std::string result = "";

        if(depth == 0){

            result += "Res[";

            for(size_t i = 0; i < notatedIndices.size(); i++){

                result += std::to_string(notatedIndices[i]);
                result += i < notatedIndices.size() - 1 ? "," : "";
            }

            result += "] = ";
        }

        if(Relation == TkType::Argument){

            result += label + "[";

            for(size_t i = 0; i < notatedIndices.size(); i++){

                result += std::to_string(notatedIndices[i]);
                result += i < notatedIndices.size() - 1 ? "," : "";
            }

            result += "]";
        }
        else if(Relation == TkType::Operator){
            
            RETURNING_ASSERT(IndexNotationOperatorStrings.contains(Operator), "Unbekannter IndexNotationOperator", "");

            result += depth > 0 ? "(" : "";

            for(size_t i = 0; i < children.size(); i++){

                const TensorExpression& child = children[i];

                result += (i > 0) ? " " + IndexNotationOperatorStrings[Operator] + " " : "";
                result += child.toString(depth + 1);
            }

            result += depth > 0 ? ")" : "";
        }

        return result;
    }

    std::vector<NotationIndex> getUniqueChildIndices() const {

        std::unordered_map<NotationIndex, int> indexCount;
        std::vector<NotationIndex> order;

        for (const auto& child : children) {
            for (auto idx : child.notatedIndices) {
                if (indexCount[idx]++ == 0) {
                    order.push_back(idx);
                }
            }
        }

        std::vector<NotationIndex> uniqueIndices;
        uniqueIndices.reserve(order.size());

        for (auto idx : order) {
            if (indexCount[idx] == 1) {
                uniqueIndices.push_back(idx);
            }
        }

        return uniqueIndices;
    }

    void replaceIndex(NotationIndex oldIndex, NotationIndex newIndex){

        for(auto& idx : notatedIndices){

            if(idx == oldIndex){

                idx = newIndex;
            }
        }

        for(auto& child : children){

            child.replaceIndex(oldIndex, newIndex);
        }
    }

    void replaceIndices(const std::vector<NotationIndex>& oldIndices, const std::vector<NotationIndex>& newIndices){

        RETURNING_ASSERT(oldIndices.size() == newIndices.size(),
            "replaceIndices: oldIndices und newIndices müssen die gleiche Länge haben", );

        for(size_t i = 0; i < oldIndices.size(); i++){

            replaceIndex(oldIndices[i], newIndices[i]);
        }
    }

    bool isValid(){
        return label != NULLSTR && tensorOrder >= 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const TensorExpression& expr);
};

void moveSelfIntoFirstChild(TensorExpression& node);

namespace types{

    class TENSOR_EXPRESSION : public INativeObject<TENSOR_EXPRESSION, TensorExpression>{

    public:

        static int setUpClass();

        TENSOR_EXPRESSION() = default;
        TENSOR_EXPRESSION(TensorExpression* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{

            LOG << getMember();
        }
    };
};