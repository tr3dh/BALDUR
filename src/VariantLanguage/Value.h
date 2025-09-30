#pragma once

#include "ValueVariant.h"

ValueVariant initValueVariant(Type type);

bool operator<(const Value& lhs, const Value& rhs);

struct Value{

    //
    bool mIsReference = false;
    bool mIsConstValue = false;

    //
    ValueVariant mData;
    // sharedPtr<Value> mDataRef = nullptr;
    Value* mDataRef = nullptr;

    //
    Value();
    Value(Type tp);
    Value(const Value& other);

    template<typename T>
    explicit Value(const T& member){

        if constexpr (std::is_same_v<T, std::string> || std::is_convertible_v<T, const char*>) {

            mData = std::make_unique<std::string>(member);
        }
        else if constexpr (std::is_same_v<T, std::vector<Value>>) {

            mData = std::make_unique<std::vector<Value>>(member);
        }
        else if constexpr (std::is_same_v<T, std::map<Value, Value>>) {

            mData = std::make_unique<std::map<Value, Value>>(member);
        }
        else{
            
            // alle anderen Konstruktoraufrufe werden über wrapper rekursiv bis hier her umgesetzt 
            mData = member;
        }
    }

    // Für Initializer-List von Values
    explicit Value(std::initializer_list<Value> list);
    explicit Value(std::initializer_list<std::pair<Value, Value>> list);

    template<typename T>
    void operator=(const T& member){

        *this = Value(member);
    }

    template<typename T>
    bool operator==(const T& member){
        return *this == Value(member);
    }

    bool operator==(const Value& other) const{
        
        //
        return getData() == other.getData();
    }

    // Copy-Assignment
    Value& operator=(const Value& other);

    const ValueVariant& getData() const;
    ValueVariant& getData();

    bool isInitialized();

    Type getType() const;
    void setType(Type tp);

    void setMemberIsRef(bool isRef = true);
    void setMemberIsConst(bool isConst = true);

    void reference(Value& other);

    friend std::ostream& operator<<(std::ostream& os, const Value& val);
};