#pragma once
#include <list>

#include "Location.h"
#include "Material.h"

class World
{
public:
	World();
	~World();

	MaterialLibrary MaterialLib;
	std::list<Location> lLocations;

	void BuildWorld();
	int FindTile(TileInWorld x, TileInWorld y, TileInWorld z);
	int FindTile(TileInWorld x, TileInWorld y, TileInWorld z, Location **loc, int *index);
	int AddTile(TileInWorld x, TileInWorld y, TileInWorld z, char mat, bool show);
	//int RemoveTile(signed short x, signed short y, signed short z, bool show);
	//void DrawLoadedTiles();

	void GetLocByTile(TileInWorld x, TileInWorld z, LocInWorld *locx, TileInWorld *locz);
	Location* GetLocByTile(TileInWorld x, TileInWorld z);
	void GetPosInLocByWorld( TileInWorld x, TileInWorld z, TileInLoc *locx, TileInLoc *locz );
	void AddLocation(LocInWorld x, LocInWorld z);
//private:
	void ShowTile(Location *loc, int index, char N);
	void HideTile(Location *loc, int index, char N);
	//unsigned long ComputeBin(signed short x, signed short y, signed short z);
	//bool building;
	//bool skipbuild;
};

