#include "templateDecls.h"

// git clone https://github.com/DanielChappuis/reactphysics3d.git
// cd reactphysics3d
// mkdir build && cd build
// cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..


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