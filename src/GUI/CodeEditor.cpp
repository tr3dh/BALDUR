#include "CodeEditor.h"

CodeEditor::CodeEditor(){

    lines = {""};
    linePositions = {};

    mainCursor.lines = &lines;
    dragCursor.lines = &lines;
}

size_t CodeEditor::countLines(const std::string path){

    std::ifstream file(path, std::ios::binary);
    RETURNING_ASSERT(file.is_open(), "File " + path + " konnte nicht geöffnet werden", 0);

    size_t lines = 0;

    char c;
    while (file.get(c)) {
        if (c == '\n') {
            lines++;
        }
    }

    file.close();
    return lines + 1;
}

void CodeEditor::openScript(const std::string& scriptPath){

    size_t numLines = countLines(scriptPath);
    LOG << "++ [Editor] : Open Script " << scriptPath << " | " << numLines << " lines" << ENDL;

    std::fstream scriptFile(scriptPath);

    RETURNING_ASSERT(scriptFile.is_open(), "Fehler beim Laden des Skript Files " + scriptPath,);

    cScriptPath = scriptPath;

    lines.clear();
    lines.resize(numLines);

    for(size_t i = 0; i < numLines; ++i){
        std::getline(scriptFile, lines[i]);
    }

    scriptFile.close();

    if(lines.empty()){
        lines = {""};
    }

    if(!mainCursor.isOnValidPosition()){

        //
        mainCursor.cursorLine = lines.size() - 1;
        mainCursor.cursorCol = lines[mainCursor.cursorLine].size();
    }

    pushUndo();
}

void CodeEditor::save(){

    if(cScriptPath == NULLSTR){
        return;
    }
    
    ASSERT(fs::exists(cScriptPath), "Skript Datei existiert nicht");

    LOG << "** [Editor] Speichere File unter : " << cScriptPath << endln;

    std::ofstream outFile(cScriptPath);

    for(size_t idx = 0; idx < lines.size(); idx++){
        outFile << lines[idx] + ((idx < lines.size() - 1) ? "\n" : "");
    }

    outFile.close();
}

void CodeEditor::highlightLine(size_t lineIdx, const  ImU32& frameColor, const  ImU32& fillColor, float relThickness,
                                bool strip, bool markEmptyLines, bool ignoreEmptyLines){
    
    // Wird relativ häufig getriggert, da linePositon und lines buffer teilweise einen Frame hintereinander
    // herhängen, zudem ist es nicht wichtig wenn einen Frame mal ne zeile nicht gehighlightet wird

    // RETURNING_ASSERT(lineIdx < lines.size(), "angegebener lineIndex existiert nicht",);

    if(lineIdx >= lines.size()){
        return;
    }

    highlightLineSection(lineIdx, 0, lines[lineIdx].size(), frameColor, fillColor, relThickness, strip, markEmptyLines);
}

void CodeEditor::highlightLineSection(size_t lineIdx, size_t startAtColIdx, size_t stopAtColIdx,
        const ImU32& frameColor, const  ImU32& fillColor, float relThickness, bool strip,
        bool markEmptyLines, bool ignoreEmptyLines){

    // -1 führt zum buffer overflow und entspricht damit dem numerischen maximum der signed variable
    if(lineIdx == -1){
        return;
    }

    if((lineIdx > linePositions.size() - 1) || (linePositions.size() != lines.size())){
        return;
    }

    RETURNING_ASSERT(lineIdx < lines.size(), "lines Buffer enthält übergebenen line Idx nicht",);
    
    const std::string& line = lines[lineIdx];

    if(ignoreEmptyLines && line.size() == 0){
        return;
    }

    bool fullLine = (startAtColIdx == 0) && (stopAtColIdx >= line.size());
    bool skipDrawing = false;

    ImVec2 startPos = linePositions[lineIdx];
    ImVec2 textSize = lines[lineIdx].size() > 0 ? ImGui::CalcTextSize(lines[lineIdx].c_str())
                                                : ImVec2(4, ImGui::CalcTextSize(lines[lineIdx].c_str()).y);

    // Index des ersten sichtbaren Zeichens
    size_t firstNonSpace = line.find_first_not_of(" \t");

    if(startAtColIdx > line.size()){
        startAtColIdx = line.size();
    }

    if(stopAtColIdx > line.size()){
        stopAtColIdx = line.size();
    }

    if((startAtColIdx == stopAtColIdx) && startAtColIdx >= 0 && !fullLine){
        return;
    }

    // Leere zeile wird mit halben Leerzeichen länge gehighlightet
    if (firstNonSpace == std::string::npos && line.size() == 0 && (startAtColIdx == 0 || fullLine) && markEmptyLines){
            
        textSize.x += ImGui::CalcTextSize("|").x;
    }
    else if(line.size() == 0 && (startAtColIdx != 0)){

        skipDrawing = true;
    }
    // Zeile die nur Leerzeichen/Tabs enthält wird vollständig gehighlightet
    else if(firstNonSpace == std::string::npos && fullLine){

        // nur letztes leerzeichen markieren
        // textSize.x = ImGui::CalcTextSize("|").x;
        // startPos.x += ImGui::CalcTextSize(line.substr(0, line.size()).c_str()).x;
    }
    else{

        if(startAtColIdx < firstNonSpace && stopAtColIdx > firstNonSpace && strip){

            startAtColIdx = firstNonSpace;
        }

        if((startAtColIdx == stopAtColIdx) && startAtColIdx > 0){
            return;
        }

        textSize.x = ImGui::CalcTextSize(line.substr(startAtColIdx, stopAtColIdx - startAtColIdx).c_str()).x;
        startPos.x += ImGui::CalcTextSize(line.substr(0,startAtColIdx).c_str()).x;
    }

    if(skipDrawing){
        return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddRectFilled(
        startPos,
        startPos + textSize,
        fillColor);

    drawList->AddRect(
        startPos,
        startPos + textSize,
        frameColor,
        0.0f, 0,
        relThickness * ImGui::GetFont()->Scale * ImGui::GetIO().FontGlobalScale );
}

std::vector<ImWchar> CodeEditor::getInputChars(){

    std::vector<ImWchar> inputChars;

    // Einfacher Textinput
    ImGuiIO& io = ImGui::GetIO();
    for (int n = 0; n < io.InputQueueCharacters.Size; n++) {
        inputChars.emplace_back(io.InputQueueCharacters[n]);
    }
    io.InputQueueCharacters.resize(0); // ersetzt io.ClearInputCharacters()
    ImGui::SetNextFrameWantCaptureKeyboard(true);

    return inputChars;
}

void CodeEditor::setCursorToBottom(){

    mainCursor.cursorLine = lines.size() - 1;
    mainCursor.cursorCol = lines[mainCursor.cursorLine].size();

    dragCursor.placeOnCursor(mainCursor);
}

bool CodeEditor::isMainCursorBeforeDragged(){
    
    //
    if(mainCursor.cursorLine == dragCursor.cursorLine){
        return mainCursor.cursorCol < dragCursor.cursorCol;
    }
    else{
        return mainCursor.cursorLine < dragCursor.cursorLine;
    }
}

std::tuple<size_t, size_t, size_t, size_t> CodeEditor::getCursorRange(){

    bool mainCursorIsUpper = isMainCursorBeforeDragged();

    return std::make_tuple((mainCursorIsUpper ? mainCursor : dragCursor).cursorLine,
                            (!mainCursorIsUpper ? mainCursor : dragCursor).cursorLine,
                            (mainCursorIsUpper ? mainCursor : dragCursor).cursorCol,
                            (!mainCursorIsUpper ? mainCursor : dragCursor).cursorCol);
}

void CodeEditor::deleteCursorRange(){

    //
    auto [startAtLine, stopAtLine, startAtCol, stopAtCol] = getCursorRange();

    // LOG << startAtLine << " " << stopAtLine << " " << startAtCol << " " << stopAtCol << endln;

    bool startLineIsFull = false;
    bool stopLineIsFull = false;

    for(size_t line = stopAtLine; line >= startAtLine; ){

        size_t startAtLineCol = 0;
        size_t stopAtLineCol = lines[line].size();

        if(line == startAtLine){ startAtLineCol = startAtCol; }
        if(line == stopAtLine){ stopAtLineCol = stopAtCol; }

        bool fullLine = ((startAtLineCol == 0) && (stopAtLineCol == lines[line].size())) || lines[line].size() == 0;

        if(line == startAtLine && fullLine){ startLineIsFull = true; }
        else if(line == stopAtLine && fullLine){ stopLineIsFull = true; }

        if(fullLine && line == stopAtLine){
            
            lines[line] = "";
        }
        else if(fullLine){

            lines.erase(lines.begin() + line);
        }
        else{

            lines[line].erase(startAtLineCol, stopAtLineCol - startAtLineCol);
        }

        if(line > 0) line--;
        else break;
    }

    if((startAtLine != stopAtLine) && !startLineIsFull && !stopLineIsFull && startAtLine + 1 < lines.size()){

        lines[startAtLine] += lines[startAtLine + 1];
        lines.erase(lines.begin() + startAtLine + 1);
    }

    if(lines.empty()){
        lines = {""};
    }

    mainCursor.placeOnClick(startAtLine, startAtCol);
    dragCursor.placeOnCursor(mainCursor);

    // if(startLineIsFull && startAtLine > 0 && stopLineIsFull){
        
    //     mainCursor.deleteOnCursor();
    // }
}

void CodeEditor::copyCursorRange(){

    //
    auto [startAtLine, stopAtLine, startAtCol, stopAtCol] = getCursorRange();

    //
    std::string clipboardContent = "";

    for(size_t line = startAtLine; line < stopAtLine + 1; line++){

        size_t startAtLineCol = 0;
        size_t stopAtLineCol = lines[line].size();

        if(line == startAtLine){ startAtLineCol = startAtCol; }
        if(line == stopAtLine){ stopAtLineCol = stopAtCol; }

        bool fullLine = (startAtLineCol == 0) && (stopAtLineCol == lines[line].size());

        if (lines[line].empty()) {
            clipboardContent += "";
        } else {
            clipboardContent += fullLine 
                ? lines[line] 
                : lines[line].substr(startAtLineCol, stopAtLineCol - startAtLineCol);
        }

        if (line != stopAtLine) {
            clipboardContent += "\n"; // Newline nur wenn nicht letzte Zeile
        }
    }

    setClipboard(clipboardContent);
}

void CodeEditor::pasteOnMainCursor(){

    //
    std::vector<std::string> splittedClipboardContent = string::split(getClipboard(), '\n');

    //
    for(size_t lineIdx = 0; lineIdx < splittedClipboardContent.size(); lineIdx++){

        //
        if(lineIdx == 0){
            lines[mainCursor.cursorLine].insert(mainCursor.cursorCol, splittedClipboardContent[lineIdx]);
        }
        else{

            //
            lines.insert(lines.begin() + mainCursor.cursorLine + lineIdx, splittedClipboardContent[lineIdx]);
        }
    }
}

void CodeEditor::pasteBesideMainCursor(){

    //
    std::vector<std::string> splittedClipboardContent = string::split(getClipboard(), '\n');

    //
    for(size_t lineIdx = 0; lineIdx < splittedClipboardContent.size(); lineIdx++){

        lines[mainCursor.cursorLine + lineIdx].insert(
            // std::min(lines[mainCursor.cursorLine + lineIdx].size(), mainCursor.cursorCol),
            lines[mainCursor.cursorLine + lineIdx].size(),
            splittedClipboardContent[lineIdx]);
    }
}

std::string CodeEditor::exportString(){

    std::string exportedString = "";

    for(size_t lineIdx = 0; lineIdx < lines.size(); lineIdx++){
        
        if(lines[lineIdx].empty()){

            exportedString += "";
        }
        else{

            exportedString += lines[lineIdx];
        }

        if(lineIdx < lines.size() - 1){
            exportedString += "\n";
        }
    }

    return exportedString;
}

void CodeEditor::importString(const std::string& str){

    //
    lines.clear();

    //
    if(str.length() == 0){

        lines = {""};
    }
    else{

        lines = string::split(str, '\n');
    }
}

void CodeEditor::clearVersionStacks(){

    undoStack.clear();
    redoStack.clear();
}

void CodeEditor::pushUndo() {
    
    if(writeRestriction){
        return;
    }

    std::string current = exportString();

    if (!undoStack.empty() && undoStack.back().linesString == current
        && undoStack.back().cursorLine == mainCursor.cursorLine
        && undoStack.back().cursorCol == mainCursor.cursorCol) {
        return; // nichts Neues
    }

    undoStack.push_back({current, mainCursor.cursorLine, mainCursor.cursorCol});
    redoStack.clear(); // Redo-Kette bricht bei neuer Aktion ab
}

void CodeEditor::undo() {

    if (undoStack.size() < 2) return; // mindestens 2 Zustände nötig

    // aktuellen Zustand auf Redo-Stack
    redoStack.push_back({exportString(), mainCursor.cursorLine, mainCursor.cursorCol});

    // letzten Zustand verwerfen
    undoStack.pop_back();

    // vorletzten Zustand anwenden
    const auto& prev = undoStack.back();
    importString(prev.linesString);
    mainCursor.cursorLine = prev.cursorLine;
    mainCursor.cursorCol = prev.cursorCol;

    if(prev.linesString.size() == 0 && lines.size() == 0){
        
        lines = {""};
    }
}

void CodeEditor::redo() {

    if (redoStack.empty()) return;

    // Zustand von Redo holen
    const auto next = redoStack.back();
    redoStack.pop_back();

    // aktuellen Zustand auf Undo ablegen
    undoStack.push_back({exportString(), mainCursor.cursorLine, mainCursor.cursorCol});

    // Redo-Zustand anwenden
    importString(next.linesString);
    mainCursor.cursorLine = next.cursorLine;
    mainCursor.cursorCol = next.cursorCol;

    if(next.linesString.size() == 0 && lines.size() == 0){
        
        lines = {""};
    }
}

float CodeEditor::getLineHeight(){

    ImGui::PushFont(ttfJuliaMonoItalic);
    float lineHeight = linePositions.size() > 1 ? linePositions[1].y - linePositions[0].y :
                         ImGui::GetTextLineHeight() * (1 + lineSpacing);
    ImGui::PopFont();

    return lineHeight;
}

void CodeEditor::render(const ImVec2& leftCorner, const ImVec2& size, bool ignoreDummys, bool disableAutomaticScrolling){

    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(leftCorner);
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowBgAlpha(0);

    ImGui::Begin(windowName.c_str(), nullptr, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiFocusedFlags_RootAndChildWindows); // ImGuiWindowFlags_MenuBar
    
    // Wir setzen das Flag, um Scrollen zu blockieren, wenn ALT gedrückt ist
    ImGuiWindowFlags childFlags = ImGuiWindowFlags_None;
    if (IsKeyDown(KEY_LEFT_ALT)) {
        childFlags |= ImGuiWindowFlags_NoScrollWithMouse;
    }

    // if (ImGui::BeginMenuBar())
    // {
    //     // ImGui::MenuItem("hallo");
    //     ImGui::EndMenuBar();
    // }

    // ImGui::Text("Script : %s", cScriptPath.c_str());

    // TextCentered("Script : %s", cScriptPath.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, ImGui::GetTextLineHeight() * lineSpacing));
    ImGui::PushFont(ttfJuliaMonoItalic);

    //
    if(IsKeyDown(KEY_LEFT_ALT)) {

        float wheelMove = GetMouseWheelMove();
        
        if(wheelMove != 0) {

            // Skalierungsfaktor anpassen, clamp zwischen 0.5 und 3, sodass UI nicht vollkommen aus
            // dem Bereich einer sinnvollen Skalierung entweicht
            ImGui::GetFont()->Scale += wheelMove * 0.1f;
            ImGui::GetFont()->Scale = Clamp(ImGui::GetFont()->Scale, 0.5f, 3.0f);
        }
    }

    float numberColumnWidth = ImGui::CalcTextSize(
        ("" + std::string(maxDigits, '0') + "__").c_str()).x;

    if(renderLineNumbers){

        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 0));

        ImGui::BeginChild((windowName + "numbersID").c_str(), ImVec2(numberColumnWidth, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | childFlags);
        ImGui::SetScrollY(syncedScrollY);

        if(!ignoreDummys) ImGui::Text("");

        for (size_t lineIdx = 0; lineIdx < lines.size(); lineIdx++) {

            ImGui::Text("%*d", maxDigits, lineIdx + 1);
        }

        if(!ignoreDummys) ImGui::Text("");

        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::SameLine();
    }

    ImGuiWindowFlags editflags = !disableAutomaticScrolling ?
        ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar | childFlags :
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | childFlags;

    if(renderLineNumbers){
        ImGui::BeginChild((windowName + "codeeditor").c_str(), ImVec2(0, 0), true,
            editflags);
    }

    if(!ignoreDummys) ImGui::Text("");

    //
    mainCursor.validatePosition();
    dragCursor.validatePosition();

    mainCursor.nullRenderingSettings();
    dragCursor.nullRenderingSettings();

    if(linePositions.size() != lines.size()){

        linePositions.resize(lines.size());
    }

    hoveredCol = -1;
    hoveredLine = -1;

    //
    if (!vScrollDragging) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isVScrollActiveChild()) {
            vScrollDragging = true;
        }
    } else {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            vScrollDragging = false;
        }
    }

    //
    float lineHeight = ImGui::GetTextLineHeight() * (1 + lineSpacing);

    syncedScrollY = ImGui::GetScrollY();
    ImVec2 mousePos = ImGui::GetMousePos();

    //
    for (size_t lineIdx = 0; lineIdx < lines.size(); lineIdx++) {

        ImGui::Text(" ");
        ImGui::SameLine();

        linePositions[lineIdx] = ImGui::GetCursorScreenPos();
        const ImVec2& linePos = linePositions[lineIdx];

        // mit 3/4 und 1/4 fühlt sich das irgendwie am natürlichsten an
        if (ImGui::IsWindowHovered() && !vScrollDragging && ((mousePos.y >= linePos.y - lineHeight/4 && mousePos.y < linePos.y + 3*lineHeight/4) ||
            (lineIdx == 0 && mousePos.y < linePos.y + 3*lineHeight/4) ||
            (lineIdx == lines.size() - 1 && mousePos.y >= linePos.y - lineHeight/4))) {
            
            hoveredLine = lineIdx;

            hoveredCol = -1;
            float cursorPosXInLine = mousePos.x - linePos.x;

            float prevSize = -1;
            float curSize;

            for(size_t i = 0; i < lines[lineIdx].size(); i++){

                curSize = ImGui::CalcTextSize(lines[lineIdx].substr(0, i).c_str()).x;

                if(cursorPosXInLine < 0){

                    hoveredCol = 0;
                    break;
                }

                if(curSize >= cursorPosXInLine && prevSize < cursorPosXInLine){
                    
                    float currDiff = curSize - cursorPosXInLine; 
                    float prevDiff = cursorPosXInLine - prevSize;

                    if(currDiff > prevDiff){
                        hoveredCol = i == 0 ? 0 : i - 1;
                    }
                    else{
                        hoveredCol = i;
                    }

                    break;
                }

                prevSize = curSize;
            }

            if(hoveredCol == -1 && cursorPosXInLine > 0){
                hoveredCol = lines[lineIdx].size();
            }
            // maus ist hinter Rand
            else if(hoveredCol == -1){
                hoveredCol = 0;
            }

            if ((ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) ||
                ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {

                //
                dragCursor.placeOnClick(hoveredLine, hoveredCol);
            }
            else if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)){

                mainCursor.placeOnClick(hoveredLine, hoveredCol);
                dragCursor.placeOnCursor(mainCursor);
            }
        }

        ImGui::Text("%s", lines[lineIdx].c_str());

        mainCursor.setUpRendering(linePos, lineIdx);
        dragCursor.setUpRendering(linePos, lineIdx);

        // if (lineIdx == hoveredLine)
        //     ImGui::PopStyleColor();
    }

    if(!ignoreDummys) ImGui::Text("");

    float textLineHeight = linePositions.size() > 1 ? linePositions[1].y - linePositions[0].y : 0;

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImGui::SetWindowFocus(); // Child-Fenster fokussieren
    }

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        mainCursor.isOnValidPosition()) {

        //
        bool changedLines = true;

        //
        selectionNotActive = (mainCursor.cursorLine == dragCursor.cursorLine) &&
                                (mainCursor.cursorCol == dragCursor.cursorCol);

        const auto inputChars = getInputChars();
        bool inputEntered = !inputChars.empty();

        entered = false;
        if(singleLineWin && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(ImGuiKey_Enter)){
            entered = true;
        }

        // Einzige Veränderung von lines
        if(!singleLineWin && !writeRestriction && ImGui::IsKeyPressed(ImGuiKey_Enter)){

            !selectionNotActive ? deleteCursorRange() : void();
            selectionNotActive = true;
            
            mainCursor.enterParagraphOnCursor();
            mainCursor.validatePosition();

            // if(mainCursor.cursorLine == lines.size() - 1){

            //     ImGui::SetScrollY(ImGui::GetScrollMaxY());
            // }

            // ImGui::SetScrollY(ImGui::GetScrollY() + lineHeight);
        }
        else if(!writeRestriction && ImGui::IsKeyPressed(ImGuiKey_Backspace)){

            selectionNotActive ? mainCursor.deleteOnCursor() : deleteCursorRange();
            selectionNotActive = true;
        }
        else if(!writeRestriction && ImGui::IsKeyPressed(ImGuiKey_Tab)){

            !selectionNotActive ? deleteCursorRange() : void();
            selectionNotActive = true;

            mainCursor.enterCharakterOnCursor("    ");
        }
        else if(!selectionNotActive && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C)){
        
            copyCursorRange();
            changedLines = false;
        }
        else if(selectionNotActive && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C)){
        
            mainCursor.enterCharakterOnCursor("^C");
        }
        else if(!writeRestriction && !selectionNotActive && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_X) ){

            copyCursorRange();
            deleteCursorRange();
        }
        else if(!writeRestriction && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V)){

            //
            !selectionNotActive ? deleteCursorRange() : void();
            selectionNotActive = true;

            pasteOnMainCursor();
        }
        else if(!writeRestriction && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_B)){

            //
            !selectionNotActive ? deleteCursorRange() : void();
            selectionNotActive = true;

            pasteBesideMainCursor();
        }
        else if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_A)){

            //
            clearClipboard();
            changedLines = false;
        }
        else if(!writeRestriction && inputEntered){

            !selectionNotActive ? deleteCursorRange() : void();
            selectionNotActive = true;
            
            mainCursor.enterTextOnCursorPos(inputChars);
        }
        else{
            changedLines = false;
        }

        if(!writeRestriction && selectionNotActive && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Y)){

            //
            undo();
            // LOG << mainCursor.cursorLine << " " << mainCursor.cursorCol << endln;
        }
        else if(!writeRestriction && selectionNotActive && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z)){

            //
            redo();
        }

        if(changedLines == true || (undoStack.empty() && redoStack.empty())){

            pushUndo();
        }
        
        if(selectionNotActive){

            dragCursor.placeOnCursor(mainCursor);
        }
    }

    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
        && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S)){

        save();
    }

    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
        && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_R)){
        
        openScript(cScriptPath);

        mainCursor.validatePosition();
        dragCursor.placeOnCursor(mainCursor);
    }

    if(ImGui::IsKeyDown(ImGuiKey_LeftShift)){

        // den Drag Cursor steuern der normale bleibt auf seiner Position
        dragCursor.stear();
    }
    else{

        //
        if(mainCursor.stear()){

            dragCursor.placeOnCursor(mainCursor);
        }
    }

    if(!disableAutomaticScrolling && selectionNotActive && mainCursor.positionChanged()){

        mainCursor.drawCursor = !scrollToCursor(mainCursor, scrollOffsetLines);
    }
    else if(!disableAutomaticScrolling && !selectionNotActive && dragCursor.positionChanged()){

        dragCursor.drawCursor = !scrollToCursor(dragCursor, scrollOffsetLines);
    }

    if(!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)){

        mainCursor.setOnInValidPosition();
        dragCursor.setOnInValidPosition();
    }

    //
    mainCursor.cache();
    dragCursor.cache();

    // Render
    if(selectionNotActive && mainCursor.isOnValidPosition() && mainCursor.drawCursor){
        highlightLine(mainCursor.cursorLine, ImColor(255,255,255,255), ImColor(255,255,255,0));
    }
    else if(!selectionNotActive){

        //
        auto [startAtLine, stopAtLine, startAtCol, stopAtCol] = getCursorRange();

        for(size_t line = startAtLine; line < stopAtLine + 1; line++){

            size_t startAtLineCol = 0;
            size_t stopAtLineCol = lines[line].size();

            if(line == startAtLine){
                startAtLineCol = startAtCol;
            }
            
            if(line == stopAtLine){

                stopAtLineCol = stopAtCol;
                // if(stopAtLineCol > lines[line].size())
                //     stopAtLineCol = lines[line].size();
            }

            highlightLineSection(line, startAtLineCol, stopAtLineCol, IM_COL32(255,255,0,255), IM_COL32(255,255,0,0),
                                    0.2f, false, true, false);
        }
    }

    // Render
    if(hoveredLine != -1){
        highlightLine(hoveredLine, ImColor(0,255,0,255), ImColor(255,255,255,0));
    }

    //
    mainCursor.draw();
    dragCursor.draw();

    //
    ImGui::PopFont();
    ImGui::PopStyleVar();

    if(renderLineNumbers){
        ImGui::EndChild();
    }

    ImGui::End();
}

bool CodeEditor::scrollToCursor(Cursor& cursor, int offsetLines, int offsetColumns) {

    if (!cursor.isOnValidPosition() || linePositions.size() < 2) // || linePositions.size() != lines.size()) 
        return false;

    // Aktuelle Scrollposition
    float scrollY       = ImGui::GetScrollY();
    float scrollX       = ImGui::GetScrollX();
    float windowHeight  = ImGui::GetWindowHeight();
    float windowWidth   = ImGui::GetWindowWidth();

    // Zeilenhöhe bestimmen
    float lineHeight = linePositions.size() > 2 
        ? linePositions[1].y - linePositions[0].y 
        : ImGui::GetTextLineHeight() * (1 + lineSpacing);

    float offsetY = offsetLines * lineHeight;

    // Cursor Y-Position (vertikal)
    float cursorY = cursor.cursorLine * lineHeight;

    float topVisible    = scrollY + offsetY;
    float bottomVisible = scrollY + windowHeight - offsetY - lineHeight;

    float targetScrollY = scrollY;
    if (cursorY < topVisible) {
        targetScrollY = cursorY - offsetY;
    } else if (cursorY > bottomVisible) {
        targetScrollY = cursorY - windowHeight + offsetY + lineHeight;
    }

    // ---------------------------------------
    // Horizontaler Teil
    // ---------------------------------------
    float charWidth   = ImGui::CalcTextSize("M").x; // Monospace: Breite eines Zeichens
    float cursorX     = cursor.cursorCol * charWidth;
    float offsetX     = offsetColumns * charWidth;

    float leftVisible  = scrollX + offsetX;
    float rightVisible = scrollX + windowWidth - offsetX - charWidth;

    float targetScrollX = scrollX;
    if (cursorX < leftVisible) {
        targetScrollX = cursorX - offsetX;
    } else if (cursorX > rightVisible) {
        targetScrollX = cursorX - windowWidth + offsetX + charWidth;
    }

    // ---------------------------------------
    // Smooth Scroll (kann angepasst werden)
    // ---------------------------------------
    float speed = 1.0f; // 0 < speed <= 1
    bool changed = false;

    if (targetScrollY != scrollY) {
        ImGui::SetScrollY(scrollY + (targetScrollY - scrollY) * speed);
        changed = true;
    }
    if (targetScrollX != scrollX) {
        ImGui::SetScrollX(scrollX + (targetScrollX - scrollX) * speed);
        changed = true;
    }

    return changed;
}

bool CodeEditor::isVScrollActiveChild() {

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (!window) return false;

    ImVec2 scrollBarStart(window->Pos.x + window->Size.x - ImGui::GetStyle().ScrollbarSize, window->Pos.y);
    ImVec2 scrollBarEnd(window->Pos.x + window->Size.x, window->Pos.y + window->Size.y);

    ImVec2 mousePos = ImGui::GetIO().MousePos;

    return mousePos.x >= scrollBarStart.x && mousePos.x <= scrollBarEnd.x &&
           mousePos.y >= scrollBarStart.y && mousePos.y <= scrollBarEnd.y;
}