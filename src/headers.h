#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------Asset Loaders-------*/
Sound loadSound(char path[]);
Music loadMusic(char path[]);
Texture2D loadTex(char path[]);
/*---------------------------*/

/*------Helper Functions-----*/
char* concat(char path[], int parentDirs);
/*---------------------------*/

#endif