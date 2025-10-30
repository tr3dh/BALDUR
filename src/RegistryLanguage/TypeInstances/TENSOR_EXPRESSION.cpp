#include "TENSOR_EXPRESSION.h"

NotationIndex TensorExpression::NotationIndexCounter = 0;

std::map<IndexNotationOperator, std::string> IndexNotationOperatorStrings = {

    {IndexNotationOperator::Addition, "+"},
    {IndexNotationOperator::Subtraction, "-"},
    {IndexNotationOperator::Multiplication, "*"},
};

std::ostream& operator<<(std::ostream& os, const TensorExpression& expr){

    os << "TensorExpr >> ";

    if(expr.Relation == TkType::Argument){

        os << "Argument '" << expr.label << "' [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "]";
    }
    else if(expr.Relation == TkType::Operator){

        os << "Operation : " << magic_enum::enum_name(expr.Operator) << " | indices [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "] | " << expr.children.size() << " childs : " << endl;

        for(const auto& child : expr.children){

            os << "| " << child << endl; 
        }
    }
    else if(expr.Relation == TkType::Container){

        os << "Container | indices [";

        for(const auto& idx : expr.notatedIndices){

            os << idx << ", ";
        }

        os << "] | " << expr.children.size() << " childs : " << endl;

        for(const auto& child : expr.children){

            os << "| " << child << endl; 
        }
    }
    else{

        os << "NodeType '" << magic_enum::enum_name(expr.Relation) << "'";
    }

    return os;
}

void moveSelfIntoFirstChild(TensorExpression& node)
{
    if (node.children.empty()){
        node.children.emplace_back();
    }

    TensorExpression tmp = std::move(node);

    node = TensorExpression();
    node.children.push_back(std::move(tmp));
}

namespace types{

    int TENSOR_EXPRESSION::setUpClass(){

        // register in TypeRegister
        if(!init("TensorExpression", [](){ return new TENSOR_EXPRESSION(); })){ return false; }

        // Registrierung von Keyword zur rvalue Konstruktion von ints
        // ...

        // Konstruktoren
        registerFunction("TensorExpression", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "TensorExpression", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), arg1->getMember());
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Konstruktoren
        registerFunction("tExp", {STRING::typeIndex, INT::typeIndex},
            [__functionLabel__ = "tExp", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(STRING, 0); GET_ARG(INT, 1);

                // schreiben in returns
                ret0->getMember() = TensorExpression(arg0->getMember(), arg1->getMember());
        },
        {TENSOR_EXPRESSION::typeIndex});

        // Member
        registerMemberFunction(TENSOR_EXPRESSION::typeIndex, "toString", {},
            [__functionLabel__ = "toString", __numArgs__ = 0](FREG_ARGS){

                // Asserts
                ASSERT_IS_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns | Inputs
                GET_RETURN(STRING, 0);
                GET_MEMBER(TENSOR_EXPRESSION);

                //
                ret0->getMember() = mb->getMember().toString();
        },
        {STRING::typeIndex});

        // Konstruktoren

        // Operatoren

        // Addition
        // Bedingung: Gleiche TensorStufe
        // IndexNotation: A[i,j] + B[i,j] = C[i,j]
        // Ergebnis: Tensor der Stufe beider Operanden, beteiligte Indices bleiben frei
        //           (Summenkonvention greift nicht innerhalb Operanden der Addition)
        registerFunction("__addAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__addAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Addition){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Addition;
                }
                
                //
                member0.children.emplace_back(member1);

                //
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder == processedMember1.tensorOrder,
                    "Addition von Tensoren unterschiedlicher Stufe nicht möglich", );
                
                // A[i,j] + B[i,j] = C[i,j]
                processedMember1.replaceIndices(processedMember1.notatedIndices, processedMember0.notatedIndices);

                // Aufgrund von Assoziaitivität
                if(processedMember1.Relation == TkType::Operator &&
                   processedMember1.Operator == IndexNotationOperator::Addition){

                    TensorExpression tmp = std::move(processedMember1);
                    member0.children.pop_back();

                    member0.children.insert(
                        member0.children.end(),
                        std::make_move_iterator(tmp.children.begin()),
                        std::make_move_iterator(tmp.children.end())
                    );
                }

                // ab hier sind die processedMember Referenzen nicht mehr gültig

                // freie Indices bleiben erhalten
                member0.notatedIndices = member0.children.back().notatedIndices;
                member0.tensorOrder = member0.children.back().tensorOrder;
        },
        {});

        // Subtraktion (>> Addition ähnlich)
        // Bedingung: Gleiche TensorStufe
        // IndexNotation: A[i,j] - B[i,j] = C[i,j]
        // Ergebnis: Tensor der Stufe beider Operanden, beteiligte Indices bleiben frei
        //           (Summenkonvention greift nicht innerhalb Operanden der Addition)
        registerFunction("__subAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__subAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Subtraction){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Subtraction;
                }
                
                //
                member0.children.emplace_back(member1);

                //
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder == processedMember1.tensorOrder,
                    "Subtraktion von Tensoren unterschiedlicher Stufe nicht möglich", );
                
                // A[i,j] - B[i,j] = C[i,j]
                processedMember1.replaceIndices(processedMember1.notatedIndices, processedMember0.notatedIndices);

                // freie Indices bleiben erhalten
                member0.notatedIndices = member0.children.back().notatedIndices;
                member0.tensorOrder = member0.children.back().tensorOrder;
        },
        {});

        // Multiplikation
        // Bedingung: mindestens ein skalarer Operand
        // IndexNotation: A[] * B[i,j] = C[i,j]
        // Ergebnis: Tensor der Stufe des Tensor Operands oder Skalar, Tensor Indices bleiben frei
        registerFunction("__mulAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mulAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Multiplication){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Multiplication;
                }
                
                //
                member0.children.emplace_back(member1);

                //
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder == 0 || processedMember1.tensorOrder == 0,
                    "Kein Skalarer Operand für Multiplikation gefunden", );
                
                // A[] * B[i,j] = C[i,j]
                // ...

                // Aufgrund von Assoziaitivität
                if(processedMember1.Relation == TkType::Operator &&
                   processedMember1.Operator == IndexNotationOperator::Multiplication){

                    TensorExpression tmp = std::move(processedMember1);
                    member0.children.pop_back();

                    member0.children.insert(
                        member0.children.end(),
                        std::make_move_iterator(tmp.children.begin()),
                        std::make_move_iterator(tmp.children.end())
                    );
                }

                // Tensorstufe : m + n - 2
                // freie Indices : [i_1 ... i_{m-1}, j_2 ... j_n] >> nur k wird summiert
                member0.notatedIndices = member0.getUniqueChildIndices();
                member0.tensorOrder = member0.notatedIndices.size();
        },
        {});

        // Skalar/Dot Product
        // Operanden können unterschiedliche TensorStufen haben
        // lhs : Stufe m, rhs : Stufe n
        // IndexNotation: A[i_1 ...i_{m-1},k] * B[k, j_2 ... j_n] = C[i_1 ... i_{m-1}, j_2 ... j_n]
        // Ergebnis:
        // Tensorstufe : m + n - 2
        // freie Indices : [i_1 ... i_{m-1}, j_2 ... j_n] >> nur k wird summiert
        registerFunction("__dotProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__dotProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Multiplication){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Multiplication;
                }
                
                //
                member0.children.emplace_back(member1);

                // die hintersten beiden Childs sind die Operanden
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder > 0 && processedMember1.tensorOrder > 0,
                    "Skalarproduktbildung mit skalaren Operanden funktioniert nicht", );
                
                // A[i_1 ...i_{m-1},k] * B[k, j_2 ... j_n] = C[i_1 ... i_{m-1}, j_2 ... j_n]
                processedMember1.replaceIndex(processedMember1.notatedIndices[0], processedMember0.notatedIndices.back());

                // Aufgrund von Assoziaitivität
                if(processedMember1.Relation == TkType::Operator &&
                   processedMember1.Operator == IndexNotationOperator::Multiplication){

                    TensorExpression tmp = std::move(processedMember1);
                    member0.children.pop_back();

                    member0.children.insert(
                        member0.children.end(),
                        std::make_move_iterator(tmp.children.begin()),
                        std::make_move_iterator(tmp.children.end())
                    );
                }

                // Tensorstufe : m + n - 2
                // freie Indices : [i_1 ... i_{m-1}, j_2 ... j_n] >> nur k wird summiert
                member0.notatedIndices = member0.getUniqueChildIndices();
                member0.tensorOrder = member0.notatedIndices.size();
        },
        {});

        // Dyadisches Product
        // Operanden können unterschiedliche TensorStufen haben
        // lhs : Stufe m, rhs : Stufe n
        // IndexNotation: A[i_1 ... i_m] (x) B[j_1 ... j_n] = C[i_1 ... i_m, j_1 ... j_n]
        // Ergebnis:
        // Tensorstufe : m + n
        // freie Indices : [i_1 ... i_m, j_1 ... j_n] >> nichts wird summiert
        registerFunction("__dyadProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__dyadProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Multiplication){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Multiplication;
                }
                
                //
                member0.children.emplace_back(member1);

                // die hintersten beiden Childs sind die Operanden
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder > 0 && processedMember1.tensorOrder > 0,
                    "Skalarproduktbildung mit skalaren Operanden funktioniert nicht", );
                
                // A[i_1 ... i_m] (x) B[j_1 ... j_n] = C[i_1 ... i_m, j_1 ... j_n]
                // keine Änderung >> Indices bleiben unverändert >> alle singulär

                // Aufgrund von Assoziaitivität
                if(processedMember1.Relation == TkType::Operator &&
                   processedMember1.Operator == IndexNotationOperator::Multiplication){

                    TensorExpression tmp = std::move(processedMember1);
                    member0.children.pop_back();

                    member0.children.insert(
                        member0.children.end(),
                        std::make_move_iterator(tmp.children.begin()),
                        std::make_move_iterator(tmp.children.end())
                    );
                }

                // Tensorstufe : m + n
                // freie Indices : [all]
                member0.notatedIndices = member0.getUniqueChildIndices();
                member0.tensorOrder = member0.notatedIndices.size();
        },
        {});

        // Kreuz Product
        // Bedingungen : Operanden müssen beide TensorStufe 1 haben (>> Vectoren)
        // lhs : Stufe 1, rhs : Stufe 1
        // Indexnotation : a[i] x b[j] = e[z,i, j] a[i] b[j]
        // Indices bleiben aber civitapermutations Symbol wird anmultipliziert
        // Ergebnis:
        // Tensorstufe : 1
        // freie Indices : [z] >> nur i und j werden summiert
        registerFunction("__crossProductAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__crossProductAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Multiplication){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Multiplication;
                }
                
                //
                member0.children.emplace_back(member1);

                // die hintersten beiden Childs sind die Operanden
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder == 1 && processedMember1.tensorOrder == 1,
                    "Kreuzprodukt ist nur für vektorielle Operanden definiert",);
                
                // a[i] x b[j] = e[z,i, j] a[i] b[j]
                TensorExpression hodgeStar("epsilon", 3);
                hodgeStar.notatedIndices.reserve(3);
                hodgeStar.notatedIndices = {TensorExpression::NotationIndexCounter++, processedMember0.notatedIndices.back(), processedMember1.notatedIndices.back()};

                member0.children.emplace_back(std::move(hodgeStar));

                // Aufgrund von Assoziaitivität
                if(processedMember1.Relation == TkType::Operator &&
                   processedMember1.Operator == IndexNotationOperator::Multiplication){

                    TensorExpression tmp = std::move(processedMember1);
                    member0.children.pop_back();

                    member0.children.insert(
                        member0.children.end(),
                        std::make_move_iterator(tmp.children.begin()),
                        std::make_move_iterator(tmp.children.end())
                    );
                }

                // Tensorstufe : m + n
                // freie Indices : [all]
                member0.notatedIndices = member0.getUniqueChildIndices();
                member0.tensorOrder = member0.notatedIndices.size();
        },
        {});

        // Doppelte Überschiebung
        // Bedingungen : beide Tensoren müssen mindestens Stufe 2 haben
        // lhs : Stufe m, rhs : Stufe n
        // Indexnotation : A[i_1, ..., i_n, k, l] : B[o, p, j_1, ..., j_n] = A[i_1, ..., i_n, k, l] B[k, l, j_1, ..., j_n]
        // Ergebnis:
        // Tensorstufe : m + n - 2
        // freie Indices : [i_1, ..., i_n, j_1, ..., j_n] >> nur k und l werden summiert
        registerFunction("__crossingDoubleContractionAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__crossingDoubleContractionAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Multiplication){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Multiplication;
                }
                
                //
                member0.children.emplace_back(member1);

                // die hintersten beiden Childs sind die Operanden
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder >= 2 && processedMember1.tensorOrder >= 2,
                    "Für doppelte Überschiebung müssen beide Tensoren mindestens zweiter Stufe sein",);
                
                // A[i_1, ..., i_n, k, l] : B[o, p, j_1, ..., j_n] = A[i_1, ..., i_n, k, l] B[k, l, j_1, ..., j_n]
                // >> letze zwei Indices von member0 und erste zwei Indices von member1 werden ersetzt
                processedMember1.replaceIndices(
                    {processedMember1.notatedIndices[0], processedMember1.notatedIndices[1]},
                    {processedMember0.notatedIndices[processedMember0.notatedIndices.size() - 2],
                        processedMember0.notatedIndices[processedMember0.notatedIndices.size() - 1]});

                // Aufgrund von Assoziaitivität
                if(processedMember1.Relation == TkType::Operator &&
                   processedMember1.Operator == IndexNotationOperator::Multiplication){

                    TensorExpression tmp = std::move(processedMember1);
                    member0.children.pop_back();

                    member0.children.insert(
                        member0.children.end(),
                        std::make_move_iterator(tmp.children.begin()),
                        std::make_move_iterator(tmp.children.end())
                    );
                }

                // Tensorstufe : m + n - 4
                member0.notatedIndices = member0.getUniqueChildIndices();
                member0.tensorOrder = member0.notatedIndices.size();
        },
        {});

        // Doppelte Überschiebung
        // Bedingungen : beide Tensoren müssen mindestens Stufe 2 haben
        // lhs : Stufe m, rhs : Stufe n
        // Indexnotation : A[i_1, ..., i_n, k, l] .. B[o, p, j_1, ..., j_n] = A[i_1, ..., i_n, k, l] B[l, k, j_1, ..., j_n]
        // Ergebnis:
        // Tensorstufe : m + n - 2
        // freie Indices : [i_1, ..., i_n, j_1, ..., j_n] >> nur k und l werden summiert
        registerFunction("__mirroringDoubleContractionAssign__", {TENSOR_EXPRESSION::typeIndex, TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__mirroringDoubleContractionAssign__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 1);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& member1 = arg1->getMember();

                if(member0.Relation != TkType::Operator || member0.Operator != IndexNotationOperator::Multiplication){

                    moveSelfIntoFirstChild(arg0->getMember());

                    //
                    arg0->getMember().Relation = TkType::Operator;
                    arg0->getMember().Operator = IndexNotationOperator::Multiplication;
                }
                
                //
                member0.children.emplace_back(member1);

                // die hintersten beiden Childs sind die Operanden
                TensorExpression& processedMember0 =
                    member0.children.size() == 2 ? member0.children[member0.children.size() - 2] : member0;
                TensorExpression& processedMember1 = member0.children[member0.children.size() - 1];

                //
                if(processedMember0.Relation == TkType::Argument) processedMember0.fillIndices();
                if(processedMember1.Relation == TkType::Argument) processedMember1.fillIndices();

                //
                RETURNING_ASSERT(processedMember0.tensorOrder >= 2 && processedMember1.tensorOrder >= 2,
                    "Für doppelte Überschiebung müssen beide Tensoren mindestens zweiter Stufe sein",);
                
                // A[i_1, ..., i_n, k, l] .. B[o, p, j_1, ..., j_n] = A[i_1, ..., i_n, k, l] B[l, k, j_1, ..., j_n]
                // >> letze zwei Indices von member0 und erste zwei Indices von member1 werden ersetzt (vertauscht)
                processedMember1.replaceIndices(
                    {processedMember1.notatedIndices[0], processedMember1.notatedIndices[1]},
                    {processedMember0.notatedIndices[processedMember0.notatedIndices.size() - 1],
                        processedMember0.notatedIndices[processedMember0.notatedIndices.size() - 2]});

                // Aufgrund von Assoziaitivität
                if(processedMember1.Relation == TkType::Operator &&
                   processedMember1.Operator == IndexNotationOperator::Multiplication){

                    TensorExpression tmp = std::move(processedMember1);
                    member0.children.pop_back();

                    member0.children.insert(
                        member0.children.end(),
                        std::make_move_iterator(tmp.children.begin()),
                        std::make_move_iterator(tmp.children.end())
                    );
                }

                // Tensorstufe : m + n - 4
                member0.notatedIndices = member0.getUniqueChildIndices();
                member0.tensorOrder = member0.notatedIndices.size();
        },
        {});

        // absolut trace Funktion
        // Bedingung : Tensor muss mindestens Stufe 2 haben
        // Funktionsweise : Indices der Expression node zb [i, j, k, l] werden so ersetzt, dass die
        // indices [2] , ..., [n] mit dem ersten kontrahiert werden also hier [j,k,l] mit dem ersten also i ersetzt
        // >> absolutTrace(node[i,j,k,l]) = node[i,i,i,i]
        registerFunction("__absolutTrace__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__absolutTrace__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0); GET_ARG(TENSOR_EXPRESSION, 0);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& return0 = ret0->getMember();
            
                RETURNING_ASSERT(member0.tensorOrder >= 2,
                    "Absolute Spur nur für Tensoren mindestens zweiter Stufe definiert", );

                // copy
                return0.children.emplace_back(member0);

                //
                if(return0.children.back().Relation == TkType::Argument) return0.children.back().fillIndices();

                //
                return0.Relation = TkType::Container;

                //
                for(size_t i = 1; i < return0.children.back().notatedIndices.size(); ++i){

                    return0.children.back().replaceIndex(return0.children.back().notatedIndices[i], return0.children.back().notatedIndices[0]);
                }

                return0.notatedIndices = return0.getUniqueChildIndices();
                return0.tensorOrder = return0.notatedIndices.size();
        },
        {TENSOR_EXPRESSION::typeIndex});

        // trace Funktion
        // Bedingung : Tensor muss mindestens Stufe 2 haben
        // Funktionsweise : Indices der Expression node zb [i, j, k, l] werden so ersetzt, dass die
        // indices [2] , ..., [n] mit dem ersten kontrahiert werden also hier [j,k,l] mit dem ersten also i ersetzt
        // >> absolutTrace(node[i,j,k,l]) = node[i,i,i,i]
        registerFunction("__trace__", {TENSOR_EXPRESSION::typeIndex, INT::typeIndex},
            [__functionLabel__ = "__trace__", __numArgs__ = 2](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0); GET_ARG(INT, 1);

                TensorExpression& member0 = arg0->getMember();
                int& contractNumIndices = arg1->getMember();
                TensorExpression& return0 = ret0->getMember();
            
                RETURNING_ASSERT(member0.tensorOrder >= 2,
                    "Absolute Spur nur für Tensoren mindestens zweiter Stufe definiert", );

                RETURNING_ASSERT(member0.tensorOrder > static_cast<size_t>(contractNumIndices),
                    "Tensorstufe muss größer sein als Anzahl kontraktierter Indices", );

                // copy
                return0.children.emplace_back(member0);

                //
                if(return0.children.back().Relation == TkType::Argument) return0.children.back().fillIndices();

                //
                return0.Relation = TkType::Container;

                //
                for(size_t i = 1; i < contractNumIndices + 1; ++i){

                    return0.children.back().replaceIndex(return0.children.back().notatedIndices[i], return0.children.back().notatedIndices[0]);
                }

                return0.notatedIndices = return0.getUniqueChildIndices();
                return0.tensorOrder = return0.notatedIndices.size();
        },
        {TENSOR_EXPRESSION::typeIndex});

        // trace Funktion
        // Bedingung : Tensor muss mindestens Stufe 2 haben
        // Funktionsweise : Indices der Expression node zb [i, j, k, l] werden so ersetzt, dass die
        // indices [2] , ..., [n] mit dem ersten kontrahiert werden also hier [j,k,l] mit dem ersten also i ersetzt
        // >> absolutTrace(node[i,j,k,l]) = node[i,i,i,i]
        registerFunction("__trace__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__trace__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0);

                TensorExpression& member0 = arg0->getMember();
                int contractNumIndices = 1;
                TensorExpression& return0 = ret0->getMember();
            
                RETURNING_ASSERT(member0.tensorOrder >= 2,
                    "Absolute Spur nur für Tensoren mindestens zweiter Stufe definiert", );

                RETURNING_ASSERT(member0.tensorOrder > static_cast<size_t>(contractNumIndices),
                    "Tensorstufe muss größer sein als Anzahl kontraktierter Indices", );

                // copy
                return0.children.emplace_back(member0);

                //
                if(return0.children.back().Relation == TkType::Argument) return0.children.back().fillIndices();

                //
                return0.Relation = TkType::Container;

                //
                for(size_t i = 1; i < contractNumIndices + 1; ++i){

                    return0.children.back().replaceIndex(return0.children.back().notatedIndices[i], return0.children.back().notatedIndices[0]);
                }

                return0.notatedIndices = return0.getUniqueChildIndices();
                return0.tensorOrder = return0.notatedIndices.size();
        },
        {TENSOR_EXPRESSION::typeIndex});

        // transpose >> über __transpose__ oder "'" Operator (ruft transpose auf)

        // transpose Funktion
        // Bedingung : 
        // Funktionsweise : Indices der Expression node zb [i, j, k, l] werden umgekehrt zu [l, k, j, i]
        registerFunction("__transpose__", {TENSOR_EXPRESSION::typeIndex},
            [__functionLabel__ = "__transpose__", __numArgs__ = 1](FREG_ARGS){

                // Asserts
                ASSERT_IS_NO_MEMBER_FUNCTION;
                ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
                PREPARE_RETURNS;

                // Returns
                GET_RETURN(TENSOR_EXPRESSION, 0);
                GET_ARG(TENSOR_EXPRESSION, 0);

                TensorExpression& member0 = arg0->getMember();
                TensorExpression& return0 = ret0->getMember();
            
                // copy
                return0 = member0;

                //
                if(return0.Relation == TkType::Argument) return0.fillIndices();

                //
                std::reverse(return0.notatedIndices.begin(), return0.notatedIndices.end());  // dreht den Vektor um
        },
        {TENSOR_EXPRESSION::typeIndex});

        return true;
    }
}