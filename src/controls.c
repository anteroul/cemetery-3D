#include "globals.h"
#include "headers.h"

void sprint(Player* p, Camera* cam)
{
    if (IsKeyDown(32))
    {
        if (p->energy > 0)
        {
            UpdateCamera(cam, CAMERA_FIRST_PERSON);
            if (IsKeyDown(65) || IsKeyDown(83) || IsKeyDown(68) || IsKeyDown(87))
                p->energy--;
            else if (p->energy < 100)
                p->energy++;
        }
    } else if (p->energy < 100) {
        p->energy++;
    }
}

void attack(Player* p, Sound* sound)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !p->attacking)
    {
        if (p->energy >= 30) {
            PlaySound(*sound);
            p->attacking = true;
            p->energy -= 30;
        }
    }
}

void takeDamage(void)
{
    player.hp -= rand() % player.level + 1;
}