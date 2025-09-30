#pragma once

#include "StdOperations.h"

struct EvalResult {

    //
    static EvalResult nullRef;

    //
    Value value;
    Value* ptr = nullptr;

    //
    EvalResult();

    //
    EvalResult(const Value& valueIn);
    EvalResult(Value* ptrIn);

    //
    bool isLValue() const;
    bool isRValue() const;
    Value& getRef();
    const Value& getRef() const;

    bool operator==(const EvalResult& other) const;

    bool isNull();

    void operator+=(const EvalResult& other);
    void operator-=(const EvalResult& other);

    friend std::ostream& operator<<(std::ostream& os, const EvalResult& res);
};