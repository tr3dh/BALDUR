// Test Env für UI
//
// Absicherung gegen Grafikkarten/Treiber spezifische Probleme
// Aufbau:
// |_ ContentTexture   : um scaling Faktor größer als Fenster >> wird runter scaliert >> pseudo AA,
// |                     >> glLineWidth(factor) setzen damit linie nicht zu dünn
// |_ UITexture        : texture mit 1:1 auflösung für UI >> MUSS unbedingt 1:1 auflösung sein damit UI überhaupt gerendert wird
// |_ Rendering        : rendering der Texturen hintereinander
// 
// Da das Rendering nun in eine Textur erfolgt und nicht mehr direkt auf den Bildschirm,
// Vorteile :
// . Postprocessing möglich (bspw. Bloom, ToneMapping, Farbkorrektur)
// . Inhalt und UI direkt als Textur verfügbar >> unkompliziertes Speichern / weiterverarbeiten
// 
// Nachteile :
// . Performance evtl. etwas schlechter
// . kein direktes Antialiasing mehr möglich (MSAA)
//
// Maßnahmen :
// . Pseudo Antialiasing :
//   . ContentTexture umd bestimmten Faktor größer als Fenster
//   . Runterskalieren beim Rendern
//   . damit linien nicht zu dünn werden >> glLineWidth(factor) vorher setzen
//   . danach über glLineWidth(1.0f) wieder zurücksetzen
//
// Ergebnis :
// . UI sieht aus wie vorher
// . Inhalt wirkt etwas weicher durch Pseudo AA
// . Weniger anfällig für Treiber/Grafikkarten spezifische Probleme (agressive Quadro Treiber)

#include "templateDecls.h"

int main()
{
    InitWindow(800, 600, "UI in RenderTexture");
    SetTargetFPS(60);
    RenderTexture2D uiTexture = LoadRenderTexture(800, 600);
    rlImGuiSetup(true); // Fonts laden

    // HWND hwnd = GetWindowHandle(); // raylib Funktion
    // SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, screenWidth, screenHeight, SWP_SHOWWINDOW);

    // 3D-Kamera initialisieren
    Camera camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
    camera.target   = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up       = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection     = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose())
    {
        // 1️⃣ UI auf RenderTexture rendern
        BeginTextureMode(uiTexture);
        ClearBackground(BLANK);

        rlImGuiBegin();

        ImGui::Begin("RenderTexture UI");
        ImGui::Text("FPS: %d", GetFPS());
        static bool saveTexture = false;
        if (ImGui::Button("Speichere PNG")) saveTexture = true;
        ImGui::End();

        rlImGuiEnd();

        EndTextureMode();

        // 2️⃣ RenderTexture auf Bildschirm zeichnen
        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode3D(camera);

        // Grid mit dicker Linien
        // rlPushMatrix();
        glLineWidth(10.0f);
        DrawGrid(10, 1.0f);
        // rlPopMatrix();

        EndMode3D();

        DrawTextureRec(
            uiTexture.texture,
            { 0, 0, (float)uiTexture.texture.width, -(float)uiTexture.texture.height },
            { 0, 0 },
            WHITE
        );

        EndDrawing();

        // 3️⃣ Bei Button-Click RenderTexture speichern
        if (saveTexture)
        {
            // 1. Image erzeugen
            Image img = GenImageColor(uiTexture.texture.width, uiTexture.texture.height, BLANK);

            // 2. Pixel auslesen
            void *pixels = rlReadTexturePixels(
                uiTexture.texture.id,
                uiTexture.texture.width,
                uiTexture.texture.height,
                RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
            );
            memcpy(img.data, pixels, uiTexture.texture.width * uiTexture.texture.height * 4); // 4 Bytes pro Pixel

            // 3. Exportieren
            ExportImage(img, "ui_render.png");
            UnloadImage(img);

            saveTexture = false;
        }
    }

    rlImGuiShutdown();
    UnloadRenderTexture(uiTexture);
    CloseWindow();
    return 0;
}