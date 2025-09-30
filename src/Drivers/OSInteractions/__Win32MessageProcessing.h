#pragma once

#include <iostream>
#include <functional>

void ProcessWindowMessages(void* windowHandle, std::function<void(char**, int)>HandleDroppedFiles);