#include "raylib.h"
#include <math.h>

// Authentic NERV Hex Codes
#define NERV_AMBER     (Color){ 255, 140, 0, 255 }
#define NERV_DARK_RED  (Color){ 140, 0, 0, 255 }
#define NERV_GRID_DARK (Color){ 40, 20, 0, 100 }

void DrawNervGrid(int width, int height, float spacing) {
    for (int i = 0; i <= width; i += (int)spacing) {
        DrawLine(i, 0, i, height, NERV_GRID_DARK);
    }
    for (int i = 0; i <= height; i += (int)spacing) {
        DrawLine(0, i, width, i, NERV_GRID_DARK);
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Set high-quality anti-aliasing for that "vector" look
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "NERV | SIGNAL MONITOR");

    float amplitude = 60.0f;
    float frequency = 2.0f;
    float scrollSpeed = 5.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float time = (float)GetTime();

        BeginDrawing();
        ClearBackground((Color){ 10, 5, 0, 255 }); // Deep off-black

        // 1. Background Grid
        DrawNervGrid(screenWidth, screenHeight, 30.0f);

        // 2. Reference Lines
        DrawLine(0, screenHeight/2, screenWidth, screenHeight/2, NERV_GRID_DARK);

        // 3. The "Signal" Sine Wave
        for (int x = 0; x < screenWidth; x++) {
            // Calculate two sine waves for a "unstable" organic look
            float y = screenHeight/2 + 
                      sinf(x * 0.02f + time * scrollSpeed) * amplitude +
                      sinf(x * 0.05f + time * 10.0f) * 5.0f; // High freq noise
            
            DrawPixel(x, (int)y, NERV_AMBER);
            // Add a slight vertical glow
            DrawPixel(x, (int)y + 1, (Color){ 255, 140, 0, 100 });
        }

        // 4. UI Overlay
        DrawRectangle(15, 15, 280, 35, NERV_DARK_RED);
        DrawText("SIGNAL MAGI: MELCHIOR", 25, 23, 18, BLACK);
        
        // Animated Status Text
        if ((int)(time * 2) % 2 == 0) {
            DrawText("PATTERN BLUE", 20, 60, 20, NERV_AMBER);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}