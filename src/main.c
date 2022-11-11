#include "globals.h"

int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Cemetery 3D");
    InitAudioDevice();

    initGame();

    // Main game loop
    while (!WindowShouldClose())
    {
        updateGame();
    }

    // De-Initialization
    UnloadSound(fxSlash);
    UnloadSound(fxBegin);
    UnloadSound(fxDeath);
    UnloadTexture(sword);
    UnloadModel(wallModel);
    UnloadTexture(wall);
    UnloadTexture(crosshair);
    UnloadTexture(hud);
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
