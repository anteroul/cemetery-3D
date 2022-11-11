#include "globals.h"

void initGame(void) {
    fxSlash = loadSound(concat("resources/slash.wav"));
    fxBegin = loadSound(concat("resources/begin.wav"));
    fxDeath = loadSound(concat("resources/death.wav"));
    music = loadMusic(concat("resources/music.mp3"));

    player = initPlayer();
    gameOver = false;
    Enemy enemy[ENEMIES];

    for (int i = 0; i < ENEMIES; i++) {
        enemy[i].enemyStartPos.x = rand() % 32 - 16;
        enemy[i].enemyStartPos.y = -1.0f;
        enemy[i].enemyStartPos.z = rand() % 32 - 16;
        enemy[i].enemyBoxPos.x = enemy[i].enemyStartPos.x;
        enemy[i].enemyBoxPos.y = enemy[i].enemyStartPos.y;
        enemy[i].enemyBoxPos.z = enemy[i].enemyStartPos.z;
        enemy[i].active = true;
    }

    hud = LoadTexture(concat("resources/hud.png"));
    wall = LoadTexture(concat("resources/wall.png"));
    sword = LoadTexture(concat("resources/sword.png"));
    crosshair = LoadTexture(concat("resources/crosshair.png"));
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

// TODO: FIX THIS!
void updateGame(void) {
    UpdateMusicStream(music);
    // Update
    drawGame();
    Vector3 oldCamPos = camera.position;
    UpdateCamera(&camera);                  // Update camera

    if (!gameOver) {
        //Running logic

        if (IsKeyDown(32)) {
            if (player.energy > 0) {
                UpdateCamera(&camera);
                if (IsKeyDown(65) || IsKeyDown(83) || IsKeyDown(68) || IsKeyDown(87))
                    player.energy--;
                else if (player.energy < 100)
                    player.energy++;
            }
        } else if (player.energy < 100) {
            player.energy++;
        }

        // Sword moves with camera

        swordPosition.y = 50.0f * camera.target.y;
        swordPosition.x = 340.0f;

        if (camera.target.y <= 2.5f)
            swordPosition.y += 30.0f;
        if (camera.target.y <= 1.5f)
            swordPosition.y += 25.0f;

        player.framesCounter++;

        // Bounding boxes

        // Attack Range
        struct BoundingBox range = {
                camera.target.x - 0.5f, camera.target.y - 1.0f, camera.target.z - 0.5f,
                camera.target.x + 0.5f, camera.target.y + 1.5f, camera.target.z + 0.5f
        };

        // Player Bounding Box
        struct BoundingBox playerHitBox = {
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
                    else
                        arr_enemy[i].enemyBoxPos.x -= 0.04f;

                    if (camera.position.z > arr_enemy[i].enemyBoxPos.z - 3.0f)
                        arr_enemy[i].enemyBoxPos.z += 0.04f;
                    else
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

                if (CheckCollisionBoxes(playerHitBox, arr_enemy[i].enemyBounds) && arr_enemy[i].enemyBoxPos.y >= 1.0f) {
                    if (((player.framesCounter / 30) % 2) == 1) {
                        player.hp -= rand() % player.level + 1;
                        player.framesCounter = 0;
                    }
                }

                // Attack enemy logic

                if (player.attacking) {
                    if (CheckCollisionBoxes(range, arr_enemy[i].enemyBounds)) {
                        player.score += 100;
                        player.level++;
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

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player.attacking) {
            if (player.energy >= 30) {
                PlaySound(fxSlash);
                player.attacking = true;
                player.energy -= 30;
            }
        }
        if (player.attacking) {
            frameRec.y = 400.0f;
            frameRec.x = 400.0f * player.currentFrame;
            swordPosition.x = 240.0f;
            swordPosition.y = 145.0f;
            player.attackCounter++;
            if (player.attackCounter > 2) {
                player.currentFrame++;
                if (player.currentFrame >= NUM_FRAMES_PER_LINE) {
                    player.currentFrame = 0;
                    player.attacking = false;
                }
                player.attackCounter = 0;
            }
        } else {
            frameRec.y = 0.0f;
            frameRec.x = 0.0f;
        }

        // Game Over logic

        if (player.hp <= 0) {
            StopMusicStream(music);
            gameOver = true;
        }
    }
}

void animateSword(void) {
    // Sword moves with camera:
    swordPosition.y = 50.0f * camera.target.y;
    swordPosition.x = 340.0f;

    if (camera.target.y <= 2.5f)
        swordPosition.y += 30.0f;
    if (camera.target.y <= 1.5f)
        swordPosition.y += 25.0f;

    if (player.attacking) {
        frameRec.y = 400.0f;
        frameRec.x = 400.0f * player.currentFrame;
        swordPosition.x = 240.0f;
        swordPosition.y = 145.0f;
        player.attackCounter++;
        if (player.attackCounter > 2) {
            player.currentFrame++;
            if (player.currentFrame >= NUM_FRAMES_PER_LINE) {
                player.currentFrame = 0;
                player.attacking = false;
            }
            player.attackCounter = 0;
        }
    } else {
        frameRec.y = 0.0f;
        frameRec.x = 0.0f;
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
        DrawText(TextFormat("%04i", player.score), 20, 380, 40, GREEN);
        DrawText(TextFormat("%03i", player.hp), 720, 380, 40, RED);
        DrawText(TextFormat("%03i", player.energy), 580, 380, 40, BLUE);
        DrawTexture(crosshair, 390, 215, WHITE);
        DrawTexture(hud, 0, 0, WHITE);

    } else {
        if (!deathSound) {
            PlaySound(fxDeath);
            deathSound = true;
        }

        DrawText("YOU ARE DEAD!", GetScreenWidth() / 2 - MeasureText("YOU ARE DEAD!", 20) / 2,
                 GetScreenHeight() / 2 - 50, 20, RED);
        DrawText("PRESS SPACE TO RETRY!", GetScreenWidth() / 2 - MeasureText("PRESS SPACE TO RETRY!", 20) / 2,
                 GetScreenHeight() / 2, 20, RAYWHITE);
        DrawText(TextFormat("%04i", player.score), 20, 380, 40, GREEN);

        if (IsKeyDown(32)) {
            resetGame();
            gameOver = false;
        }
    }
    DrawFPS(0, 0);

    EndDrawing();
}
