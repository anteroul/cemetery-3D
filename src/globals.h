#ifndef VALIANTENGINE_GLOBALS_H
#define VALIANTENGINE_GLOBALS_H

#include "headers.h"

#define NUM_FRAMES_PER_LINE     4
#define ENEMIES                 4


typedef struct Player {
    int hp;
    int energy;
    int score;
    int level;
    bool attacking;
} Player;

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
static int currentFrame;
static int framesCounter;
static bool gameOver;
static bool deathSound;

// Structs:
static Player player;
static Enemy arr_enemy[ENEMIES];
static Texture2D hud;
static Texture2D wall;
static Texture2D sword;
static Texture2D crosshair;
static Model wallModel;
static Vector2 swordPosition;
static Rectangle frameRec;
static Camera camera = { 0 };

// Gameplay functions:
void initGame(void);
void drawGame(void);
void updateGame(void);
void resetGame(void);

// Initializers:
Player initPlayer(void);

// Animations and gameplay logic:
void animateSword(void);
void sprint(Player* p, Camera* cam);
void attack(Player* p, Sound* sound);
void kill(Enemy* enemy);
void takeDamage(void);

#endif //VALIANTENGINE_GLOBALS_H
