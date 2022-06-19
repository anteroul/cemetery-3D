//
// Created by valiant on 19.6.2022.
//

#ifndef VALIANTENGINE_GLOBALS_H
#define VALIANTENGINE_GLOBALS_H

#define NUM_FRAMES_PER_LINE     4
#define ENEMIES                 4

#include "raylib.h"

typedef struct Enemy {
    Vector3 enemyStartPos;
    Vector3 enemyBoxPos;
    BoundingBox enemyBounds;
    bool active;
} Enemy;

// Sounds:
static Sound fxSlash;
static Sound fxBegin;
static Sound fxDeath;
static Music music;

// Variables:
static int hp;
static int energy;
static int score;
static int level;
static int currentFrame;
static int framesCounter;
static int attackCounter;
static bool attacking;
static bool gameOver;
static bool deathSound;

// Functions:
static void initGame(void);
static void drawGame(void);
static void updateGame(void);
static void resetGame(void);

// Structs:
static Enemy arr_enemy[ENEMIES];
static Texture2D hud;
static Texture2D wall;
static Texture2D sword;
static Texture2D crosshair;
static Model wallModel;
static Vector2 swordPosition;
static Rectangle frameRec;
static Camera camera = { 0 };

#endif //VALIANTENGINE_GLOBALS_H
