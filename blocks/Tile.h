#pragma once

#define MATERIAL_NO		0
#define MATERIAL_YES	1

class Tile
{
public:
	Tile(signed short xc, signed short yc, signed short zc, char zmat)
	{ x = xc; y = yc; z = zc; mat = zmat; };
	~Tile (void);

	signed short x;
	signed short y;
	signed short z;

	char mat;
	/*
	Tile *tUpTile;
	Tile *tDownTile;
	Tile *tLeftTile;
	Tile *tRightTile;
	Tile *tFrontTile;
	Tile *tBottomTile;*/
};
