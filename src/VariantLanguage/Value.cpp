#include "Value.h"

ValueVariant initValueVariant(Type type) {

    switch(type) {
        case Type::NONE:           return ValueVariant(std::monostate{});
        case Type::BOOL:           return ValueVariant(false);
        case Type::INT:            return ValueVariant(0);
        case Type::UNSIGNED_INT:   return ValueVariant(size_t(0));
        case Type::FLOAT:          return ValueVariant(0.0f);
        case Type::DOUBLE:         return ValueVariant(0.0);
        case Type::STRING:         return ValueVariant(std::make_unique<std::string>(""));
        case Type::VECTOR:         return ValueVariant(std::make_unique<std::vector<Value>>());
        case Type::MAP:            return ValueVariant(std::make_unique<std::map<Value, Value>>());
        default:
            CRITICAL_ASSERT(TRIGGER_ASSERT, "Cannot create Value for Type::NONE");
            return ValueVariant(std::monostate{});
    }
}

bool operator<(const Value& lhs, const Value& rhs) {

    if (lhs.getType() != rhs.getType())
        return static_cast<uint8_t>(lhs.getType()) < static_cast<uint8_t>(rhs.getType());

    return std::visit([](auto&& a, auto&& b) -> bool {
        using T1 = std::decay_t<decltype(a)>;
        using T2 = std::decay_t<decltype(b)>;

        if constexpr (!std::is_same_v<T1, T2>) {
            // unterschiedliche Typen, nach Type enum Reihenfolge
            return false;
        } else if constexpr (std::is_same_v<T1, std::monostate>) {
            return false;
        } else if constexpr (std::is_same_v<T1, bool> ||
                             std::is_same_v<T1, int> ||
                             std::is_same_v<T1, size_t> ||
                             std::is_same_v<T1, float> ||
                             std::is_same_v<T1, double>) {
            return a < b;
        } else if constexpr (std::is_same_v<T1, uniquePtr<std::string>>) {
            if (!a || !b) return !a && b;
            return *a < *b;
        } else if constexpr (std::is_same_v<T1, uniquePtr<std::vector<Value>>>) {
            if (!a || !b) return !a && b;
            return a->size() < b->size();
        } else if constexpr (std::is_same_v<T1, uniquePtr<std::map<Value, Value>>>) {
            if (!a || !b) return !a && b;
            return a->size() < b->size();
        } else {
            return false;
        }

    }, lhs.getData(), rhs.getData());
}

//
Value::Value() = default;

Value::Value(Type tp){

    setType(tp);
}

Value::Value(const Value& other) {
    
    getData() = std::visit([](auto&& arg) -> ValueVariant {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::unique_ptr<std::string>>) {
            return ValueVariant{std::make_unique<std::string>(*arg)};
        } else if constexpr (std::is_same_v<T, std::unique_ptr<std::vector<Value>>>) {
            auto vec = std::make_unique<std::vector<Value>>();
            for (const auto& v : *arg) vec->emplace_back(v);
            return ValueVariant{std::move(vec)};
        } else if constexpr (std::is_same_v<T, std::unique_ptr<std::map<Value, Value>>>) {
            auto m = std::make_unique<std::map<Value, Value>>();
            for (const auto& [k, v] : *arg) (*m)[k] = v;
            return ValueVariant{std::move(m)};
        } else {
            return ValueVariant{arg}; // bool, int, double, monostate
        }

        // return initValueVariant(Type::NONE);

    }, other.getData());
}

const ValueVariant& Value::getData() const {

    if(mIsReference){
        return mDataRef->mData;
    }

    return mData;
}

ValueVariant& Value::getData() {

    if(mIsReference){
        return mDataRef->mData;
    }

    return mData;
}

// Für Initializer-List von Values
Value::Value(std::initializer_list<Value> list) {

    getData() = std::make_unique<std::vector<Value>>(std::move(list));
}

Value::Value(std::initializer_list<std::pair<Value, Value>> list) {

    //
    getData() = std::make_unique<std::map<Value, Value>>();

    //
    auto& mapPtr = std::get<std::unique_ptr<std::map<Value, Value>>>(getData());

    for (auto& p : list) {
        mapPtr->insert(std::make_pair(p.first, p.second));
    }
}

bool Value::isInitialized() {
    return getType() != Type::NONE;
}

void Value::setMemberIsRef(bool isRef){

    mIsReference = isRef;
}

void Value::setMemberIsConst(bool isConst){

    mIsConstValue = isConst;
}

Type Value::getType() const{
    
    return static_cast<Type>(getData().index());;
}

void Value::setType(Type tp){

    getData() = initValueVariant(tp);
}

// Copy-Assignment
Value& Value::operator=(const Value& other) {
    
    if (this == &other) {
        
        return *this;
    }

    if(other.getType() == Type::NONE){
        
        return *this;
    }

    if(getType() != Type::NONE && getType() != other.getType()){

        // Durchgehen lassen, Cast in erwarteten Datentyp oder execption ??

        _ERROR << "Conflicting Declaration in assignment : " << getType() << " >> " << other.getType() << endl;
    }

    //
    Value tmp(other);       
    std::swap(getData(), tmp.getData());     

    //
    return *this;
}

void Value::reference(Value& other){

    //
    mIsReference = true;

    //
    if(other.mIsReference){
        return;
    }

    // mDataRef = sharedPtr<Value>(&other, [](Value*){});
    mDataRef = &other;
}

std::ostream& operator<<(std::ostream& os, const Value& val) {

    //
    os << "Type " << magic_enum::enum_name(val.getType()) << ((val.mIsReference) ? " REF " : " ") << "Value ";

    //
    std::visit([&os](auto&& arg){

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::monostate>) {

            os << "uninitialized";

        } else if constexpr (std::is_same_v<T, bool>) {

            os << (arg ? "true" : "false");

        } else if constexpr (std::is_same_v<T, int>) {

            os << arg;

        } else if constexpr (std::is_same_v<T, size_t>) {

            os << arg;

        } else if constexpr (std::is_same_v<T, float>) {

            os << arg;

        } else if constexpr (std::is_same_v<T, double>) {

            os << arg;

        }
        else if constexpr (std::is_same_v<T, uniquePtr<std::string>>) {

            if (arg)
                os << "\"" << arg << "\"";
            else
                os << "<null string>";

        }
        else if constexpr (std::is_same_v<T, uniquePtr<std::vector<Value>>>) {

            os << endl;
            os << arg << endl;

        }
        else if constexpr (std::is_same_v<T, uniquePtr<std::map<Value, Value>>>) {

            os << endl;

            for(const auto& [k, v] : *arg){
                os << k << " : " << v << endl;
            }

        } else {
            os << "<unknown type>";
        }
    }, val.getData());

    return os;
}