#pragma once

#define MATERIAL_NO		0
#define MATERIAL_YES	1

class Tile
{
public:
	Tile(signed long xc, signed long yc, signed long zc, char zmat)
	{ x = xc; y = yc; z = zc; mat = zmat; };
	~Tile (void);

	signed long x;
	signed long y;
	signed long z;

	char mat;
	/*
	Tile *tUpTile;
	Tile *tDownTile;
	Tile *tLeftTile;
	Tile *tRightTile;
	Tile *tFrontTile;
	Tile *tBottomTile;*/
};
