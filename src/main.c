/*
Author: Uljas Antero Lindell
Version: 1.5
*/

#include "globals.h"
#include "stdlib.h"


int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Cemetery 3D");
    InitAudioDevice();
    initGame();
    // Main game loop
    while (!WindowShouldClose())                // Detect window close button or ESC key
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


void initGame(void) {

    fxSlash = LoadSound("resources/slash.wav");
    fxBegin = LoadSound("resources/begin.wav");
    fxDeath = LoadSound("resources/death.wav");
    music = LoadMusicStream("resources/music.mp3");

    hp = 100;
    energy = 100;
    score = 0;
    level = 1;
    currentFrame = 0;
    framesCounter = 0;
    attackCounter = 0;
    attacking = false;
    gameOver = false;
    Enemy arr_enemy[ENEMIES];

    for (int i = 0; i < ENEMIES; i++) {
        arr_enemy[i].enemyStartPos.x = rand() % 32 - 16;
        arr_enemy[i].enemyStartPos.y = -1.0f;
        arr_enemy[i].enemyStartPos.z = rand() % 32 - 16;
        arr_enemy[i].enemyBoxPos.x = arr_enemy[i].enemyStartPos.x;
        arr_enemy[i].enemyBoxPos.y = arr_enemy[i].enemyStartPos.y;
        arr_enemy[i].enemyBoxPos.z = arr_enemy[i].enemyStartPos.z;
        arr_enemy[i].active = true;
    }

    Image hudImage = LoadImage("resources/hud.png");
    hud = LoadTextureFromImage(hudImage);
    UnloadImage(hudImage);
    Image wallImage = LoadImage("resources/wall.png");
    wall = LoadTextureFromImage(wallImage);
    UnloadImage(wallImage);
    sword = LoadTexture("resources/sword.png");
    Image image = LoadImage("resources/crosshair.png");
    crosshair = LoadTextureFromImage(image);
    UnloadImage(image);
    wallModel = LoadModelFromMesh(GenMeshCube(12.0f, 5.0f, 0.0f));
    wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wall;
    swordPosition = (Vector2) {340.0f, 120.0f};
    frameRec.x = 0.0f;
    frameRec.y = 0.0f;
    frameRec.width = (float) sword.width / 4;
    frameRec.height = (float) sword.height / 2;

    // Define camera:
    camera.position = (Vector3) {4.0f, 2.0f, 4.0f};
    camera.target = (Vector3) {0.0f, 1.8f, 0.0f};
    camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    deathSound = false;
    PlaySound(fxBegin);
    PlayMusicStream(music);
}


void resetGame(void) {
    hp = 100;
    energy = 100;
    score = 0;
    level = 1;
    currentFrame = 0;
    framesCounter = 0;
    attackCounter = 0;
    attacking = false;
    gameOver = false;
    camera.position = (Vector3) {4.0f, 2.0f, 4.0f};
    deathSound = false;
    PlaySound(fxBegin);
    PlayMusicStream(music);
    updateGame();

    for (int i = 0; i < ENEMIES; i++) {
        arr_enemy[i].enemyBoxPos.x = rand() % 32 - 16;
        arr_enemy[i].enemyBoxPos.y = -1.0f;
        arr_enemy[i].enemyBoxPos.z = rand() % 32 - 16;
    }
}


void updateGame(void) {
    UpdateMusicStream(music);
    // Update
    drawGame();
    Vector3 oldCamPos = camera.position;
    UpdateCamera(&camera);                  // Update camera

    if (!gameOver) {
        //Running logic

        if (IsKeyDown(32) == true) {
            if (energy > 0) {
                UpdateCamera(&camera);
                if (IsKeyDown(65) || IsKeyDown(83) || IsKeyDown(68) || IsKeyDown(87))
                    energy--;
                else if (energy < 100)
                    energy++;
            }
        } else if (energy < 100) {
            energy++;
        }

        // Sword moves with camera

        swordPosition.y = 50.0f * camera.target.y;
        swordPosition.x = 340.0f;

        if (camera.target.y <= 2.5f)
            swordPosition.y += 30.0f;
        if (camera.target.y <= 1.5f)
            swordPosition.y += 25.0f;

        framesCounter++;

        // Bounding boxes

        // Attack Range
        struct BoundingBox range = {
                camera.target.x - 0.5f, camera.target.y - 1.0f, camera.target.z - 0.5f,
                camera.target.x + 0.5f, camera.target.y + 1.5f, camera.target.z + 0.5f
        };

        // Player Bounding Box
        struct BoundingBox player = {
                camera.position.x - 1.0f, camera.position.y - 1.5f, camera.position.z - 1.0f,
                camera.position.x + 1.0f, camera.position.y + 0.5f, camera.position.z + 1.0f
        };

        // Enemy logic

        for (int i = 0; i < ENEMIES; i++) {
            if (arr_enemy[i].active == true) {
                if (arr_enemy[i].enemyBoxPos.y >= 1.0f) {
                    // Follow player
                    if (camera.position.x > arr_enemy[i].enemyBoxPos.x - 3.0f)
                        arr_enemy[i].enemyBoxPos.x += 0.04f;
                    if (camera.position.x < arr_enemy[i].enemyBoxPos.x + 3.0f)
                        arr_enemy[i].enemyBoxPos.x -= 0.04f;
                    if (camera.position.z > arr_enemy[i].enemyBoxPos.z - 3.0f)
                        arr_enemy[i].enemyBoxPos.z += 0.04f;
                    if (camera.position.z < arr_enemy[i].enemyBoxPos.z + 3.0f)
                        arr_enemy[i].enemyBoxPos.z -= 0.04f;

                    // Define Bounding Box for enemy

                    struct BoundingBox cBounds = {
                            arr_enemy[i].enemyBoxPos.x - 2.5f, 0.0f, arr_enemy[i].enemyBoxPos.z - 2.5f,
                            arr_enemy[i].enemyBoxPos.x + 2.5f, 2.5f, arr_enemy[i].enemyBoxPos.z + 2.5f
                    };

                    arr_enemy[i].enemyBounds = cBounds;

                } else if (arr_enemy[i].enemyBoxPos.y < 1.0f) {
                    arr_enemy[i].enemyBoxPos.y += 0.01f;
                }

                // Attack player logic

                if (CheckCollisionBoxes(player, arr_enemy[i].enemyBounds) && arr_enemy[i].enemyBoxPos.y >= 1.0f) {
                    if (((framesCounter / 30) % 2) == 1) {
                        hp -= rand() % level + 1;
                        framesCounter = 0;
                    }
                }

                // Attack enemy logic

                if (attacking) {
                    if (CheckCollisionBoxes(range, arr_enemy[i].enemyBounds)) {
                        score += 100;
                        level++;
                        arr_enemy[i].active = false;
                    }
                }

            }

                // Respawn

            else {
                arr_enemy[i].enemyBoxPos.x = rand() % 32 - 16;
                arr_enemy[i].enemyBoxPos.y = -1.5f;
                arr_enemy[i].enemyBoxPos.z = rand() % 32 - 16;

                struct BoundingBox newBounds =
                        {
                                arr_enemy[i].enemyBoxPos.x - 2.5f,
                                0.0f,
                                arr_enemy[i].enemyBoxPos.z - 2.5f,
                                arr_enemy[i].enemyBoxPos.x + 2.5f,
                                2.5f,
                                arr_enemy[i].enemyBoxPos.z + 2.5f
                        };

                arr_enemy[i].enemyBounds = newBounds;
                arr_enemy[i].active = true;
            }
        }

        // Wall logic

        if (camera.position.x <= -15.0f || camera.position.x >= 15.0f)
            camera.position = oldCamPos;

        if (camera.position.z <= -15.0f || camera.position.z >= 15.0f)
            camera.position = oldCamPos;

        // Attack logic

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !attacking) {
            if (energy >= 30) {
                PlaySound(fxSlash);
                attacking = true;
                energy -= 30;
            }
        }
        if (attacking) {
            frameRec.y = 400.0f;
            frameRec.x = 400.0f * currentFrame;
            swordPosition.x = 240.0f;
            swordPosition.y = 145.0f;
            attackCounter++;
            if (attackCounter > 2) {
                currentFrame++;
                if (currentFrame >= NUM_FRAMES_PER_LINE) {
                    currentFrame = 0;
                    attacking = false;
                }
                attackCounter = 0;
            }
        } else {
            frameRec.y = 0.0f;
            frameRec.x = 0.0f;
        }

        // Game Over logic

        if (hp <= 0) {
            StopMusicStream(music);
            gameOver = true;
        }
    }
}


void drawGame(void) {
    BeginDrawing();

    ClearBackground(BLACK);

    if (!gameOver) {
        BeginMode3D(camera);

        DrawPlane((Vector3) {0.0f, 0.0f, 0.0f}, (Vector2) {33.5f, 33.5f}, BROWN); // Draw ground

        // WALL 1
        DrawModel(wallModel, (Vector3) {12.0f, 2.5f, 16.0f}, 1.0f, WHITE);
        DrawModel(wallModel, (Vector3) {0.0f, 2.5f, 16.0f}, 1.0f, WHITE);
        DrawModel(wallModel, (Vector3) {-12.0f, 2.5f, 16.0f}, 1.0f, WHITE);
        // WALL 2
        DrawModel(wallModel, (Vector3) {12.0f, 2.5f, -16.0f}, 1.0f, WHITE);
        DrawModel(wallModel, (Vector3) {0.0f, 2.5f, -16.0f}, 1.0f, WHITE);
        DrawModel(wallModel, (Vector3) {-12.0f, 2.5f, -16.0f}, 1.0f, WHITE);
        // WALL 3
        DrawModelEx(wallModel, (Vector3) {16.5f, 2.5f, -10.0f}, (Vector3) {0.0f, 1.0f, 0.0f}, 90.0f,
                    (Vector3) {1.0f, 1.0f, 1.0f}, WHITE);
        DrawModelEx(wallModel, (Vector3) {16.5f, 2.5f, 2.0f}, (Vector3) {0.0f, 1.0f, 0.0f}, 90.0f,
                    (Vector3) {1.0f, 1.0f, 1.0f}, WHITE);
        DrawModelEx(wallModel, (Vector3) {16.5f, 2.5f, 14.0f}, (Vector3) {0.0f, 1.0f, 0.0f}, 90.0f,
                    (Vector3) {1.0f, 1.0f, 1.0f}, WHITE);
        // WALL 4
        DrawModelEx(wallModel, (Vector3) {-16.5f, 2.5f, 10.0f}, (Vector3) {0.0f, 1.0f, 0.0f}, 90.0f,
                    (Vector3) {1.0f, 1.0f, 1.0f}, WHITE);
        DrawModelEx(wallModel, (Vector3) {-16.5f, 2.5f, -2.0f}, (Vector3) {0.0f, 1.0f, 0.0f}, 90.0f,
                    (Vector3) {1.0f, 1.0f, 1.0f}, WHITE);
        DrawModelEx(wallModel, (Vector3) {-16.5f, 2.5f, -14.0f}, (Vector3) {0.0f, 1.0f, 0.0f}, 90.0f,
                    (Vector3) {1.0f, 1.0f, 1.0f}, WHITE);

        for (int i = 0; i < ENEMIES; i++)
            DrawCube(arr_enemy[i].enemyBoxPos, 2.0f, 2.0f, 2.0f, DARKGREEN);

        EndMode3D();
        DrawTextureRec(sword, frameRec, swordPosition, WHITE);
        DrawText(TextFormat("%04i", score), 20, 380, 40, GREEN);
        DrawText(TextFormat("%03i", hp), 720, 380, 40, RED);
        DrawText(TextFormat("%03i", energy), 580, 380, 40, BLUE);
        DrawTexture(crosshair, 390, 215, WHITE);
        DrawTexture(hud, 0, 0, WHITE);

    } else {
        if (IsKeyDown(32))
            resetGame();

        if (!deathSound) {
            PlaySound(fxDeath);
            deathSound = true;
        }

        DrawText("YOU ARE DEAD!", GetScreenWidth() / 2 - MeasureText("YOU ARE DEAD!", 20) / 2,
                 GetScreenHeight() / 2 - 50, 20, RED);
        DrawText("PRESS SPACE TO RETRY!", GetScreenWidth() / 2 - MeasureText("PRESS SPACE TO RETRY!", 20) / 2,
                 GetScreenHeight() / 2, 20, RAYWHITE);
        DrawText(TextFormat("%04i", score), 20, 380, 40, GREEN);
    }
    DrawFPS(0, 0);

    EndDrawing();
}
