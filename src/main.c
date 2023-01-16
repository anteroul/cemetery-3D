#include "globals.h"

#define BUILD_DIR_IN_ROOT 0
#define BUILD_DIR_CLION 1
#define BUILD_DIR_XCODE 2
#define BUILD_DIR_VS 3


int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Cemetery 3D");
    InitAudioDevice();

    initGame(BUILD_DIR_VS);

    // Main game loop
    while (!WindowShouldClose())
    {
        updateGame();
    }

    // De-Initialization
    UnloadSound(fxSlash);
    UnloadSound(fxBegin);
    UnloadSound(fxDeath);
    CloseAudioDevice();
    UnloadTexture(sword);
    UnloadTexture(crosshair);
    UnloadTexture(hud);
    UnloadTexture(wall);
    UnloadModel(wallModel);
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
