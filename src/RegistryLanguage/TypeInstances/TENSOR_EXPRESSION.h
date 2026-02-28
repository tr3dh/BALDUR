#pragma once

#include "../SemanticAnalysis/AST.h"

#include "InstanceIncludes.h"
#include "ARGS.h"
#include "BOOL.h"
#include "INT.h"
#include "DOUBLE.h"
#include "STRING.h"

typedef int NotationIndex;

enum class IndexNotationOperator{

    Addition,
    Subtraction,
    Multiplication,
    Transposition,
    Inversion,
    Trace,
    Determinant,
    Frobenius,
    Macaulay,
    Signum,
    Sqrt,

    Diff,

    Arbitary,
};

extern std::map<IndexNotationOperator, std::string> IndexNotationOperatorStrings;

enum class TensorExpressionOperator;
class IndexNotatedTensorExpression;

extern std::map<TensorExpressionOperator, void(IndexNotatedTensorExpression::*)(const IndexNotatedTensorExpression&)> operatorFunctions;

//
extern bool g_compareTemplateDependencies;

enum class TensorExpressionOperator{
    
    None,

    Addition,
    Subtraction,
    Multiplication,

    DotProduct,
    ContractingDotProduct,
    CrossProduct,
    DyadicProduct,
    CrossingDoubleContraction,
    MirroringDoubleContraction,
    
    Negation,
    Inversion,
    Transposition,
    Trace,
    Determinant,
    Frobenius,
    Macaulay,
    Signum,
    Sqrt,
    Ones,
    Zeros,
    Identity,

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

extern std::map<TensorExpression, TensorExpression> tensorExpressionSimplifications;

struct SubstitutionComparator {

    bool operator()(const TensorExpression& a, const TensorExpression& b) const;
};

//
extern bool unwrapOperands;

//
extern bool usingTullio;

//
struct TensorExpression{

    //
    typedef std::map<TensorExpression, TensorExpression, SubstitutionComparator> substitutionMap;

    //
    static void replaceBySubstitutions(TensorExpression& expr, const substitutionMap& subsMap);
    static void rawReplaceBySubstitutions(TensorExpression& expr, const substitutionMap& subsMap);
    static bool assembleSubstitutionMap(const TensorExpression& tmplExpr, const TensorExpression& expr, substitutionMap& subsMap, bool disableLog = false);
    static bool structurallyEqual(const TensorExpression& a, const TensorExpression& b);

    static int minCnstLimit;

    //
    TkType Relation = TkType::None;
    TensorExpressionOperator Operator = TensorExpressionOperator::None;

    bool isConstant = false;
    bool isArgTmpl = false;

    int contractNIndices = 0;
    int tensorOrder = -1;
    float value = 0.0f;

    std::string label = NULLSTR;

    std::vector<TensorExpression> children = {};
    std::vector<int> dimensions = {};

    TensorExpression();
    
    // Konstruktion einer Arg node
    TensorExpression(const std::string& labelIn, int tensorOrderIn);
    TensorExpression(const std::string& labelIn, int tensorOrderIn, const std::vector<int>& dimensionsIn);
    TensorExpression(float valueIn);

    bool operator==(const TensorExpression& other) const;

    //
    void moveSelfIntoFirstChild();

    //
    TensorExpression asExternalNode(const std::string& label);
    bool contains(const TensorExpression& other) const;
    int countOccurences(const TensorExpression& other) const;
    bool containsDimensions() const;
    
    //
    bool isWrapped() const;
    bool isUnWrapped() const;

    // Operatoren
    void addAssign(const TensorExpression& other);
    void subAssign(const TensorExpression& other);
    void mulAssign(const TensorExpression& other);
    void dotProductAssign(const TensorExpression& other);
    void contractingDotProductAssign(const TensorExpression& other);
    void crossProductAssign(const TensorExpression& other);
    void dyadProductAssign(const TensorExpression& other);
    void mirroringDoubleContractionAssign(const TensorExpression& other);
    void crossingDoubleContractionAssign(const TensorExpression& other);
    void transposeAssign();
    void inverseAssign();
    void zerosAssign();
    void onesAssign();
    void identityAssign();

    //
    bool isValid() const;

    //
    bool isCommutativ() const;

    //
    void convertToTemplate();
    void convertToConstantTemplate();
    void convertToConstantTemplate(const std::string& labelIn);
    void convertToArgTemplate();

    bool isTemplatedNode() const;
    bool isTemplate() const;

    bool isConstantTemplate() const;
    bool isInstanceTemplate() const;
    bool isArgTemplate() const;

    bool isTemplateDependencie() const;
    bool containsTemplateDependencie() const;

    //
    size_t getNumOfUniqueNodes() const;
    size_t getNumOfNodes() const;
    size_t getNumOfExternalNodes() const;

    //
    std::vector<const TensorExpression*> getUniqueExternalNodes() const;

    //
    std::vector<std::string> getRawLabels();

    //
    void rawDiffAssign(const TensorExpression& other);
    void diffAssign(const TensorExpression& other);
    void diffAssign(const TensorExpression& other, int times);

    void traceAssign(int contractIndices);
    void traceAssign();

    void determinantAssign();
    void frobeniusAssign();
    void macaulayAssign();
    void signumAssign();
    void sqrtAssign();

    void sectionAssign();

    //
    TensorExpression rebuild() const;
    TensorExpression unwrap() const;
    TensorExpression wrap() const;

    //
    bool simplifyOnce();
    void simplify();
    
    //
    std::string toString(size_t depth = 0) const;

    friend std::ostream& operator<<(std::ostream& os, const TensorExpression& expr);
};

void moveSelfIntoFirstChild(TensorExpression& node);

bool operator<(const TensorExpression& lhs, const TensorExpression& rhs);

//
struct IndexNotatedTensorExpression{

    //
    static NotationIndex NotationIndexCounter;

    //
    bool isConstant = false;
    float value = 0.0f;

    int tensorOrder = -1;

    // Inhalt
    std::string label = NULLSTR;
    
    std::vector<NotationIndex> notatedIndices;
    std::vector<int> dimensions = {};
    // mutable std::vector<NotationIndex> cachedSortedIndices;

    //
    TkType Relation;
    IndexNotationOperator Operator;
    std::vector<IndexNotatedTensorExpression> children;

    //
    IndexNotatedTensorExpression();
    IndexNotatedTensorExpression(const std::string& labelIn, int tensorOrderIn);
    IndexNotatedTensorExpression(float valueIn);
    IndexNotatedTensorExpression(const std::string& labelIn, int tensorOrderIn, const std::vector<int>& dimensionsIn);

    //
    IndexNotatedTensorExpression asExternalNode(const std::string& label) const;

    //
    bool containsDimensions() const;
    bool containsIndices() const;
    bool containsOnlyScalars() const;
    
    //
    size_t getNumOfNodes() const;
    size_t getNumOfExternalNodes() const;

    //
    static bool areEqualExternals(const IndexNotatedTensorExpression& lhs, const IndexNotatedTensorExpression& rhs); 
    std::vector<const IndexNotatedTensorExpression*> getUniqueExternalNodes() const;
    
    //
    void moveSelfIntoFirstChild();

    bool isValid();

    void fillIndices();
    void replaceIndex(NotationIndex oldIndex, NotationIndex newIndex);
    void replaceIndices(const std::vector<NotationIndex>& oldIndices, const std::vector<NotationIndex>& newIndices);
    std::vector<NotationIndex> getUniqueChildIndices() const;
    std::vector<NotationIndex> getUniqueChildDimensions() const;
    std::vector<NotationIndex> getNotUniqueChildIndices() const;

    //
    const std::vector<NotationIndex>& getSortedIndices();

    // Operatoren
    void addAssign(const IndexNotatedTensorExpression& other);
    void subAssign(const IndexNotatedTensorExpression& other);
    void mulAssign(const IndexNotatedTensorExpression& other);
    void dotProductAssign(const IndexNotatedTensorExpression& other);
    void contractingDotProductAssign(const IndexNotatedTensorExpression& other);
    void crossProductAssign(const IndexNotatedTensorExpression& other);
    void dyadProductAssign(const IndexNotatedTensorExpression& other);
    void mirroringDoubleContractionAssign(const IndexNotatedTensorExpression& other);
    void crossingDoubleContractionAssign(const IndexNotatedTensorExpression& other);
    void transposeAssign();
    void inverseAssign();

    void diffAssign(const IndexNotatedTensorExpression& other);

    void traceAssign(int contractIndices);
    void traceAssign();

    void determinantAssign();
    void frobeniusAssign();
    void macaulayAssign();
    void signumAssign();
    void sqrtAssign();

    // void reEvaluateIndices();

    bool equals(const IndexNotatedTensorExpression& other);

    void evaluateIndexDimensions(std::map<int, int>& indexDimensions) const;

    // std::string wrapTensorSequenceIDNString() const;
    // std::string generateTensorSequenceIDNString(size_t depth = 0, bool forceSubstitution = false, bool useTensorNotation = false);

    std::string generateTensorSequenceJuliaString(const std::vector<NotationIndex>& indexPermutation, size_t depth = 0) const;

    std::string wrapTensorSequenceTullioString() const;
    std::string generateTensorSequenceTullioString(size_t depth = 0, bool forceSubstitution = false, bool useTensorNotation = false);
    std::string toJuliaString(const std::string& instanceLabel = "Tmp") const;

    std::string toString(size_t depth = 0) const;
    friend std::ostream& operator<<(std::ostream& os, const IndexNotatedTensorExpression& expr);
};

void moveSelfIntoFirstChild(IndexNotatedTensorExpression& node);

namespace types{

    class TENSOR_EXPRESSION : public INativeObject<TENSOR_EXPRESSION, TensorExpression>{

    public:

        static bool setUpClass();

        TENSOR_EXPRESSION() = default;
        TENSOR_EXPRESSION(TensorExpression* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override;
    };

    class INDEX_NOTATED_TENSOR_EXPRESSION : public INativeObject<INDEX_NOTATED_TENSOR_EXPRESSION, IndexNotatedTensorExpression>{

    public:

        static bool setUpClass();

        INDEX_NOTATED_TENSOR_EXPRESSION() = default;
        INDEX_NOTATED_TENSOR_EXPRESSION(IndexNotatedTensorExpression* Ptr) : INativeObject(Ptr){}

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{

            LOG << getMember().toString();
        }
    };
};