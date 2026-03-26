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
        std::cout << "Return: " << typeid(ReturnType).name() << endl;
    }
    
    size_t index = 0;
    ([&]<typename T>() {
        using ParamType = T;
        std::cout << "Param " << index++ << ": " << typeid(ParamType).name() << endl;
    }.template operator()<Args>(), ...);
}

//
template<typename Class, typename Ret, typename... Args>
void iterateTypes(Ret(Class::*func)(Args...)) {

    {
        using ReturnType = Ret;
        std::cout << "Return: " << typeid(ReturnType).name() << endl;
    }
    
    size_t index = 0;
    ([&]<typename T>() {
        using ParamType = T;
        std::cout << "Param " << index++ << ": " << typeid(ParamType).name() << endl;
    }.template operator()<Args>(), ...);
}

//
template<typename Class, typename Ret, typename... Args>
void iterateTypes(Ret(Class::*func)(Args...) const) {
    std::cout << "Member-Funktion (const)" << endl;
    
    {
        using ReturnType = Ret;
        std::cout << "Return: " << typeid(ReturnType).name() << endl;
    }
    
    size_t index = 0;
    ([&]<typename T>() {
        using ParamType = T;
        std::cout << "Param " << index++ << ": " << typeid(ParamType).name() << endl;
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

    std::cout << "=== Freie Funktion ===" << endl;
    iterateTypes(&freeFunc);
    
    std::cout << "\n=== Statische Member-Funktion ===" << endl;
    iterateTypes(&MyClass::staticFunc);
    
    std::cout << "\n=== Member-Funktion ===" << endl;
    iterateTypes(&MyClass::memberFunc);
    
    std::cout << "\n=== Const Member-Funktion ===" << endl;
    iterateTypes(&MyClass::constMemberFunc);

    return 0;
}