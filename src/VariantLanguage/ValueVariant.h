#pragma once

#include "Types.h"

template<typename T>
using sharedPtr = std::shared_ptr<T>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<T>& ptr){

    os << *ptr;
    return os;
}

template<typename T>
using uniquePtr = std::unique_ptr<T>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::unique_ptr<T>& ptr){

    os << *ptr;
    return os;
}

template<typename Variant, typename NewType>
struct variant_append;

template<typename... Ts, typename NewType>
struct variant_append<std::variant<Ts...>, NewType> {
    using type = std::variant<Ts..., NewType>;
};

template<typename Variant, typename F, std::size_t... I>
void for_each_type_impl(F f, std::index_sequence<I...>) {
    (f(std::type_identity<std::variant_alternative_t<I, Variant>>{}), ...);
}

template<typename Variant, typename F>
void for_each_type(F f) {
    for_each_type_impl<Variant>(f, std::make_index_sequence<std::variant_size_v<Variant>>{});
}

// Anwendung zb
// for_each_type<ValueVariant>([](auto t) {

//     using T = typename decltype(t)::type;

//     Value val;
//     val.setType(evalType(T{}));
//     LOG << val << endl;
// });

struct Value;

template<typename T>
Type evalType(const T& member){

    if constexpr (std::is_same_v<T, std::monostate>) {

        return Type::NONE;

    } else if constexpr (std::is_same_v<T, bool>) {

        return Type::BOOL;

    } else if constexpr (std::is_same_v<T, int>) {

        return Type::INT;

    } else if constexpr (std::is_same_v<T, size_t>) {

        return Type::UNSIGNED_INT;

    } else if constexpr (std::is_same_v<T, float>) {

        return Type::FLOAT;

    } else if constexpr (std::is_same_v<T, double>) {

        return Type::DOUBLE;
    }
    else if constexpr (std::is_same_v<T, std::unique_ptr<std::string>> || std::is_same_v<T, std::string>) {

        return Type::STRING;
    }
    else if constexpr (std::is_same_v<T, std::unique_ptr<std::vector<Value>>> || std::is_same_v<T, std::vector<Value>>) {

        return Type::VECTOR;
    }
    else if constexpr (std::is_same_v<T, std::unique_ptr<std::map<Value, Value>>> || std::is_same_v<T, std::map<Value, Value>>) {

        return Type::MAP;
    }
    else {

        return Type::NONE;
    }

    return Type::NONE;
}

template<Type T>
struct TypeMap;

template<> struct TypeMap<Type::NONE>         { using type = std::monostate; };
template<> struct TypeMap<Type::BOOL>         { using type = bool; };
template<> struct TypeMap<Type::INT>          { using type = int; };
template<> struct TypeMap<Type::UNSIGNED_INT> { using type = size_t; };
template<> struct TypeMap<Type::FLOAT>        { using type = float; };
template<> struct TypeMap<Type::DOUBLE>       { using type = double; };
template<> struct TypeMap<Type::STRING>       { using type = uniquePtr<std::string>; };
template<> struct TypeMap<Type::VECTOR>       { using type = uniquePtr<std::vector<Value>>; };
template<> struct TypeMap<Type::MAP>          { using type = uniquePtr<std::map<Value, Value>>; };

template<Type T>
using TypeMap_t = typename TypeMap<T>::type;

using ValueVariant = std::variant<
    TypeMap_t<Type::NONE>,
    TypeMap_t<Type::BOOL>,
    TypeMap_t<Type::INT>,
    TypeMap_t<Type::UNSIGNED_INT>,
    TypeMap_t<Type::FLOAT>,
    TypeMap_t<Type::DOUBLE>,
    TypeMap_t<Type::STRING>,
    TypeMap_t<Type::VECTOR>,
    TypeMap_t<Type::MAP>
>;

template<Type T>
ValueVariant makeVariant() {
    return TypeMap_t<T>{};
}