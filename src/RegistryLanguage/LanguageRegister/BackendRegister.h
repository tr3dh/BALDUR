#pragma once

#include "RegistryLanguage/TypeInstances/InstanceIncludes.h"

extern std::map<std::string, bool(*)()> g_backendRegister;
extern std::map<std::string, bool> g_fetchedBackends;

bool emplaceBackend(const std::string& backendLabel, bool(*funcPtr)());
bool fetchBackend(const std::string& backendLabel);
bool setUpBackendRegister();