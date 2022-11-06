#include "headers.h"

Sound loadSound(char* path)
{
	return LoadSound(path);
}

Music loadMusic(char* path)
{
	return LoadMusicStream(path);
}

Texture2D loadGraphics(char* path)
{
	Image img = LoadImage(path);
	Texture2D tex = LoadTextureFromImage(img);
	UnloadImage(img);

	return tex;
}