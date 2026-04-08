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

#include "Alberich/Drivers/__Asserts.h"
#include "Alberich/Drivers/__StringProcessing.h"
#include "Drivers/ReccHandling/__jsonSerialize.h"
#include "Drivers/ReccHandling/__lineCounter.h"
#include "Alberich/Drivers/__timeStamp.h"
#include "Drivers/ReccHandling/__Threadsafe.h"

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
#include <magic_enum/magic_enum.hpp>

//
#include "Alberich/ByteSequence/src/ByteSequence/ByteSequence.h"
#include "Serialization/SequenceSerializations.h"

#ifndef endln
#define endln ENDL
#endif

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
static std::string g_glVersion;

static std::string g_vendorCorp = NULLSTR;
static bool g_CudaBackendEnabled = false;

static Color g_backgroundColor = Color(30,30,30,255);

static std::string githubRepositoryUrl = "https://github.com/tr3dh/ALF";

static std::string g_encoderKey = "//BLD\\\\";

extern std::string g_env;