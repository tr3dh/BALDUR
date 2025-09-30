#pragma once

#include "Cursor.h"

struct VersionStackFrame{
    
    std::string linesString = "";
    size_t cursorLine = -1, cursorCol = -1;
};

struct CodeEditor{

    int maxDigits = 5;
    float lineSpacing = 0;

    bool renderLineNumbers = true;
    bool writeRestriction = false;

    float syncedScrollY = 0.0f;

    size_t hoveredCol = -1;
    size_t hoveredLine = -1;

    bool selectionNotActive = true;
    int scrollOffsetLines = 5;
    bool singleLineWin = false;
    bool entered = false;

    //
    bool vScrollDragging = false;

    std::string cScriptPath = NULLSTR;
    std::vector<std::string> lines = {};
    std::vector<ImVec2> linePositions = {};

    Cursor mainCursor, dragCursor;
    std::string windowName = "editorWin##" + std::to_string(reinterpret_cast<uintptr_t>(this));

    CodeEditor();

    size_t countLines(const std::string path);
    void openScript(const std::string& scriptPath);
    void render(const ImVec2& leftCorner, const ImVec2& size, bool ignoreDummys = false, bool disableAutomaticScrolling = false);
    void save();

    void highlightLine(size_t lineIdx, const  ImU32& frameColor = IM_COL32(255, 255, 255, 255),
                    const  ImU32& fillColor = IM_COL32(255, 255, 255, 0), float relThickness = 0.2,
                    bool strip = true, bool markEmptyLines = true, bool ignoreEmptyLines = false);
    
    void highlightLineSection(size_t lineIdx, size_t startAtColIdx, size_t stopAtColIdx,
        const  ImU32& frameColor = IM_COL32(255, 255, 255, 255), const  ImU32& fillColor = IM_COL32(255, 255, 255, 0), float relThickness = 0.2,
        bool strip = true, bool markEmptyLines = true, bool ignoreEmptyLines = false);

    std::vector<ImWchar> getInputChars();

    bool isMainCursorBeforeDragged();
    std::tuple<size_t, size_t, size_t, size_t> getCursorRange();

    void deleteCursorRange();
    void copyCursorRange();
    void pasteOnMainCursor();
    void pasteBesideMainCursor();

    std::string exportString();
    void importString(const std::string& str);

    std::vector<VersionStackFrame> undoStack = {};
    std::vector<VersionStackFrame> redoStack = {};

    void clearVersionStacks();
    void pushUndo();
    void undo();
    void redo();

    bool scrollToCursor(Cursor& cursor, int offsetLines = 5, int offsetColumns = 10);
    bool isVScrollActiveChild();

    void setCursorToBottom();
    float getLineHeight();
};