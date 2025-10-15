// Header sorgt dafür das bestimmte template Instanzierungen unterdrückt werden,
// das sie bereits in anderen cpp bzw. obj files liegen

#pragma once

#include "defines.h"

#include "GUI/ImGuiStyleDecls.h"
#include "GUI/ImGuiCustomElements.h"
// #include "Rendering/CameraMovement.h"
#include "Rendering/RenderingStandardShapes.h"
#include "WinHandle/GetEnv.h"
#include "WinHandle/WinCmd.h"
#include "Prompts/Curl.h"
#include "Logging/Logging.h"
#include "GUI/CodeEditor.h"
#include "GUI/Terminal.h"
#include "SemanticAnalysis/AST.h"
#include "CleanUp/ClearCaches.h"
#include "RegistryLanguage/Interpreter.h"

// // Precompiles
// #include "bin/EigenPrecompiles.h"
// #include "bin/StringProcessingPreCompiles.h"
// #include "bin/UIPrecompiles.h"