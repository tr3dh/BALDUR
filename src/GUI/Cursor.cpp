#include "Cursor.h"

void Cursor::recalCursor(){

    if(cursorCol == 0){
        lock = CursorLock::Min;
    }
    else if(cursorCol == (*lines)[cursorLine].size()){
        lock = CursorLock::Max;
    }
    else{
        lock = CursorLock::None;
        lockedCol = cursorCol;
    }
}

void Cursor::draw(){

    if(drawCursor){

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine(cursorPos, {cursorPos.x, cursorPos.y + ImGui::GetTextLineHeight()}, IM_COL32(0,255,0,255),
        2.5f * ImGui::GetFont()->Scale * ImGui::GetIO().FontGlobalScale);
    }
}

bool Cursor::stear(){

    // Eingabe steuerung
    if(cursorLine == -1 || cursorCol == -1){
        return false;
    }

    size_t cachedCursorLine = cursorLine;
    size_t cachedCursorCol = cursorCol;

    if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {

        if (cursorCol > 0){

            cursorCol--;
            if(cursorCol == 0 && lock == CursorLock::None){
                lock = CursorLock::Min;
            }
        }
        else if (cursorLine > 0) {

            cursorLine--;
            cursorCol = (*lines)[cursorLine].size();
        }

        recalCursor();
    }
    if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {

        if (cursorCol < (*lines)[cursorLine].size()){
            
            cursorCol++;
        }
        else if (cursorLine < (*lines).size() - 1) {
            cursorLine++;
            cursorCol = 0;
        }

        recalCursor();
    }
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {

        if (cursorLine > 0) {
            cursorLine--;
            if(lock == CursorLock::Max){
                cursorCol = (size_t)(*lines)[cursorLine].size();
            }
            else if(lock == CursorLock::Min){
                cursorCol = 0;
            }
            else if(lockedCol != -1){
                cursorCol = lockedCol < (size_t)(*lines)[cursorLine].size() ? lockedCol : (size_t)(*lines)[cursorLine].size();
            }
            else{
                cursorCol = std::min(cursorCol, (size_t)(*lines)[cursorLine].size());
                lockedCol = cursorCol;
            }
        }
    }
    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {

        if (cursorLine < (*lines).size() - 1) {
            cursorLine++;
            if(lock == CursorLock::Max){
                cursorCol = (size_t)(*lines)[cursorLine].size();
            }
            else if(lock == CursorLock::Min){
                cursorCol = 0;
            }
            else if(lockedCol != -1){
                cursorCol = lockedCol < (size_t)(*lines)[cursorLine].size() ? lockedCol : (size_t)(*lines)[cursorLine].size();
            }
            else{
                cursorCol = std::min(cursorCol, (size_t)(*lines)[cursorLine].size());
            }
        }
    }

    return cachedCursorLine != cursorLine || cachedCursorCol != cursorCol;
}

void Cursor::nullRenderingSettings(){

    drawCursor = false;
    cursorPos = {-1,-1};
    cursorLinePos = {-1,-1};
}

void Cursor::setUpRendering(const ImVec2& linePos, size_t lineIdx){

    if(cursorLine == lineIdx){

        drawCursor = true;
        cursorPos = linePos;
        cursorLinePos = linePos;

        cursorPos.x += ImGui::CalcTextSize((*lines)[lineIdx].substr(0, cursorCol).c_str()).x;
    }
}

void Cursor::placeOnClick(size_t hoveredLine, size_t hoveredCol){

    cursorLine = hoveredLine;
    cursorCol = hoveredCol;
    lockedCol = cursorCol;

    recalCursor();
}

void Cursor::placeOnCursor(const Cursor& other){

    cursorLine = other.cursorLine;
    cursorCol = other.cursorCol;

    lockedCol = other.cursorCol;
    lock = other.lock;
}

void Cursor::validatePosition(){
    
    if(!isOnValidPosition()){
        return;
    }

    if(cursorLine > (*lines).size() - 1){
        cursorLine -= cursorLine - ((*lines).size() - 1);
    }

    if(cursorCol > (*lines)[cursorLine].size()){
        cursorCol -= cursorCol - (*lines)[cursorLine].size();
    }
}

bool Cursor::isOnValidPosition(){

    return cursorLine != -1 && cursorCol != -1;
}

void Cursor::setOnInValidPosition(){

    cursorLine = -1;
    cursorCol = -1;
}

void Cursor::enterCharakterOnCursor(const char* c){

    (*lines)[cursorLine].insert(cursorCol, c);
    cursorCol+=strlen(c);
}

void Cursor::enterTextOnCursorPos(const std::vector<ImWchar>& chars){

    for (int n = 0; n < chars.size(); n++) {

        ImWchar c = chars[n];

        // Buffer für UTF-8-Zeichen
        char utf8Char[5] = {0};
        ImTextCharToUtf8(utf8Char, c);

        switch (c){

            // Möglichkeit für Ersetzungen wie "Ae" für 'Ä' etc.
            case L'\n':{

            }
            default:{

                enterCharakterOnCursor(utf8Char);
                break;
            }
        }

        if(c == '('){
            enterCharakterOnCursor(")");
            cursorCol--;
        }
        if(c == '"'){
            enterCharakterOnCursor("\"");
            cursorCol--;
        }
        if(c == '['){
            enterCharakterOnCursor("]");
            cursorCol--;
        }
        if(c == '{'){
            enterCharakterOnCursor("}");
            cursorCol--;
        }
    }
}

void Cursor::enterParagraphOnCursor(){

    // Zeilenumbruch
    std::string& line = (*lines)[cursorLine];

    // Index des ersten sichtbaren Zeichens
    size_t firstNonSpace = line.find_first_not_of(" \t");
    if (firstNonSpace == std::string::npos && line.size() == 0){
        firstNonSpace = 0;
    }
    else if(firstNonSpace == std::string::npos){
        firstNonSpace = line.size();
    }

    std::string newLine = line.substr(0,firstNonSpace) + line.substr(cursorCol);
    line = line.substr(0, cursorCol);
    (*lines).insert((*lines).begin() + cursorLine + 1, newLine);

    cursorLine++;
    cursorCol = firstNonSpace;

    recalCursor();
}

void Cursor::deleteOnCursor() {
    if(cursorCol > 0){
        std::string& line = (*lines)[cursorLine];

        // Suche den Start des vorherigen UTF-8 Zeichens
        size_t prevCharStart = cursorCol - 1;
        while(prevCharStart > 0 && (line[prevCharStart] & 0xC0) == 0x80) {
            // Solange es ein Folgebite ist (10xxxxxx), eins zurück
            prevCharStart--;
        }

        line.erase(prevCharStart, cursorCol - prevCharStart);
        cursorCol = prevCharStart;
    }
    else if(cursorCol == 0 && cursorLine > 0){
        cursorCol = (*lines)[cursorLine - 1].size();
        (*lines)[cursorLine - 1] += (*lines)[cursorLine];
        (*lines).erase((*lines).begin() + cursorLine);
        cursorLine--;
    }

    recalCursor();
}

void Cursor::cache(){

    cachedCursorLine = cursorLine;
    cachedCursorCol = cursorCol;
}

bool Cursor::positionChanged(){

    return (cachedCursorLine != cursorLine) || (cachedCursorCol!= cursorCol);
}