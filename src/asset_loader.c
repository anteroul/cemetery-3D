#include "headers.h"

Sound loadSound(char* path)
{
	return LoadSound(path);
}

Music loadMusic(char* path)
{
	return LoadMusicStream(path);
}

Texture2D loadTex(char* path)
{
	Image img = LoadImage(path);
	Texture2D tex = LoadTextureFromImage(img);
	
	UnloadImage(img);

	return tex;
}

char* fixPath(char* path)
{
	char root[10] = "../../../";
	char* newPath;

	newPath = malloc(strlen(path) + 10);
	
	strcpy(newPath, root);
	strcat(newPath, path);

	char pathCopy = *newPath;
	
	free(newPath);

	return pathCopy;
}