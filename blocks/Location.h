#pragma once

#include <vector>

typedef struct t
{
	char cMaterial;
	bool bVisible[6];
}Tile;

class Location
{
public:
	Location(void);
	~Location(void);

	std::vector<Tile> tTile;

	signed short x;
	signed short z;

	char GetTileMaterial(signed short x, signed short y, signed short z);
	void SetTileMaterial(signed short x, signed short y, signed short z, char cMat);
	void GetTilePositionByPointer(Tile *tCurrentTile, signed short *x, signed short *y, signed short *z);
};

