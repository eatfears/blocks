#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
//#include <gl\glaux.h>

//#include "Location.h"

#define MAT_NO				0
#define MAT_DIRT			1
#define MAT_GRASS			2
#define MAT_STONE			3
#define MAT_SAND			4
#define MAT_WATER			5
#define MAT_GLASS			6

class MaterialLibrary
{
public:
	MaterialLibrary();
	~MaterialLibrary();

	GLuint *texture;

	void AllocGLTextures();
	void LoadGLTextures();

	int GetTextureInfo(int ColourType);
	GLuint loadImage(const char *filename);

	void GetTextureOffsets(double& offsetx, double& offsety, int material, int N);
};

