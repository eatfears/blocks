#pragma once

#include <list>

#include "Blocks_Definitions.h"
#include "Material.h"
#include "Mutex.h"

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
	Location(LocInWorld x, LocInWorld z, MaterialLibrary *MaterialLib);
	~Location(void);

	Tile *tTile;
	MaterialLibrary *MaterialLib;
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
	int SetTileMaterial(TileInLoc x, TileInLoc y, TileInLoc z, char cMat);
	
	int GetTilePositionByPointer(Tile *tCurrentTile, TileInLoc *x, TileInLoc *y, TileInLoc *z);
	int GetIndexByPosition(TileInLoc x, TileInLoc y, TileInLoc z);

// 	void Acquire() {AccessMutex.Acquire();}
// 	void Release() {AccessMutex.Release();}
// 
// private:
// 	Mutex AccessMutex;
};

