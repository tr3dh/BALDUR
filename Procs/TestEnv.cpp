#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

#include "Alberich/Interpreter.h"

//
template<typename Ret, typename... Args>
void iterateTypes(Ret(*func)(Args...)) {
    
    {
        using ReturnType = Ret;
        std::cout << "Return: " << typeid(ReturnType).name() << endln;
    }
    
    size_t index = 0;
    ([&]<typename T>() {
        using ParamType = T;
        std::cout << "Param " << index++ << ": " << typeid(ParamType).name() << endln;
    }.template operator()<Args>(), ...);
}

//
template<typename Class, typename Ret, typename... Args>
void iterateTypes(Ret(Class::*func)(Args...)) {

    {
        using ReturnType = Ret;
        std::cout << "Return: " << typeid(ReturnType).name() << endln;
    }
    
    size_t index = 0;
    ([&]<typename T>() {
        using ParamType = T;
        std::cout << "Param " << index++ << ": " << typeid(ParamType).name() << endln;
    }.template operator()<Args>(), ...);
}

//
template<typename Class, typename Ret, typename... Args>
void iterateTypes(Ret(Class::*func)(Args...) const) {
    std::cout << "Member-Funktion (const)" << endln;
    
    {
        using ReturnType = Ret;
        std::cout << "Return: " << typeid(ReturnType).name() << endln;
    }
    
    size_t index = 0;
    ([&]<typename T>() {
        using ParamType = T;
        std::cout << "Param " << index++ << ": " << typeid(ParamType).name() << endln;
    }.template operator()<Args>(), ...);
}

//
class MyClass {
public:

    int memberFunc(double x, std::string y) {
        return static_cast<int>(x);
    }
    
    float constMemberFunc(int a) const {
        return static_cast<float>(a);
    }
    
    static std::string staticFunc(int a, double b) {
        return "test";
    }
};

// Freie Funktion
void freeFunc(int a, float b) {}

int main() {

    std::cout << "=== Freie Funktion ===" << endln;
    iterateTypes(&freeFunc);
    
    std::cout << "\n=== Statische Member-Funktion ===" << endln;
    iterateTypes(&MyClass::staticFunc);
    
    std::cout << "\n=== Member-Funktion ===" << endln;
    iterateTypes(&MyClass::memberFunc);
    
    std::cout << "\n=== Const Member-Funktion ===" << endln;
    iterateTypes(&MyClass::constMemberFunc);

    return 0;
}