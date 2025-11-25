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
    Transposition,
    Inversion,
    Trace,

    Diff,

    Arbitary,
};

extern std::map<IndexNotationOperator, std::string> IndexNotationOperatorStrings;

enum class TensorExpressionOperator;
class IndexNotatedTensorExpression;

extern std::map<TensorExpressionOperator, void(IndexNotatedTensorExpression::*)(const IndexNotatedTensorExpression&)> operatorFunctions;

enum class TensorExpressionOperator{
    
    None,

    Addition,
    Subtraction,
    Multiplication,

    DotProduct,
    CrossProduct,
    DyadicProduct,
    CrossingDoubleContraction,
    MirroringDoubleContraction,
    
    Negation,
    Inversion,
    Transposition,
    Trace,
    Section,

    Diff,

    Arbitary,
};

struct TensorExpression;

extern std::map<TensorExpressionOperator, std::string> TensorExpressionOperatorStrings;
extern std::map<TensorExpressionOperator, void (TensorExpression::*)(const TensorExpression&)> operatorMemberFunctions;
extern std::map<TensorExpressionOperator, void (TensorExpression::*)()> singleArgOperatorMemberFunctions;

extern std::map<std::pair<TensorExpression, TensorExpression>, TensorExpression> tensorExpressionDiffs;
extern std::map<std::pair<TensorExpression, TensorExpression>, TensorExpression> tensorExpressionDiffTemplates;

struct TensorExpression{

    //
    typedef std::map<TensorExpression, TensorExpression> substitutionMap;

    //
    static void replaceBySubstitutions(TensorExpression& expr, const substitutionMap& subsMap);
    static void assembleSubstitutionMap(const TensorExpression& tmplExpr, const TensorExpression& expr, substitutionMap& subsMap);

    //
    TkType Relation;
    TensorExpressionOperator Operator;

    std::string label = NULLSTR;
    int tensorOrder = -1;

    int contractNIndices = 0;

    std::vector<TensorExpression> children;

    TensorExpression();
    
    // Konstruktion einer Arg node
    TensorExpression(const std::string& labelIn, int tensorOrderIn);

    bool operator==(const TensorExpression& other) const;

    //
    void moveSelfIntoFirstChild();

    // Operatoren
    void addAssign(const TensorExpression& other);
    void subAssign(const TensorExpression& other);
    void mulAssign(const TensorExpression& other);
    void dotProductAssign(const TensorExpression& other);
    void crossProductAssign(const TensorExpression& other);
    void dyadProductAssign(const TensorExpression& other);
    void mirroringDoubleContractionAssign(const TensorExpression& other);
    void crossingDoubleContractionAssign(const TensorExpression& other);
    void transposeAssign();
    void inverseAssign();

    //
    void convertToTemplate();
    bool isTemplatedNode() const;
    bool isTemplate() const;

    //
    std::vector<std::string> getRawLabels();

    //
    void rawDiffAssign(const TensorExpression& other);
    void diffAssign(const TensorExpression& other);

    void traceAssign(int contractIndices);
    void traceAssign();

    void sectionAssign();

    //
    TensorExpression rebuild();

    //
    std::string toString(size_t depth = 0) const;

    friend std::ostream& operator<<(std::ostream& os, const TensorExpression& expr);
};

void moveSelfIntoFirstChild(TensorExpression& node);

bool operator<(const TensorExpression& lhs, const TensorExpression& rhs);

struct IndexNotatedTensorExpression{

    //
    static NotationIndex NotationIndexCounter;

    // Inhalt
    std::string label = NULLSTR;
    int tensorOrder = -1;
    std::vector<NotationIndex> notatedIndices;
    // mutable std::vector<NotationIndex> cachedSortedIndices;

    //
    TkType Relation;
    IndexNotationOperator Operator;
    std::vector<IndexNotatedTensorExpression> children;

    //
    IndexNotatedTensorExpression();
    IndexNotatedTensorExpression(const std::string& labelIn, int tensorOrderIn);

    //
    void moveSelfIntoFirstChild();

    bool isValid();

    void fillIndices();
    void replaceIndex(NotationIndex oldIndex, NotationIndex newIndex);
    void replaceIndices(const std::vector<NotationIndex>& oldIndices, const std::vector<NotationIndex>& newIndices);
    std::vector<NotationIndex> getUniqueChildIndices() const;

    //
    const std::vector<NotationIndex>& getSortedIndices();

    // Operatoren
    void addAssign(const IndexNotatedTensorExpression& other);
    void subAssign(const IndexNotatedTensorExpression& other);
    void mulAssign(const IndexNotatedTensorExpression& other);
    void dotProductAssign(const IndexNotatedTensorExpression& other);
    void crossProductAssign(const IndexNotatedTensorExpression& other);
    void dyadProductAssign(const IndexNotatedTensorExpression& other);
    void mirroringDoubleContractionAssign(const IndexNotatedTensorExpression& other);
    void crossingDoubleContractionAssign(const IndexNotatedTensorExpression& other);
    void transposeAssign();
    void inverseAssign();

    void traceAssign(int contractIndices);
    void traceAssign();

    std::string toString(size_t depth = 0) const;
    friend std::ostream& operator<<(std::ostream& os, const IndexNotatedTensorExpression& expr);
};

void moveSelfIntoFirstChild(IndexNotatedTensorExpression& node);

namespace types{

    class TENSOR_EXPRESSION : public INativeObject<TENSOR_EXPRESSION, TensorExpression>{

    public:

        static int setUpClass();

        TENSOR_EXPRESSION() = default;
        TENSOR_EXPRESSION(TensorExpression* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override;
    };

    class INDEX_NOTATED_TENSOR_EXPRESSION : public INativeObject<INDEX_NOTATED_TENSOR_EXPRESSION, IndexNotatedTensorExpression>{

    public:

        static int setUpClass();

        INDEX_NOTATED_TENSOR_EXPRESSION() = default;
        INDEX_NOTATED_TENSOR_EXPRESSION(IndexNotatedTensorExpression* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{

            LOG << getMember().toString();
        }
    };
};