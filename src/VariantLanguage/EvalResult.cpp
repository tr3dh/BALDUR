#include "EvalResult.h"

EvalResult EvalResult::nullRef = EvalResult();

//
EvalResult::EvalResult() = default;

//
EvalResult::EvalResult(const Value& valueIn) : value(std::move(valueIn)){}
EvalResult::EvalResult(Value* ptrIn) : ptr(ptrIn){}

//
bool EvalResult::isLValue() const{ return ptr != nullptr; }
bool EvalResult::isRValue() const{ return !isLValue(); }

Value& EvalResult::getRef(){ return ptr ? *ptr : value; }
const Value& EvalResult::getRef() const{ return ptr ? *ptr : value; }

bool EvalResult::operator==(const EvalResult& other) const{

    return getRef() == other.getRef();
}

bool EvalResult::isNull(){ return (*this) == nullRef; }

std::ostream& operator<<(std::ostream& os, const EvalResult& res) {

    os << res.getRef();
    return os;
}

// Verknüpfung mehrerer EvalResults

void EvalResult::operator+=(const EvalResult& other){

    std::visit([&](auto&& lhs, auto&& rhs){

        using LhsType = std::decay_t<decltype(lhs)>;
        using RhsType = std::decay_t<decltype(rhs)>;

        if constexpr (hasAdd<LhsType, RhsType>::value) {

            add(this->getRef(), lhs, rhs);
        }
        else {

            RETURNING_ASSERT(TRIGGER_ASSERT,
                "Add Funktion für Typen " + std::string(typeid(LhsType).name()) + " und " + std::string(typeid(RhsType).name()) +\
                " nicht definiert",);
        }

    }, this->getRef().getData(), other.getRef().getData());
}

void EvalResult::operator-=(const EvalResult& other){

    std::visit([&](auto&& lhs, auto&& rhs){

        using LhsType = std::decay_t<decltype(lhs)>;
        using RhsType = std::decay_t<decltype(rhs)>;

        if constexpr (hasSub<LhsType, RhsType>::value) {

            sub(this->getRef(), lhs, rhs);
        }
        else {

            RETURNING_ASSERT(TRIGGER_ASSERT,
                "Sub Funktion für Typen " + std::string(typeid(LhsType).name()) + " und " + std::string(typeid(RhsType).name()) +\
                " nicht definiert",);
        }

    }, this->getRef().getData(), other.getRef().getData());
}