#pragma once

#include "Scope.h"
#include "SemanticAnalysis/AST.h"

// Addition

// SFINAE templates
template <typename, typename, typename = void>
struct hasAdd : std::false_type {};

template <typename A, typename B>
struct hasAdd<A, B, std::void_t<decltype(add(std::declval<Value&>(), std::declval<A&>(), std::declval<B>()))>>
    : std::true_type {};

// Konkrete Implementierungen

// Int <> double
inline void add(Value& src, int& a, int b) { a += b; }
inline void add(Value& src, int& a, double b) { src = static_cast<double>(a) + b; }
inline void add(Value& src, double& a, int b) { a += static_cast<double>(b); }
inline void add(Value& src, double& a, double b) { a += b; }

// String
inline void add(Value& src, uniquePtr<std::string>& a, const uniquePtr<std::string>& b) { *a += *b; }

// Subtraktion

// SFINAE templates
template <typename, typename, typename = void>
struct hasSub : std::false_type {};

template <typename A, typename B>
struct hasSub<A, B, std::void_t<decltype(sub(std::declval<Value&>(), std::declval<A&>(), std::declval<B>()))>>
    : std::true_type {};

// Konkrete Implementierungen

// Int <> double
inline void sub(Value& src, int& a, int b) { a -= b; }
inline void sub(Value& src, int& a, double b) { src = static_cast<double>(a) - b; }
inline void sub(Value& src, double& a, int b) { a -= static_cast<double>(b); }
inline void sub(Value& src, double& a, double b) { a -= b; }

// String
inline void sub(Value& src, uniquePtr<std::string>& a, const uniquePtr<std::string>& b) { 

    if(string::endsWith(*a, *b)){

        *a = a->substr(0, a->size() - b->size());
    }
}