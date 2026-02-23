#include "__Asserts.h"

std::string (*g_getErrorContext)() = nullptr;
bool g_suppressAssertionWarnings = false;
bool g_storedSuppressionFlag = false;
bool g_terminateAfterAssertionFailed = false;

//
void DISABLE_ASSERTION_LOGGING(){

    g_storedSuppressionFlag = true;
    std::swap(g_suppressAssertionWarnings, g_storedSuppressionFlag);
}

void ENABLE_ASSERTION_LOGGING(){

    g_storedSuppressionFlag = false;
    std::swap(g_suppressAssertionWarnings, g_storedSuppressionFlag);
}

void RESET_ASSERTION_LOGGING(){

    std::swap(g_suppressAssertionWarnings, g_storedSuppressionFlag);
}