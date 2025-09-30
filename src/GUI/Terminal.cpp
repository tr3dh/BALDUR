#include "Terminal.h"
#include <regex>

Terminal::Terminal(){

    // Editor setup
    m_editor.renderLineNumbers = false;
    m_editor.writeRestriction = true;
    m_editor.scrollOffsetLines = 2;

    //
    m_promptImput.renderLineNumbers = false;
    m_promptImput.scrollOffsetLines = 1;
    m_promptImput.singleLineWin = true;

    // shell setup
    m_shell.start(shellStartUpCommand);
}

Terminal::~Terminal(){

    m_shell.stop();
}

std::string stripAnsi(const std::string& input) {

    // Regex für typische ANSI-Sequenzen
    static const std::regex csi("\x1B\\[[0-9;?]*[ -/]*[@-~]");  // ESC [ ... 
    static const std::regex osc("\x1B\\].*?\x07");              // ESC ] ... BEL
    static const std::regex esc("\x1B[@-Z\\^_`a-z{|}~]");       // einzelne ESC-Sequenzen

    std::ostringstream oss;
    std::string out = std::regex_replace(input, csi, "");
    out = std::regex_replace(out, osc, "");
    out = std::regex_replace(out, esc, "");

    oss << out;
    return oss.str();
}

void Terminal::sendCommand(const std::string& cmd){

    //
    if(cmd == "^C"){

        m_shell.killCurrentProcess();
        return;
    }

    //
    m_shell.sendCommand(cmd);
}

void Terminal::render(const ImVec2& leftCorner, const ImVec2& size){

    size_t sizeBefore = shellContent.size();
    shellContent += stripAnsi(m_shell.getOutput());
    size_t sizeAfter = shellContent.size();

    if(sizeBefore != sizeAfter){

        m_editor.importString(shellContent);
        m_editor.setCursorToBottom();
    }

    //
    float promptLineHeight = (1 * m_editor.getLineHeight() + 2 * ImGui::GetStyle().WindowPadding.y);
    m_editor.render(leftCorner, size + ImVec2(0,-promptLineHeight));
    
    //
    ImGui::PushFont(ttfJuliaMonoItalic);

    //
    ImGui::SetNextWindowPos(leftCorner + ImVec2(0, size.y - promptLineHeight));
    ImGui::Begin(("##editor_label" + m_promptImput.windowName).c_str(), nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
    ImGui::Text("$");
    ImGui::End();

    float dollarWidth = ImGui::CalcTextSize("$").x;
    float spacing     = ImGui::GetStyle().ItemSpacing.x;

    ImGui::PopFont();
    
    m_promptImput.render(
        leftCorner + ImVec2(dollarWidth + 2 * spacing, size.y - promptLineHeight),
        {size.x - dollarWidth - spacing, promptLineHeight},
        true, true
    );

    if(m_promptImput.entered){

        sendCommand(m_promptImput.exportString());
        m_promptImput.importString("");
    }
}