#pragma once

#define MATERIAL_NO		0
#define MATERIAL_YES	1

class Tile
{
public:
	Tile(unsigned long xc, unsigned long yc, unsigned long zc, char zmat)
	{ x = xc; y = yc; z = zc; mat = zmat; };
	~Tile (void);

	unsigned long x;
	unsigned long y;
	unsigned long z;

	char mat;
	/*
	Tile *tUpTile;
	Tile *tDownTile;
	Tile *tLeftTile;
	Tile *tRightTile;
	Tile *tFrontTile;
	Tile *tBottomTile;*/
};
