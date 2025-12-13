#pragma once

#include "InstanceIncludes.h"
#include "../Evaluation/EvalResult.h"
#include "INT.h"

namespace types{

    class ARGS : public INativeObject<ARGS, EvalResultVec>{

    public:

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
        }

        // ARGS(const EvalResultVec& params) : INativeObject(params){}
        // ARGS(EvalResultVec* paramPtr) : INativeObject(paramPtr){}

        // ARGS(ARGS& other){

        //     auto& args = getMember(), otherArgs = other.getMember();

        //     args.clear();
        //     args.reserve(otherArgs.size());

        //     for(const auto& arg : otherArgs){

        //         if(arg->isLValue()){
        //             args.emplace_back(arg);
        //         }
        //         else{
        //             args.emplace_back()->cloneIntoRValue(arg->getVariableRef());
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

        //
        std::unique_ptr<IObject> clone() override {

            LOG << "Clone " << endl;
            print();
            return std::make_unique<ARGS>();
        }

        // bool containsVariable(Variable* variablePtr) override {

        //     return attribScope.containsVariable(variablePtr);
        // }

        // std::pair<bool, Variable*> containsDataReference(IObject* dataPtr) override {

        //     return attribScope.containsDataReference(dataPtr);
        // }

        // std::pair<bool, Variable*> containsDataVariableOrReference(IObject* dataPtr) override {

        //     return attribScope.containsDataVariableOrReference(dataPtr);
        // }
    };
};