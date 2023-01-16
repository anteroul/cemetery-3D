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

char* concat(char path[], int parentDirs)
{
	char* root = "";

	if (parentDirs == 0)
	{
		return path;
	}
	else if (parentDirs == 1 || parentDirs == 2)
	{
		root = "../";
	}
	else
	{
		root = "../../../";
	}
    
	char* newPath = (char*)malloc(strlen(path) + strlen(root));
	const char* finalPath = strdup(newPath);
	newPath = NULL;

    sprintf(finalPath, "%s%s", root, path);
    printf("%s\n", finalPath);
	free(newPath);

    return finalPath;
}