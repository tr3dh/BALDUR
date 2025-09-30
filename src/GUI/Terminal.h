#pragma once

#include "CodeEditor.h"

struct Terminal{

    std::string shellContent = "";
    std::string shellStartUpCommand = "cmd.exe /K";

    CodeEditor m_editor;
    CodeEditor m_promptImput;
    PseudoTerminal m_shell;

    void sendCommand(const std::string& cmd);

    Terminal();
    ~Terminal();

    void render(const ImVec2& leftCorner, const ImVec2& size);
};