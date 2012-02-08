#pragma once
#include "Blocks_Definitions.h"
#include <GLblocks\blocksglut.h>

#define MAT_NO				0
#define MAT_DIRT			1
#define MAT_GRASS			2
#define MAT_STONE			3
#define MAT_SAND			4
#define MAT_WATER			5
#define MAT_GLASS			6
#define MAT_WOOD			7
#define MAT_SNOW			8
#define MAT_COAL			9
#define MAT_BRICKS			10
#define MAT_PUMPKIN			11
#define MAT_PUMPKIN_SHINE	12
#define MAT_TNT				13
#define MAT_PLANK			14

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
