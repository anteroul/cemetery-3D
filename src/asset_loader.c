#include "headers.h"

Sound loadSound(char path[])
{
	return LoadSound(path);
}

Music loadMusic(char path[])
{
	return LoadMusicStream(path);
}

Texture2D loadTex(char path[])
{
	Image img = LoadImage(path);
	Texture2D tex = LoadTextureFromImage(img);
	
	UnloadImage(img);

	return tex;
}

char* concat(char path[])
{
	char root[] = "../";
    char newPath[strlen(path) + strlen(root)];

    sprintf(newPath, "%s%s", root, path);
    printf("%s\n", newPath);

    return newPath;
}