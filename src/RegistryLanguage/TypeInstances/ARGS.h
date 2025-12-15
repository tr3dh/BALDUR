#pragma once

#include "InstanceIncludes.h"
#include "../Evaluation/EvalResult.h"

#include "INT.h"
#include "STRING.h"

#include "Operations.h"

namespace types{

    class ARGS : public INativeObject<ARGS, EvalResultVec>{

    public:

        //
        static bool setUpClass();

        // Für Funktionen die konkrete TypeIndices brauchen weil source Typen nach eigener Klasse registriert werden
        // >> OtherClass::typeIndex noch nicht verfügbar
        static bool emplaceOperations();

        ARGS() = default;

        ARGS(const FunctionParams& params){

            member->clear();
            member->reserve(params.size());

            for(const auto& param : params){

                member->emplace_back();

                if(param->isLValue()){

                    member->back().setLValue(&param->getVariableRef());
                }
                else{

                    member->back().cloneIntoRValue(param->getVariableRef());
                }
            }

            eliminateLValues();
        }

        void moveFrom(const FunctionParams& params){

            member->clear();
            member->reserve(params.size());

            for(const auto& param : params){

                member->emplace_back();

                if(param->isLValue()){

                    member->back().setLValue(&param->getVariableRef());
                }
                else{

                    member->back().moveIntoRValue(param->getVariableRef());
                }
            }

            eliminateLValues();
        }

        // Vorläufig weil Einbauen von lvalue behandlung in aktuelle dref routinen zu aufwendig ist
        // >> keine verwendung von lvalues
        void eliminateLValues(){

            for(auto& res : getMember()){

                if(res.isLValue()){

                    Variable* var = &res.getVariableRef();
                    
                    res.variablePtr = nullptr;
                    res.variable.forceReference(var->getUniqueData());
                }
            }
        }

        void clear() override{

            getMember().clear();
        }

        // ARGS(const EvalResultVec& params) : INativeObject(params){}
        // ARGS(EvalResultVec* paramPtr) : INativeObject(paramPtr){}

        // ARGS(ARGS& other){

        //     auto& args = getMember(), otherArgs = other.getMember();

        //     args.clear();
        //     args.reserve(otherArgs.size());

        //     for(auto& arg : otherArgs){

        //         if(arg.isLValue()){
        //             args.emplace_back(arg);
        //         }
        //         else{
        //             args.emplace_back().cloneIntoRValue(arg.getVariableRef());
        //         }
        //     }
        // }

        // virtual ist redundant, die print bleibt überscheibbar
        void print() const override{

            LOG << "ARGS [";

            for(const auto& param : getMember()){

                param.getVariableRef().getData()->print(); LOG << ", ";
            }

            LOG << "]";
        }

        // Anpassen sobald es zu MM Problemen kommt mit Gültigkeitsverletzungen bei Löschung / Verlassen von Scopes
        Variable* getAttrib(const std::string& attribLabel) override { return nullptr; }

        bool isTrivial() override { return false; }

        std::pair<bool, Variable*> containsDataReference(IObject* dataPtr) override;

        std::pair<bool, Variable*> containsDataVariableOrReference(IObject* dataPtr) override;
    };
};