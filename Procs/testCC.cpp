#include "raylib.h"
 
int main(void)
{
    const int screenWidth  = 800;
    const int screenHeight = 450;
 
    // Raylib nutzt intern ANGLE (OpenGL ES 2.0) als Backend
    InitWindow(screenWidth, screenHeight, "Hello Raylib + ANGLE");
    SetTargetFPS(60);
 
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Hello, ANGLE!",
                     screenWidth  / 2 - MeasureText("Hello, ANGLE!", 40) / 2,
                     screenHeight / 2 - 20,
                     40, DARKBLUE);
            DrawFPS(10, 10);
        EndDrawing();
    }
 
    CloseWindow();
    return 0;
}