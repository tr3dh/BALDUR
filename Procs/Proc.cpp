// Main file für FEMProc
// deklariert main funktion für executable

#include "templateDecls.h"
#include "Libaries/Libaries.h"

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

const std::string g_programsConfigCache = "../bin/example.CONFIG_CACHE";
std::string g_fileBrowserCWD = fs::current_path().string() + "/../Import/";

bool g_bindStartUpToReloadRecent = false;

bool g_logRaylibMessages = false;
int g_targetFPS = 60;
bool g_vsyncEnabled = false;

bool g_loadingScript = false;
int g_pauseFrames = 10;
std::string g_scriptPath = NULLSTR;

void cacheConfigs(){

    //
    if(!fs::exists(fs::path(g_programsConfigCache).parent_path())){
        fs::create_directory(fs::path(g_programsConfigCache).parent_path());
    }

    //
    ByteSequence bs;

    bs.insertMultiple(
        string::relPath(g_fileBrowserCWD),
        g_backgroundColor,
        g_progressDisplayBackgroundColor,
        g_progressDisplayTextColor,
        g_bindStartUpToReloadRecent,
        g_logRaylibMessages,
        g_vsyncEnabled,
        g_targetFPS,
        g_pauseFrames
    );

    //
    bs.encode(g_encoderKey);
    bs.toFile(g_programsConfigCache);

    //
    LOG << "CWD : " << fs::current_path() << ENDL;
    LOG << "** Have written config cache to " << g_programsConfigCache << ENDL;
}

void loadCachedConfigs(){

    if(!fs::exists(fs::path(g_programsConfigCache))){
        return;
    }

    //
    ByteSequence bs;

    bs.fromFile(g_programsConfigCache);
    bs.decode(g_encoderKey);

    bs.extractMultipleReversed(
        g_fileBrowserCWD,
        g_backgroundColor,
        g_progressDisplayBackgroundColor,
        g_progressDisplayTextColor,
        g_bindStartUpToReloadRecent,
        g_logRaylibMessages,
        g_vsyncEnabled,
        g_targetFPS,
        g_pauseFrames
    );

    // aus relativem Pfad global gültigen generieren
    g_fileBrowserCWD = fs::current_path().string() + "/" + g_fileBrowserCWD;
}

int main(void)
{
    #ifdef USE_LLVM
    LOG << "llvm wird genutzt" << ENDL;
    #endif

    //
    // mkdir("../bin");
    // openLogFile();
    getEnv();

    //
    LOG << "-- starte Programm in ENV " << g_env << endln;

    //
    loadCachedConfigs();

    //
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int usableHeight = workArea.bottom - workArea.top;          // nutzbare Höhe (ohne Taskleiste)
    int usableWidth  = workArea.right - workArea.left;          // nutzbare Breite (in den meisten Fällen gleich der Fensterbreite)

    //
    LOG << std::fixed << std::setprecision(4) << ENDL;
    LOG << ENDL;

    //
    LOG << "** Source Code " << countLinesInDirectory("../src") << " lines" << ENDL;
    LOG << "** Procs Code " << countLinesInDirectory("../Procs") << " lines" << ENDL;
    LOG << ENDL;

    //
    SetTraceLogCallback(RaylibLogCallback);

    //
    g_logRaylibMessages ? enableRLLogging() : disableRLLogging();

    //
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_HIDDEN | FLAG_WINDOW_RESIZABLE);
    InitWindow(600,600, "\t Adaptive, Lightweight Finite Element Tool (ALF)");

    HWND hwnd = (HWND)CORE.handle;
    ASSERT(hwnd != NULL, "Windows window handle des Raylib fensters ist ungültig");

    DragAcceptFiles(hwnd, TRUE);

    // für Progressbar >> rendern wenn fenster als initialisiert vermerkt sonst return
    windowInitialized = true;                   

    //
    const char* glVersion = (const char*)glGetString(GL_VERSION);
    LOG << "** OpenGL Version: " << glVersion << ENDL;

    const char* vendor = (const char*)glGetString(GL_VENDOR);
    LOG << "** GPU Vendor: " << vendor << ENDL;

    // string splitten da im glVersion String noch Infos über die graphikkarte stehen
    g_glVersion = std::string(glVersion);

    // ab der 4.3 enthält opengl eine shader pipeline für comp shaders
    // g_ComputeShaderBackendEnabled = g_glVersion >= 4.3f;

    //
    g_vendorCorp = std::string(vendor);

    //
    g_CudaBackendEnabled = string::contains(g_vendorCorp, "NVIDIA");

    //
    LOG << "** Init in GENV " << g_env << ENDL;

    // LOG << (g_ComputeShaderBackendEnabled ? "** Computeshader Backend freigeschaltet" :
    //     "** Computeshader Backend gesperrt, opengl version " + std::to_string(g_glVersion).substr(0,3) + " ist nicht mit comp shadern kompatibel, erforderliche Version : OpenGL 4.3") << ENDL;

    // LOG << (g_CudaBackendEnabled ? "** Cuda Backend freigeschaltet" :
    //     "** Cuda Backend gesperrt, vendor " + g_vendorCorp + " ist nicht mit Cuda kompatibel") << ENDL;

    // LOG << "** -----------------------------------------" << ENDL;

    // Raylib Fenster init
    float winSizeFaktor = 0.6f;

    int monitor = GetCurrentMonitor();
    int vsyncFPS = GetMonitorRefreshRate(monitor);

    LOG << ENDL;
    LOG << "++ Monitor VSYNC FPS für Bildschirm " << monitor << " mit " << vsyncFPS << " geladen" << ENDL;
    LOG << ENDL;

    // vsync aktivieren über Übergabe von 0 als ziel fps oder ziel fps übergeben
    SetTargetFPS(g_vsyncEnabled ? vsyncFPS : g_targetFPS);

    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    int windowWidth = screenWidth * winSizeFaktor;
    int windowHeight = screenHeight * winSizeFaktor;

    bool fullScreen = false;

    SetWindowSize(windowWidth, windowHeight);
    SetWindowPosition((screenWidth - windowWidth)/2, (screenHeight - windowHeight)/2);

    LOG << "-- init auf Bildsirm mit Screensize [" << screenWidth << "|" << screenHeight << "]" << ENDL;

    Image icon = LoadImage("../Recc/Compilation/icon.png");

    // RGB zu RGBA falls alpha Kanal fehlt
    if (icon.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        LOG << "-- Konvertiere icon.png zu RGBA -> ergänze alpha channel" << ENDL;
        LOG << ENDL;
        ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }

    SetWindowIcon(icon);
    UnloadImage(icon);

    //
    LOG << "** ImGui Version " << ImGui::GetVersion() << ENDL;

    // rlImGui initialisieren (ImGui-Kontext wird angelegt)
    rlImGuiSetup(true);

    // ImPlot-Kontext anlegen (nach ImGui!)
    ImPlot::CreateContext();

    //
    SetupImGuiStyle();

    // Fenster jetzt sichtbar machen
    ClearWindowState(FLAG_WINDOW_HIDDEN);

    float imguiScale = 1.0f;
    // scaleImguiUI(imguiScale);

    //
    float time = 0;

    //
    CodeEditor editor;
    editor.openScript("../Import/exp.bld");

    Terminal terminal;

    //
    bool closeWindow = false;
    while (!closeWindow)
    {
        // deltaTime
        static float dt;
        dt = GetFrameTime();

        //
        time += dt;

        // Screen stats
        int monitor = GetCurrentMonitor();
        Vector2 monitorSize = {GetMonitorWidth(monitor), GetMonitorHeight(monitor)};
        Vector2 winSize = {GetScreenWidth(), GetScreenHeight()};

        // Nur bei gedrückter Strg Taste reagieren
        bool currentCtrlState = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        if(currentCtrlState) {

            float wheelMove = GetMouseWheelMove();
            
            if(wheelMove != 0) {

                // Skalierungsfaktor anpassen, clamp zwischen 0.5 und 3, sodass UI nicht vollkommen aus
                // dem Bereich einer sinnvollen Skalierung entweicht
                imguiScale += wheelMove * 0.1f;
                imguiScale = Clamp(imguiScale, 0.5f, 3.0f);
                
                scaleImguiUI(imguiScale);
            }
        }

        if(WindowShouldClose()){

            // gibt true zurück wenn Schließen Icon des Fenster gedrückt wird
            // kann bei Bedarf überschrieben werden, sodass das Fenster nicht mehr über den Klick auf den Icon Button von OS
            // geschlossen werden kann
            //
            // Da progressbar ebenfalls ohne event verarbeitung ins Fenster schreibt bewirkt der Klick auf des Kreuz in der 
            // Fensterecke dort nichts
            //
            closeWindow = true;
        }

        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_ESCAPE)){

            // cleanUp
            closeWindow = true;
        }

        if(IsKeyPressed(KEY_F10)){

            LOG << "++ toggleScreen" << ENDL;

            if(!fullScreen){
                SetWindowSize(screenWidth, screenHeight);
                SetWindowPosition(0,0);
            } else {
                // resize auf originalGröße
                SetWindowSize(windowWidth, windowHeight);
                SetWindowPosition((screenWidth - windowWidth)/2, (screenHeight - windowHeight)/2);
            }

            fullScreen = !fullScreen;
        }

        if(IsKeyPressed(KEY_F11)){

            LOG << "++ toggleScreen" << ENDL;

            if(!fullScreen){
                SetWindowSize(usableWidth, usableHeight);
                SetWindowPosition(0,0);
            } else {
                // resize auf originalGröße
                SetWindowSize(windowWidth, windowHeight);
                SetWindowPosition((screenWidth - windowWidth)/2, (screenHeight - windowHeight)/2);
            }

            fullScreen = !fullScreen;
        }

        // Toggle Cursor capture
        static bool hideCursor = false;
        // if (IsKeyPressed(KEY_C) && IsKeyDown(KEY_LEFT_CONTROL))
        // {
        //     if (IsCursorHidden())
        //     {
        //         ShowCursor();
        //         EnableCursor();
        //     }
        //     else
        //     {
        //         HideCursor();
        //         DisableCursor();
        //     }
        //     hideCursor = !hideCursor;
        // }

        if(IsKeyPressed(KEY_F5)){

            editor.save();
            std::string cmd = ".\\ProcLang_d execute " + editor.cScriptPath;
            terminal.m_shell.sendCommand(cmd);
        }

        if (ImGui::GetIO().WantCaptureMouse && !hideCursor){
            
        }

        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            for (int i = 0; i < (int)droppedFiles.count; i++)
            {
                LOG << droppedFiles.paths[i] << endln;
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        BeginDrawing();

        //
        ClearBackground(g_backgroundColor); //Color(30,30,30,255));

        //
        rlImGuiBegin();

        //
        auto scriptFileDialog = [&](){

            // Sicherstellen, dass der Dialog nicht in dem kurzen Zeitfenster der Verzögerung
            // zwischen Callback und synchronem Ladevorgang wieder geäffnet werden kann
            // ~10 Frames bzw. je nach Einstellung des Nutzers
            if(g_fileDialog || g_loadingScript){
                return;
            }

            OpenFileDialog("Open Script", { ".bld" }, false, false, g_fileBrowserCWD, [&](const std::string& chosenFilePath) {

            if(!fs::is_regular_file(chosenFilePath)){
                return;
            }

            g_loadingScript = true;
            g_scriptPath = chosenFilePath;

            g_fileBrowserCWD = fs::path(chosenFilePath).parent_path().string();
            });
        };

        //
        auto crScriptFileDialog = [&](){

            // Sicherstellen, dass der Dialog nicht in dem kurzen Zeitfenster der Verzögerung
            // zwischen Callback und synchronem Ladevorgang wieder geäffnet werden kann
            // ~10 Frames bzw. je nach Einstellung des Nutzers
            if(g_fileDialog || g_loadingScript){
                return;
            }

            OpenFileDialog("create Script", { ".bld" }, true, false, g_fileBrowserCWD, [&](const std::string& chosenFilePath) {

            g_loadingScript = true;
            g_scriptPath = chosenFilePath;

            g_fileBrowserCWD = fs::path(chosenFilePath).parent_path().string();
            });
        };

        // ShortCuts
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) {
            scriptFileDialog();
        }

        static bool recordingStripped = false;
        static bool recordingFull = false;

        const static std::string mp4Cache = "../bin/mp4Cache";
        static std::vector<Image> pngs = {};

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if(ImGui::BeginMenu("New")){
                    
                    if(ImGui::MenuItem("Script (Ctrl + O)")){

                        crScriptFileDialog();
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Open"))
                {
                    if(ImGui::MenuItem("Script (Ctrl + O)")){

                        scriptFileDialog();
                    }

                    ImGui::EndMenu();
                }

                if(ImGui::BeginMenu("Export")){

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings"))
            {
                if(ImGui::BeginMenu("Startup")){

                    //
                    ImGui::Checkbox("Reload Recent on Startup", &g_bindStartUpToReloadRecent);
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Logging")){

                    //
                    if(ImGui::Checkbox("Display Raylib Logs", &g_logRaylibMessages)){
                        g_logRaylibMessages ? enableRLLogging() : disableRLLogging();
                    }

                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Filebrowser")){

                    InputSliderInt("Framedelay after Browsercallback", g_pauseFrames, 0, 25);

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("General Appearance"))
                {

                    RaylibColorEdit(g_backgroundColor,                  "Background");
                    RaylibColorEdit(g_progressDisplayBackgroundColor,   "Progressbar Background");
                    RaylibColorEdit(g_progressDisplayTextColor,         "Progressbar Text");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Monitor"))
                {
                    //
                    ImGui::Text("FPS : %.2f", 1/dt);

                    //
                    if(ImGui::Checkbox("VSYNC", &g_vsyncEnabled)){

                        SetTargetFPS(g_vsyncEnabled ? vsyncFPS : g_targetFPS);
                    }

                    //
                    if(InputSliderInt("Target FPS", g_targetFPS, 30, 320, true)){
                        
                        SetTargetFPS(g_vsyncEnabled ? vsyncFPS : g_targetFPS);
                    }

                    //
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Hardware"))
            {
                ImGui::Text("OpenGL Version : %s", glVersion);
                ImGui::Text("GPU Vendor : %s", vendor);
                ImGui::Text("Env : %s", g_env.c_str());

                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Temp")){

                if(ImGui::MenuItem("Clear Program Bin")){
                    clearBin();
                }

                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Dependencies")){

                if(ImGui::MenuItem("Install/Update")){

                    std::string command = "cmd.exe /C \"..\\Batch\\installDependenciesAsUser.bat\"";
                    streamWinCommand(command, [](const char* input){
                        LOG << input;
                        displayProgress(0.5, std::strlen(input) > 24 ? "Führe batchskript aus" : input);
                    });
                }

                if(ImGui::MenuItem("Install/Update as Admin")){

                    std::string command = "cmd.exe /C \"..\\Batch\\installDependenciesAsAdmin.bat\"";
                    streamWinCommand(command, [](const char* input){
                        LOG << input;
                        displayProgress(0.5, std::strlen(input) > 24 ? "Führe batchskript aus" : input);
                    });
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Libraries"))
            {
                if (ImGui::BeginTable("Overview", 3)) {

                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("License");
                    ImGui::TableSetupColumn("Link");
                    ImGui::TableHeadersRow();

                    for (int i = 0; i < libaryNames.size(); ++i) {

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", libaryNames[i]);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", libaryLicenses[i]);

                        ImGui::TableSetColumnIndex(2);

                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 102, 204, 255));
                        if (ImGui::SmallButton(libaryUrls[i])) {
                            OpenLink(libaryUrls[i]);
                        }
                        ImGui::PopStyleColor();
                    }

                    ImGui::EndTable();
                }

                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Github")){

                if (ImGui::BeginTable("ProjectLinks", 2)) {

                    ImGui::TableSetupColumn("Subject");
                    ImGui::TableSetupColumn("Link");
                    ImGui::TableHeadersRow();

                    auto LinkRow = [](const char* label, const char* url) {

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(label);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 102, 204, 255)); // Linkfarbe
                        if (ImGui::SmallButton(url)) {
                            OpenLink(url);
                        }
                        ImGui::PopStyleColor();
                    };

                    LinkRow("GitHub Repository", githubRepositoryUrl.c_str());
                    LinkRow("Releases", (githubRepositoryUrl + "/releases").c_str());
                    LinkRow("Find out more", (githubRepositoryUrl+ "/blob/main/README.md").c_str());
                    LinkRow("Find out even more (read Thesis)", (githubRepositoryUrl+"/blob/main/Recc/Thesis/Studienarbeit.pdf").c_str());

                    ImGui::EndTable();
                }
                ImGui::EndMenu();
            }

            float buttonWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2.0f;
            float rightEdge = ImGui::GetWindowWidth() - buttonWidth - ImGui::GetStyle().FramePadding.x;
        
            ImGui::SetCursorPosX(rightEdge);
            if (ImGui::Button("X")) {
                closeWindow = true;
            }

            ImGui::EndMainMenuBar();
        }

        ImVec2 cursorPos = ImGui::GetCursorPos();

        static float fileManagerWidth = 0.0f;

        if(fileManagerWidth > 0){
            // Tabbar über Editor
            ImGui::SetNextWindowPos({cursorPos.x - ImGui::GetStyle().WindowPadding.x, cursorPos.y - ImGui::GetStyle().WindowPadding.y});
            ImGui::SetNextWindowSize(ImVec2(fileManagerWidth, winSize.y - cursorPos.y + ImGui::GetStyle().WindowPadding.y));
            ImGui::Begin("##filemanager", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoScrollbar);

            ImGui::End();
        }

        cursorPos.x += fileManagerWidth;

        static float terminalHeight = 250.0f;
        float tabBarHeight = ImGui::GetTextLineHeightWithSpacing() * 2.2f;

        // // Tabbar über Editor
        // ImGui::SetNextWindowPos(cursorPos);
        // ImGui::SetNextWindowSize(ImVec2(winSize.x - fileManagerWidth, tabBarHeight));
        // ImGui::Begin("##editor_tabbar", nullptr,
        //     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        //     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
        //     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

        // if (ImGui::BeginTabBar("##editor_tabs",
        //     ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
        // {
        //     if (ImGui::BeginTabItem("main edit")) { ImGui::EndTabItem(); }
        //     ImGui::EndTabBar();
        // }
        // ImGui::End();

        static float splitterHeight = 5;
        ImVec2 editorPos = {fileManagerWidth, cursorPos.y - ImGui::GetStyle().WindowPadding.y};
        ImVec2 editorSize =
            {winSize.x - fileManagerWidth, winSize.y - cursorPos.y - terminalHeight + ImGui::GetStyle().WindowPadding.y - splitterHeight};

        // editor
        editor.render(editorPos, editorSize);

        // Splitter-Leiste zeichnen
        
        ImGui::SetNextWindowPos(ImVec2(0, GetScreenHeight() - terminalHeight - 2 * splitterHeight), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(GetScreenWidth(), 4 * splitterHeight), ImGuiCond_Always);

        ImGui::Begin("Splitter", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground
        );

        // Unsichtbares Drag-Element, um die Höhe zu ändern
        ImGui::InvisibleButton("##SplitterGrab", ImVec2(winSize.x, 8));

        static bool IsDraggingSplitter = false;
        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !IsDraggingSplitter){
            
            IsDraggingSplitter = true;
        }

        if(IsDraggingSplitter && ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            
            IsDraggingSplitter = false;
        }

        if(IsDraggingSplitter){

            terminalHeight -= ImGui::GetIO().MouseDelta.y;
            std::clamp(terminalHeight, 20.0f, 50.0f);
        }

        // 

        ImGui::End();

        // Tabbar über Terminal
        ImVec2 terminalPos = {fileManagerWidth, winSize.y - terminalHeight + splitterHeight};
        ImGui::SetNextWindowPos(terminalPos);
        ImGui::SetNextWindowSize(ImVec2(winSize.x - fileManagerWidth, tabBarHeight));
        ImGui::Begin("##terminal_tabbar", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

        if (ImGui::BeginTabBar("##terminal_tabs",
            ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
        {
            char buf[128];
            std::snprintf(buf, sizeof(buf), "Shell : %s", terminal.shellStartUpCommand.c_str());

            if (ImGui::BeginTabItem(buf)){ ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::End();

        // Terminal
        terminal.render(terminalPos + ImVec2({0, tabBarHeight - ImGui::GetStyle().WindowPadding.y}),
                        {winSize.x - fileManagerWidth, terminalHeight + ImGui::GetStyle().WindowPadding.y - tabBarHeight});

        RenderFileDialog();

        rlImGuiEnd();

        EndDrawing();

        //
        if (IsKeyPressed(KEY_F2)) {
        
            //
            mkdir("../bin");

            // RenderTexture/Screen in Image konvertieren
            Image img = LoadImageFromScreen();
            // ImageFlipVertical(&img);

            ExportImage(img, ("../bin/fullScreenshot_" + getTimestamp() + ".png").c_str());
            UnloadImage(img);
        }

        // static bool recordingStripped = false;
        // static bool recordingFull = false;

        // const static std::string mp4Cache = "../bin/mp4Cache";
        // static std::vector<Image> pngs = {};

        // Start Record
        if(IsKeyPressed(KEY_F4) && !recordingStripped && !recordingFull){

            //
            mkdir("../bin");

            //
            recordingFull = true;

            for(auto& png : pngs){
                UnloadImage(png);
            }

            pngs.clear();

            // Prüfen, ob das Verzeichnis existiert
            if (fs::exists(mp4Cache)) {
                
                // Löscht das Verzeichnis
                fs::remove_all(mp4Cache);
            }

            // Verzeichnis neu erstellen
            if (fs::create_directory(mp4Cache)) {
                
            } else {
                ASSERT(TRIGGER_ASSERT, "mkdir fehlgeschlagen");
            }
        }
        // end record
        else if(IsKeyPressed(KEY_F4) && !recordingStripped && recordingFull){

            mkdir("../bin");

            recordingFull = false;

            for(size_t pngCounter = 0; pngCounter < pngs.size(); pngCounter++){
                
                char filename[256];
                snprintf(filename, sizeof(filename), "%s/frame%04d.png", mp4Cache.c_str(), pngCounter);

                char progressLine[256];
                snprintf(progressLine, sizeof(progressLine), "Exportiere Video, Frame [%d, %d]", pngCounter, pngs.size());

                displayProgress((float)pngCounter/pngs.size(), progressLine);
                ExportImage(pngs[pngCounter], filename);
            }

            char cmd[256];
            snprintf(cmd, sizeof(cmd),
                "ffmpeg -framerate %f -i ../bin/%s/%s.png -c:v libx264 -pix_fmt yuv420p ../bin/fullScreenRecord_%s.mp4",
                1/dt, mp4Cache.c_str(), "frame%04d", getTimestamp().c_str());

            runWinCommand("cmd.exe /C \"" + std::string(cmd) + "\"");

            for(auto& png : pngs){
                UnloadImage(png);
            }

            pngs.clear();
        }
        else if(!recordingStripped && recordingFull){

            // RenderTexture/Screen in Image konvertieren
            pngs.emplace_back(LoadImageFromScreen());
        }

        HandleFileDialog();

        // Leider eine sehr unschöne Lösung
        // Da der Browser Zustände in ImGui speichert, kollidieren Öffnungen in zu dicht hintereinander liegenden frames
        // anscheinend, der Browser wurde noch nicht als gecleart vermerkt, die Reccourcen noch nicht freigegeben und das CWD
        // des filebrowsers wird als auswahl pfad zurückgegeben. Deshalb wird hier provisorisch um n Frames verzögert,
        // damit der Filebrowser Zeit hat sauber geschlossen zu werden, bevor die synchrone Ladeoperation den nächsten ImGui Frame
        // verzögert. Es hat nicht gereicht einen oder zwei frames zu verzögern und mit 5 bekommt man teilweise immer noch nen
        // CWD Error. Der Wert lässt sich allerdings auch über einen Schieber einstellen.
        // Die Verzögerung der Ladeoperation hat nur zur Folge, dass es eine gewisse Verzögerung gibt
        // bevor das im Filebrowser ausgewählte Modell durchgerechnet wird
        // eine langfristige Alternative wäre die Benutzung des ImGuiFileBrowsers von aiekick
        static int frameCounter = g_pauseFrames;

        // Ladebool einsetzen
        if(g_loadingScript){

            //
            frameCounter--;

            if(frameCounter == 0){

                // synchroner Ladevorgang
                LOG << "loading Skript " << g_scriptPath << ENDL;

                if(!fs::exists(g_scriptPath)){

                    if(!string::endsWith(g_scriptPath, ".bld")){
                        g_scriptPath += ".bld";
                    }

                    std::ofstream ofs(g_scriptPath);
                    ofs << "// ..."; 
                    ofs.close();
                }

                editor.openScript(g_scriptPath);

                //
                g_loadingScript = false;
                g_scriptPath = NULLSTR;

                // frameCounter zurücksetzen
                frameCounter = g_pauseFrames;
            }
        }
    }

    //
    LOG << "-- Fenster geschlossen" << endln;

    //
    ImPlot::DestroyContext();
    rlImGuiShutdown();

    //
    disableRLLogging();

    //
    CloseWindow();
    cacheConfigs();
    closeLogFile();

    //
    // ConPTY Pseudo Console führt dazu, dass msys terminal nicht mehr die asci zeichen erhält
    // sondern die ansi codes der tasten

    if(g_env == "sh.exe" && PseudoTerminal::constructedMemberThisExecution){ // && PseudoTerminal::hasConstructedMembersThisExecution){

        LOG << "-- Resette Msys Terminal" << endln;
        std::system("reset");
    }

    return 0;
}