#include "globals.h"

void initGame(int buildDirConfig)
{
    fxSlash = loadSound(concat("resources/slash.wav", buildDirConfig));
    fxBegin = loadSound(concat("resources/begin.wav", buildDirConfig));
    fxDeath = loadSound(concat("resources/death.wav", buildDirConfig));
    music = loadMusic(concat("resources/music.mp3", buildDirConfig));
    hud = loadTex(concat("resources/hud.png", buildDirConfig));
    wall = loadTex(concat("resources/wall.png", buildDirConfig));
    sword = loadTex(concat("resources/sword.png", buildDirConfig));
    crosshair = loadTex(concat("resources/crosshair.png", buildDirConfig));

    player = initPlayer();
    currentFrame = 0;
    framesCounter = 0;
    gameOver = false;
    Enemy enemy[ENEMIES];

    for (int i = 0; i < ENEMIES; i++)
    {
        enemy[i].enemyStartPos.x = rand() % 32 - 16;
        enemy[i].enemyStartPos.y = -1.0f;
        enemy[i].enemyStartPos.z = rand() % 32 - 16;
        enemy[i].enemyBoxPos.x = enemy[i].enemyStartPos.x;
        enemy[i].enemyBoxPos.y = enemy[i].enemyStartPos.y;
        enemy[i].enemyBoxPos.z = enemy[i].enemyStartPos.z;
        enemy[i].active = true;
    }

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
    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    deathSound = false;
    PlaySound(fxBegin);
    PlayMusicStream(music);
}

void updateGame(Vector2 winSize)
{
    const Vector2 windowSize = winSize;
    UpdateMusicStream(music);
    // Update
    drawGame();
    Vector2 mousePos = GetMousePosition();
    Vector3 oldCamPos = camera.position;
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);                  // Update camera

    if (!gameOver)
    {
        HideCursor();

        if (mousePos.x >= windowSize.x - 1 || mousePos.x <= windowSize.x + 1)
            SetMousePosition(windowSize.x / 2, windowSize.y / 2);

        if (mousePos.y >= windowSize.y - 1 || mousePos.y <= windowSize.y + 1)
            SetMousePosition(windowSize.x / 2, windowSize.y / 2);

        // Check input for sprinting:
        sprint(&player, &camera);

        // Bounding boxes

        // Attack Range
        BoundingBox range = {
                camera.target.x - 0.5f, camera.target.y - 0.5f, camera.target.z - 0.5f,
                camera.target.x + 0.5f, camera.target.y + 0.5f, camera.target.z + 0.5f
        };

        // Player Bounding Box
        BoundingBox playerHitBox = {
                camera.position.x - 1.0f, camera.position.y - 1.5f, camera.position.z - 1.0f,
                camera.position.x + 1.0f, camera.position.y + 0.5f, camera.position.z + 1.0f
        };

        // Enemy logic

        for (int i = 0; i < ENEMIES; i++)
        {
            if (arr_enemy[i].active == true)
            {
                if (arr_enemy[i].enemyBoxPos.y >= 1.0f)
                {
                    // Follow player
                    if (camera.position.x >= arr_enemy[i].enemyBoxPos.x)
                        arr_enemy[i].enemyBoxPos.x += 0.04f;
                    else
                        arr_enemy[i].enemyBoxPos.x -= 0.04f;

                    if (camera.position.z >= arr_enemy[i].enemyBoxPos.z)
                        arr_enemy[i].enemyBoxPos.z += 0.04f;
                    else
                        arr_enemy[i].enemyBoxPos.z -= 0.04f;

                    // Define Bounding Box for enemy

                    BoundingBox cBounds = {
                            arr_enemy[i].enemyBoxPos.x - 2.5f, 0.0f, arr_enemy[i].enemyBoxPos.z - 2.5f,
                            arr_enemy[i].enemyBoxPos.x + 2.5f, 2.5f, arr_enemy[i].enemyBoxPos.z + 2.5f
                    };

                    arr_enemy[i].enemyBounds = cBounds;

                } else if (arr_enemy[i].enemyBoxPos.y < 1.0f)
                {
                    arr_enemy[i].enemyBoxPos.y += 0.01f;
                }

                // Attack player logic

                if (CheckCollisionBoxes(playerHitBox, arr_enemy[i].enemyBounds) && arr_enemy[i].enemyBoxPos.y >= 1.0f)
                {
                    if (((framesCounter / 30) % 2) == 1)
                    {
                        player.hp -= rand() % player.level + 1;
                        framesCounter = 0;
                    }
                }

                // Attack enemy logic

                if (player.attacking)
                {
                    if (CheckCollisionBoxes(range, arr_enemy[i].enemyBounds))
                    {
                        player.score += 100;
                        player.level++;
                        arr_enemy[i].active = false;
                    }
                }

            }

                // Respawn

            else
            {
                arr_enemy[i].enemyBoxPos.x = rand() % 32 - 16;
                arr_enemy[i].enemyBoxPos.y = -1.5f;
                arr_enemy[i].enemyBoxPos.z = rand() % 32 - 16;

                BoundingBox newBounds = {
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

        // Check input for attack:
        attack(&player, &fxSlash);
        animateSword();
        framesCounter++;

        // Game Over logic:
        if (player.hp <= 0)
        {
            StopMusicStream(music);
            gameOver = true;
        }
    } else {
        if (IsKeyPressed(32))
        {
            player = initPlayer();
            camera.position = (Vector3){ 4.0f, 2.0f, 4.0f };
            deathSound = false;
            PlaySound(fxBegin);
            PlayMusicStream(music);

            for (int i = 0; i < ENEMIES; i++)
            {
                arr_enemy[i].enemyBoxPos.x = rand() % 32 - 16;
                arr_enemy[i].enemyBoxPos.y = -1.0f;
                arr_enemy[i].enemyBoxPos.z = rand() % 32 - 16;
            }

            gameOver = false;
        }

        ShowCursor();
    }
}

void animateSword(void)
{
    int attackCounter = framesCounter;
    // Sword moves with camera:
    swordPosition.y = 2.5f * camera.target.y + 120.f;
    swordPosition.x = 280.0f;

    if (player.attacking)
    {
        frameRec.y = 400.0f;
        frameRec.x = 400.0f * (float) currentFrame;
        swordPosition.x = 240.0f;
        swordPosition.y = 145.0f;
        attackCounter++;
        if (attackCounter > 2)
        {
            if (currentFrame >= NUM_FRAMES_PER_LINE)
            {
                currentFrame = 0;
                player.attacking = false;
            }
            currentFrame++;
        }
    } else {
        frameRec.y = 0.0f;
        frameRec.x = 0.0f;
    }
}

void drawGame(void)
{
    BeginDrawing();

    ClearBackground(BLACK);

    if (!gameOver)
    {
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
        if (!deathSound)
        {
            PlaySound(fxDeath);
            deathSound = true;
        }

        DrawText("YOU ARE DEAD!", GetScreenWidth() / 2 - MeasureText("YOU ARE DEAD!", 20) / 2,
            GetScreenHeight() / 2 - 50, 20, RED);
        DrawText("PRESS SPACE TO RETRY!", GetScreenWidth() / 2 - MeasureText("PRESS SPACE TO RETRY!", 20) / 2,
            GetScreenHeight() / 2, 20, RAYWHITE);
        DrawText(TextFormat("%04i", player.score), 20, 380, 40, GREEN);
    }
    DrawFPS(0, 0);
    EndDrawing();
}
