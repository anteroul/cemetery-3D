#include "globals.h"
#include "headers.h"

void checkInput(void)
{
    // Attack:
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player.attacking)
    {
        if (player.energy >= 30) {
            PlaySound(fxSlash);
            player.attacking = true;
            player.energy -= 30;
        }
    }
    //Running logic:
    if (IsKeyDown(32))
    {
        if (player.energy > 0)
        {
            UpdateCamera(&camera);
            if (IsKeyDown(65) || IsKeyDown(83) || IsKeyDown(68) || IsKeyDown(87))
                player.energy--;
            else if (player.energy < 100)
                player.energy++;
        }
    } else if (player.energy < 100) {
        player.energy++;
    }
}

void takeDamage(void)
{
    player.hp -= rand() % player.level + 1;
}