#include "raylib.h"
#include "stdlib.h"
#include <cmath>
#include <cstdlib>

//
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main() {

    //
    putenv("GLFW_USE_HYBRID_HPG=1");

    // NVIDIA OpenGL Treiber Flags
    putenv(" __GL_THREADED_OPTIMIZATIONS=0");           // Multithreading optimierungen AUS >> kann Flackern reduzieren
    putenv(" __GL_SYNC_TO_VBLANK=1");                   // VSync AN >> Tearing verhindern
    putenv(" __GL_SHADER_DISK_CACHE=1");                // Shader Caching AN >> verbessert Performance
    putenv(" __GL_LOG_ERRORS=1");                       // OpenGL Fehler Logging AN >> debugging
    putenv(" __GL_GPU_DISALLOW_SYNC_OBJECTS=1");        // Sync Objekte deaktivieren >> kann Wackeln fixen

    //
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(800, 600, "TestUmgebung");

    //
    Camera3D camera = { 0 };
    camera.position = { 4.0f, 3.0f, 4.0f };
    camera.target = { 0.0f, 0.5f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float vertices[] = {
        // Rückseite (-Z), Farbe: Rot
        -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f, -0.5f,
        // Vorderseite (+Z), Farbe: Grün
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        // Rechts (+X), Farbe: Blau
         0.5f, -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
        // Links (-X), Farbe: Gelb
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
        // Oben (+Y), Farbe: Orange
        -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
        // Unten (-Y), Farbe: Lila
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f
    };

    unsigned char colors[] = {
        // Rückseite Rot
        255,0,0,255, 255,0,0,255, 255,0,0,255,
        255,0,0,255, 255,0,0,255, 255,0,0,255,
        // Vorderseite Grün
        0,255,0,255, 0,255,0,255, 0,255,0,255,
        0,255,0,255, 0,255,0,255, 0,255,0,255,
        // Rechts Blau
        0,0,255,255, 0,0,255,255, 0,0,255,255,
        0,0,255,255, 0,0,255,255, 0,0,255,255,
        // Links Gelb
        255,255,0,255, 255,255,0,255, 255,255,0,255,
        255,255,0,255, 255,255,0,255, 255,255,0,255,
        // Oben Orange
        255,165,0,255, 255,165,0,255, 255,165,0,255,
        255,165,0,255, 255,165,0,255, 255,165,0,255,
        // Unten Lila
        128,0,128,255, 128,0,128,255, 128,0,128,255,
        128,0,128,255, 128,0,128,255, 128,0,128,255
    };

    float normals[] = {
        // Rückseite (-Z)
        0,0,-1, 0,0,-1, 0,0,-1,
        0,0,-1, 0,0,-1, 0,0,-1,
        // Vorderseite (+Z)
        0,0,1, 0,0,1, 0,0,1,
        0,0,1, 0,0,1, 0,0,1,
        // Rechts (+X)
        1,0,0, 1,0,0, 1,0,0,
        1,0,0, 1,0,0, 1,0,0,
        // Links (-X)
        -1,0,0, -1,0,0, -1,0,0,
        -1,0,0, -1,0,0, -1,0,0,
        // Oben (+Y)
        0,1,0, 0,1,0, 0,1,0,
        0,1,0, 0,1,0, 0,1,0,
        // Unten (-Y)
        0,-1,0, 0,-1,0, 0,-1,0,
        0,-1,0, 0,-1,0, 0,-1,0
    };

    Mesh cubeMesh = {0};
    cubeMesh.vertexCount = 36;
    cubeMesh.vertices = vertices;
    cubeMesh.colors = colors;
    cubeMesh.normals = normals;

    UploadMesh(&cubeMesh, false);

    Model cubeModel = LoadModelFromMesh(cubeMesh);

    float angle = 0.0f;

    while (!WindowShouldClose()) {

        angle += 0.01f;

        // Kamera auf kreisbahn mit Radius 3 um (0,2,0)
        camera.position.x = 3.0f * sinf(angle);
        camera.position.z = 3.0f * cosf(angle);
        camera.position.y = 2.0f;
        camera.target = {0.0f, 0.0f, 0.0f};

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(cubeModel, (Vector3){0,0,0}, 1.0f, WHITE);
        DrawGrid(10, 1.0f);

        EndMode3D();

        DrawText("Test Umgebung", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadModel(cubeModel);
    CloseWindow();

    return 0;
}