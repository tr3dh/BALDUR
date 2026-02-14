#include "BackendRegister.h"

std::map<std::string, bool(*)()> g_backendRegister = {};
std::map<std::string, bool> g_fetchedBackends = {};

bool emplaceBackend(const std::string& backendLabel, bool(*funcPtr)()){

    return g_backendRegister.try_emplace(backendLabel, funcPtr).second && g_fetchedBackends.try_emplace(backendLabel, false).second;
}

bool fetchBackend(const std::string& backendLabel){

    RETURNING_ASSERT(g_backendRegister.contains(backendLabel),
        "Es konnte kein gültiges Backend '" + backendLabel + "' gefunden werden" , false);

    if(g_fetchedBackends[backendLabel]){ return true; }
    
    g_fetchedBackends[backendLabel] = true;
    return (*g_backendRegister[backendLabel])();
}

bool setUpBackendRegister(){

    //
    g_backendRegister.clear();
    g_fetchedBackends.clear();

    //
    registerFunction("logRegisteredBackends", {},
        [__functionLabel__ = "logRegisteredBackends", __numArgs__ = 0](FREG_ARGS){

            // Asserts
            ASSERT_IS_NO_MEMBER_FUNCTION;
            ASSERT_HAS_N_INPUT_ARGS(__numArgs__);
            PREPARE_RETURNS;

            //
            LOG << "Backendregister with " << g_backendRegister.size() << " emplaced backends :" << endl;

            //
            for(const auto& [backendlabel, _] : g_backendRegister){

                LOG << ">> " << backendlabel << endl;
            }
    },
    {});

    return true;
}