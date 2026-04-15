#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

#include "Alberich/Interpreter.h"

int main() {

    ByteSequence bs;

    std::string n, m = "Hallo";
    std::vector<std::vector<std::vector<int>>> b, a = {{{1,2,3}, {2,3,4}}};

    bs.insertMultiple(m, a);
    bs.extractMultipleReversed(m, b);

    LOG << m << "   " << b << endln;

    return 0;
}