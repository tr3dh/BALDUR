// Dieser Header setzt die gesamte Entwicklungsumgebung auf
// Der Aufbau ist folgendermaßen
//
// .  main Env : raylib + stark eingeschränkte WinAPI + ...
//    >>  das main ENV setzt raylib und eine stark eingeschränkte WinAPI auf.
//        Das liegt daran, dass die WinAPI und raylib teilweise gleiche Makros definieren und
//        dementsprechend von verschiedenen Definition ausgehen und verschiedene Definitionen benötigen
//    >>  dadurch dass raylib aber zwangsläufig in der main gebraucht wird, wird hier die winapi nur in reduzierter
//        Form eingebunden
//    >>  das main Env stellt alle Standard includes zur Verfügung die das Projekt braucht und alle eigenen Definitionen
//
// .  Für schwergewichtite Libs (LLVM), die den build unnötig verlangsamen oder einfrieren
//    oder Ausgeschlossene WinAPI Funktionen müssen diese includes direkt in cpp Dateien erledigt werden und
//
// >> Die Winapi muss dazu wie in src/Drivers/OSInteractions/Win32Clipboard in eine Umgebung inkludiert werden, die unabhängig
//    main Env ist, sonst ist der WinAPI inklude nicht vollständig
//
// >> Der Treiber kann dann später ins main ENV inkludiert werden und da nur seine Funktionsdeklarationen von den
//    WinAPI Funktionen abhängig sind, die Makros aber nicht ins main ENV mit übernommen werden
//
// exemplarische Project Struktur
//
//    raylib  _____________       ____ Driver ____
//                        |      |               |
//    reduzierte WinAPI __|      |               |           _____________ Driver pre Decls ______________
//                        |___ Env.h ___         |          |                                            |
//    basics Treiber _____|             |        |          |                                            |
//                                      |________|_____ defines.h _____________..........................|___ templateDccls.h ___ Procs
//                                               |          |                 |                          |
//    string _________                           |          |___ llvm Objs ___|_____                     |
//                   |__ WinAPI Driver __________|          |                      |                     |
//    volle WinAPI __|                                      |______ src ___________|_____ pre Decls _____|                                                       
                                                            
#pragma once

// std includes
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <array>
#include <tuple>
#include <random>
#include <stdio.h>
#include <utility>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>
#include <typeindex>
#include <functional>
#include <map>

namespace fs = std::filesystem;
typedef fs::file_time_type fileTime;

#include <string>
#include <optional>

//
#include <raylib.h>
#include <raymath.h>

#include <rlImGui/rlImGui.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#if defined(_WIN32)

// To avoid conflicting windows.h symbols with raylib, some flags are defined
// WARNING: Those flags avoid inclusion of some Win32 headers that could be required
// by user at some point and won't be included...
//-------------------------------------------------------------------------------------

// If defined, the following flags inhibit definition of the indicated items.
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
//#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
//#define NOMINMAX          // Macros min(a,b) and max(a,b)

#ifndef NOMINMAX
#define NOMINMAX          // Verhindert Konflikte mit min/max Makros von Windows
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Reduziert Windows-Header, schnelleres Kompilieren
#endif

#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions

// Type required before windows.h inclusion
typedef struct tagMSG *LPMSG;

#include <windows.h>
#include <shellapi.h>

// Type required by some unused function...
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#include <objbase.h>
#include <mmreg.h>
#include <mmsystem.h>

// externes Deklarieren der internen CORE-Struktur
#ifdef __cplusplus
extern "C" {
#endif
    extern struct {
        void *handle; // HWND unter Windows
    } CORE;
#ifdef __cplusplus
}
#endif

// Some required types defined for MSVC/TinyC compiler
#if defined(_MSC_VER) || defined(__TINYC__)
    #include "propidl.h"
#endif
#endif

#define IMGUI_FILEBROWSER_NO_EXCEPTIONS
#include <imgui-filebrowser/imfilebrowser.h>

#include <implot.h>

#include <rlgl.h>
#ifdef RAYLIB_USES_ANGLE
#include <GLES3/gl3.h>
#else
#include <GL/gl.h>
#endif

// #include <magic_enum/magic_enum.hpp>

// //
// #if defined(LOG) | defined(_ERROR)
// #error "Logging Direktiven können nicht definiert werden, Makros bereits deklariert"
// #endif

// Für Logging relevante Treiber
#include "Alberich/Drivers/__timeStamp.h"

//
extern std::ofstream g_logFile;

#ifndef LOG_RED
#define LOG_RED     "\033[31m"
#endif

#ifndef LOG_GREEN
#define LOG_GREEN   "\033[32m"
#endif

#ifndef LOG_YELLOW
#define LOG_YELLOW  "\033[93m"
#endif

#ifndef LOG_BLUE
#define LOG_BLUE    "\033[34m"
#endif

#ifndef LOG_ORANGE
#define LOG_ORANGE  "\033[38;2;255;165;0m"
#endif

#ifndef LOG_RESET
#define LOG_RESET   "\033[0m"
#endif

#ifndef LOG
#define LOG     std::cout << LOG_ORANGE
#endif

#ifndef _ERROR
#define _ERROR  std::cerr << LOG_RED
#endif

#ifndef ENDL
#define ENDL "\n" << LOG_RESET << std::flush;
#endif

// #endif

// Debugging über message ausgabe und instance debugging, dass direkt überladung für string stream mit << aufruft
#ifndef mbug
#define mbug(message) LOG << LOG_YELLOW << "___Passed : " << #message << ENDL;
#endif

#ifndef ibug
#define ibug(objekt) LOG << LOG_YELLOW << "___Objekt : " << #objekt << " " << objekt << ENDL;
#endif

//
#include "Alberich/Drivers/__Asserts.h"

// Grade gibts nen multiple definition error aber issue im magic_enum ist schon aufgemacht worden
// Kann in paar Tagen einfach wieder einkommentiert werden
// #include <magic_enum/magic_enum.hpp>