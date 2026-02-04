// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden
// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden

#pragma once

#include "Env.h"

// Drivers
#include "Drivers/OSInteractions/__Win32Clipboard.h"
#include "Drivers/OSInteractions/__pseudoTerminal.h"
#include "Drivers/OSInteractions/__Win32MessageProcessing.h"
#include "Drivers/OSInteractions/__backgroundShell.h"

#include "Drivers/ReccHandling/__Asserts.h"
#include "Drivers/ReccHandling/__StringProcessing.h"
#include "Drivers/ReccHandling/__jsonSerialize.h"
#include "Drivers/ReccHandling/__lineCounter.h"
#include "Drivers/ReccHandling/__timeStamp.h"

#include "Drivers/Calculations/__SymbolicExpressions.h"
#include "Drivers/Calculations/__SymEngineMatrix.h"
#include "Drivers/Calculations/__SymbolicExpressionRound.h"
#include "Drivers/Calculations/__EigenMatrix.h"
#include "Drivers/Calculations/__MatrixConversions.h"

#include "Drivers/Raylib/__rlLogging.h"
#include "Drivers/Raylib/__rlProgressDisplay.h"

#include "Drivers/Visualisation/__Coloration.h"

#include "Drivers/UI/__fileBrowser.h"

#include "Drivers/Vec/__Vec.h"

// Decorators
#include "decorators/timeFunction.h"

//
#include <enet/enet.h>
#include <magic_enum/magic_enum.hpp>

//
#include "Serialization/ByteSequence.h"
#include "Serialization/SequenceSerializations.h"

#define endl ENDL

// typedefs
typedef uint16_t NodeIndex;
typedef NodeIndex CellIndex;

// globals
inline SYMBOL(x);
inline SYMBOL(y);
inline SYMBOL(z);

inline SYMBOL(r);
inline SYMBOL(s);
inline SYMBOL(t);

inline SYMBOL(xi);

const static std::vector<Symbol> g_globalKoords = {x,y,z};
const static std::vector<Symbol> g_isometricKoords = {r,s,t};

static bool g_ComputeShaderBackendEnabled = false;
static float g_glVersion = 0;

static std::string g_vendorCorp = NULLSTR;
static bool g_CudaBackendEnabled = false;

static Color g_backgroundColor = Color(30,30,30,255);

static std::string githubRepositoryUrl = "https://github.com/tr3dh/ALF";

static std::string g_encoderKey = "ALF";

static std::string g_languageScriptSuffix = "bld";

extern std::string g_env;

//
template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T>& vec) {

    os << "Vector with " << vec.size() << " elements" << endl; 
    for(size_t idx = 0; idx < vec.size(); idx++){
        os << idx << " " << vec[idx] << endl;
    }
    return os;
}

//
template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const std::map<Key, Value>& map) {

    os << "Map with " << map.size() << " elements" << endl; 
    for(const auto& [k,v] : map){
        os << k << " " << v << endl;
    }
    return os;
}

template<typename T>
std::string printPlainVector(const std::vector<T>& vec, bool logParens = true, const std::string& token = ", ") {

    if(vec.empty()) {
        return logParens ? "()" : "";
    }
    
    std::string result = logParens ? "(" : "";
    for(size_t i = 0; i < vec.size(); ++i) {
        result += std::to_string(vec[i]);
        if(i != vec.size() - 1) {
            result += token;
        }
    }
    result += logParens ? ")" : "";
    return result;
}

template<typename T>
std::string printIncreasedPlainVector(const std::vector<T>& vec, bool logParens = true, const std::string& token = ", ") {

    if(vec.empty()) {
        return logParens ? "()" : "";
    }
    
    std::string result = logParens ? "(" : "";
    for(size_t i = 0; i < vec.size(); ++i) {
        result += std::to_string(vec[i] + 1);
        if(i != vec.size() - 1) {
            result += token;
        }
    }
    result += logParens ? ")" : "";
    return result;
}

template<typename T, typename Func>
std::string fprintPlainVector(std::vector<T>& vec, const Func& printFunc, bool logParens = true, const std::string& token = ", ") {

    if(vec.empty()) {
        return logParens ? "()" : "";
    }
    
    std::string result = logParens ? "(" : "";
    for(size_t i = 0; i < vec.size(); ++i) {
        
        result += printFunc(vec[i]);
        if(i != vec.size() - 1) {
            result += token;
        }
    }
    result += logParens ? ")" : "";
    return result;
}

template<typename K, typename V>
void emplaceVectorsIntoMap(std::map<K, V>& map, const std::vector<K>& keys, const std::vector<K>& vals) {

    //
    RETURNING_ASSERT(keys.size() == vals.size(), "...",);

    //
    if(keys.empty()){ return; }

    for (size_t i = 0; i < keys.size(); ++i) {

        map.try_emplace(keys[i], vals[i]);
    }
}