#pragma once

#include <list>

#include "Blocks_Definitions.h"
#include "Material.h"

typedef unsigned short	TileInLoc;
typedef signed short	LocInWorld;
typedef signed short	TileInWorld;

typedef struct t
{
	char cMaterial;
	bool bVisible[6];
}Tile;

class Location
{
public:
	Location(LocInWorld x, LocInWorld z);
	~Location(void);

	Tile *tTile;
	std::list<Tile *> *DisplayedTiles;
	std::list<Tile *>::iterator **TexurePointerInVisible;


	LocInWorld x;
	LocInWorld z;
	bool bVisible;

	int	AddTile(TileInLoc x, TileInLoc y, TileInLoc z, char mat);
	int RemoveTile(TileInLoc x, TileInLoc y, TileInLoc z);
	
	void ShowTile(Tile *tTile, char N);
	void HideTile(Tile *tTile, char N);

	char GetTileMaterial(TileInLoc x, TileInLoc y, TileInLoc z);
	TileInLoc SetTileMaterial(TileInLoc x, TileInLoc y, TileInLoc z, char cMat);
	int GetTilePositionByPointer(Tile *tCurrentTile, TileInLoc *x, TileInLoc *y, TileInLoc *z);
};

