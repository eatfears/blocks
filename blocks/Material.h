#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "Tiles.h"

#define MAT_NUMBER			10

#define MAT_NO				0
#define MAT_DIRT			1
#define MAT_GRASS			2
#define MAT_STONE			3
#define MAT_SAND			4

#define TEX_NO				0

#define TEX_DIRT_TOP		1
#define TEX_DIRT_DOWN		1
#define TEX_DIRT_SIDE		1

#define TEX_GRASS_TOP		2
#define TEX_GRASS_DOWN		1
#define TEX_GRASS_SIDE		3

#define TEX_STONE_TOP		4
#define TEX_STONE_DOWN		4
#define TEX_STONE_SIDE		4

#define TEX_SAND_TOP		5
#define TEX_SAND_DOWN		5
#define TEX_SAND_SIDE		5

typedef struct  
{
	int iTex[6];
}stMater;

class Material
{
public:
	Material(void);
	~Material(void);

	stMater *mMater;
	GLuint *textures;
	int TexturesNum;

	std::list<Tile*>::iterator **TexPtr;

	void InitMaterials();
	GLvoid LoadGLTextures();

};

