#include "ImGuiStyleDecls.h"

ImFont* ttfJuliaMonoItalic = nullptr;
ImFont* ttfJuliaMonoBold = nullptr;

void scaleImguiUI(const float& UIZoom){

    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = UIZoom;
}

// Funktion muss einmal aufgerufen werden bevor die while-Schleife startet
void SetupImGuiStyle()
{
    // Style ref abrufen
    ImGuiStyle& style = ImGui::GetStyle();

    //
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.ChildRounding = 4.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 5.0f;

    style.WindowPadding = ImVec2(10,10);
    style.FramePadding  = ImVec2(6,4);
    style.ItemSpacing   = ImVec2(8,6);
    style.IndentSpacing = 20.0f;

    //
    ImVec4 backgroundColor    = ImVec4(0.08f, 0.06f, 0.04f, 1.0f); // Sehr dunkles Braun-Schwarz
    ImVec4 baseUIColor        = ImVec4(1.0f,  0.45f, 0.0f,  1.0f); // Kräftiges Orange
    ImVec4 baseUIElemColor    = ImVec4(0.25f, 0.18f, 0.08f, 1.0f); // Dunkles Braun-Grau
    ImVec4 hoverUIElemColor   = ImVec4(0.45f, 0.30f, 0.10f, 1.0f); // Mittleres Braun-Orange
    ImVec4 activeUIElemColor  = ImVec4(0.35f, 0.22f, 0.05f, 1.0f); // Gedämpftes dunkles Orange-Braun

    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        ImVec4& col = style.Colors[i];
        if (col.x == 1.0f && col.y == 1.0f && col.z == 1.0f) // Weiß
        {
            col = baseUIColor;
        }
        else if (col.x == 0.0f && col.y == 0.0f && col.z == 1.0f) // Blau
        {
            col = baseUIColor;
        }
    }

    // 
    style.Colors[ImGuiCol_WindowBg] = backgroundColor;
    style.Colors[ImGuiCol_TitleBg] = backgroundColor;
    style.Colors[ImGuiCol_TitleBgActive] = activeUIElemColor;
    style.Colors[ImGuiCol_TitleBgCollapsed] = hoverUIElemColor;

    // UI-Elemente
    style.Colors[ImGuiCol_Button] = baseUIElemColor;
    style.Colors[ImGuiCol_ButtonHovered] = hoverUIElemColor;
    style.Colors[ImGuiCol_ButtonActive] = activeUIElemColor;
    style.Colors[ImGuiCol_Header] = baseUIElemColor;
    style.Colors[ImGuiCol_HeaderHovered] = hoverUIElemColor;
    style.Colors[ImGuiCol_HeaderActive] = activeUIElemColor;
    style.Colors[ImGuiCol_FrameBg] = baseUIElemColor;
    style.Colors[ImGuiCol_FrameBgHovered] = hoverUIElemColor;
    style.Colors[ImGuiCol_FrameBgActive] = activeUIElemColor;

    // Tabs
    style.Colors[ImGuiCol_Tab] = baseUIElemColor;
    style.Colors[ImGuiCol_TabHovered] = hoverUIElemColor;
    style.Colors[ImGuiCol_TabActive] = activeUIElemColor;
    style.Colors[ImGuiCol_TabUnfocused] = baseUIElemColor;
    style.Colors[ImGuiCol_TabUnfocusedActive] = hoverUIElemColor;

    // Zugriff auf IO-Struktur
    ImGuiIO& io = ImGui::GetIO();

    // Fonts laden
    ttfJuliaMonoItalic = io.Fonts->AddFontFromFileTTF("Recc/Fonts/JuliaMono-ttf/JuliaMono-MediumItalic.ttf", 16.0f);
    ttfJuliaMonoBold = io.Fonts->AddFontFromFileTTF("Recc/Fonts/JuliaMono-ttf/JuliaMono-Bold.ttf", 16.0f);
    // ...

    io.FontDefault = ttfJuliaMonoBold;
}