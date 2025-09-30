#pragma once

#include "defines.h"
#include "GUI/ImGuiStyleDecls.h"
#include "GUI/ImGuiCustomElements.h"

enum CursorLock{
    None = 0, Max, Min
};

struct Cursor{
    
    std::vector<std::string>* lines;

    size_t cursorLine = -1, cursorCol = -1, lockedCol = -1;
    size_t cachedCursorLine = -1, cachedCursorCol = -1;
    CursorLock lock;

    bool drawCursor = false;
    ImVec2 cursorPos = {-1,-1};
    ImVec2 cursorLinePos = {-1,-1};

    void recalCursor();
    void draw();
    bool stear();
    void setUpRendering(const ImVec2& linePos, size_t lineIdx);
    void nullRenderingSettings();
    void placeOnClick(size_t hoveredLine, size_t hoveredCol);
    void placeOnCursor(const Cursor& other);
    bool isOnValidPosition();
    void setOnInValidPosition();

    void enterTextOnCursorPos(const std::vector<ImWchar>& chars);

    void enterParagraphOnCursor();
    void enterCharakterOnCursor(const char* c);

    void deleteOnCursor();
    void validatePosition();

    void cache();
    bool positionChanged();
};