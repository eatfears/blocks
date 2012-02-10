#pragma once
#include "Blocks_Definitions.h"
#include <GLblocks\blocksglut.h>

#define MAT_NO				0
#define MAT_DIRT			1
#define MAT_STONE			2
#define MAT_SAND			3
#define MAT_WATER			4
#define MAT_GLASS			5
#define MAT_WOOD			6
#define MAT_COAL			7
#define MAT_BRICKS			8
#define MAT_PUMPKIN			9
#define MAT_PUMPKIN_SHINE	10
#define MAT_TNT				11
#define MAT_PLANK			12

#define TERRAIN				0
#define UNDERWATER			1
#define VIGNETTE			2
#define SUN					3
#define MOON				4

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

	void GetTextureOffsets(double& offsetx, double& offsety, int material, char covered, int N);
};
