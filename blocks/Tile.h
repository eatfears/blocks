#pragma once

class Tile
{
public:
	Tile(int xc, int yc, int zc)
	{ x = xc; y = yc; z = zc; };
	~Tile (void);

	int x;
	int y;
	int z;

	Tile *tUpTile;
	Tile *tDownTile;
	Tile *tLeftTile;
	Tile *tRightTile;
	Tile *tFrontTile;
	Tile *tBottomTile;
};
