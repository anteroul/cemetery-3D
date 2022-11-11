#include "globals.h"

Player initPlayer(void)
{
    Player _player;

    _player.hp = 100;
    _player.energy = 100;
    _player.score = 0;
    _player.level = 1;
    _player.attacking = false;

    return _player;
}