#pragma once

// includes für den asserst Teil
#include <assert.h>
#include <iostream>
#include <functional>
#include <sstream>
#include <cstdlib>

//
#include "__timeStamp.h"

#ifndef LOG
#define LOG std::cout
#endif // LOG

#ifndef _ERROR
#define _ERROR std::cerr
#endif // _ERROR

#ifndef ENDL
#define ENDL "\n"
#endif // _ERROR

#define TRIGGER_ASSERT  0
#define PASS_ASSERT     1

#define RESOLVE_CONDITION(condition)\
    bool resolvedCondition = false;\
    if(condition){\
        resolvedCondition = true;\
    } 

extern std::string (*g_getErrorContext)();
extern bool g_suppressAssertionWarnings, g_storedSuppressionFlag;

//
void DISABLE_ASSERTION_LOGGING();
void ENABLE_ASSERTION_LOGGING();
void RESET_ASSERTION_LOGGING();

#define ASSERT(condition, message)\
    do{\
        if (!(condition) && !(g_suppressAssertionWarnings)) {\
            std::ostringstream oss;\
            oss << "[" << getTimestamp() << "] Assertion Failed : '" << message << "'\n" << \
            ">> Source : " << __FILE__ << ":" << __LINE__ << " >> " << __FUNCTION__ << "\n";\
            \
            if(g_getErrorContext != nullptr){ \
                oss << ">> Context : " <<  g_getErrorContext(); \
            } \
            _ERROR << oss.str() << ENDL;\
        }}\
    while(0);

#define CRITICAL_ASSERT(condition, message)\
    do{\
        RESOLVE_CONDITION(condition);\
        ASSERT(resolvedCondition, message);\
        if (!(resolvedCondition)) {\
            std::terminate();\
        }\
    }\
    while(0);

// für
#define RETURNING_ASSERT(condition, message, returnVal)\
    do{\
        RESOLVE_CONDITION(condition);\
        ASSERT(resolvedCondition, message);\
        if (!(resolvedCondition)) {\
            return returnVal;\
        }\
    }\
    while(0)\

#define RETURNING_VOID_ASSERT(condition, message, returnVal)\
    RETURNING_ASSERT(condition,message,)

// define dient eigentlich nur dazu, dass der file nicht auf eine durch '\' angehängte Zeile oder '//' endet
// das führt zu einer Warnung im GCC da Skripte so nicht enden dürfen
#define ASSERTS_DEFINED