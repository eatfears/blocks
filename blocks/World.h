#pragma once
#include <list>

#include "Location.h"
#include "Material.h"
#include "Landscape.h"

class World
{
public:
	World();
	~World();

	MaterialLibrary MaterialLib;
	std::list<Location> lLocations;
	Landscape lLandscape;

	void BuildWorld();
	int FindTile(TileInWorld x, TileInWorld y, TileInWorld z);
	int FindTile(TileInWorld x, TileInWorld y, TileInWorld z, Location **loc, int *index);
	int AddTile(TileInWorld x, TileInWorld y, TileInWorld z, char mat, bool show);
	int RemoveTile(TileInWorld x, TileInWorld y, TileInWorld z, bool show);

	void DrawLoadedTiles(Location *loc);
	void DrawUnLoadedTiles(LocInWorld x, LocInWorld z);
	void LoadLocation(LocInWorld x, LocInWorld z);
	void UnLoadLocation(LocInWorld x, LocInWorld z);
	std::list<LocationPosiion> LoadedLocations;

	void GetLocByTile(TileInWorld x, TileInWorld z, LocInWorld *locx, TileInWorld *locz);
	Location* GetLocByTile(TileInWorld x, TileInWorld z);
	void GetPosInLocByWorld(TileInWorld x, TileInWorld y, TileInWorld z, TileInLoc *locx, TileInLoc *locy, TileInLoc *locz);
	Location* AddLocation(LocInWorld x, LocInWorld z);
//private:
	void ShowTile(Location *loc, int index, char N);
	void HideTile(Location *loc, int index, char N);

	bool building;
	bool skipbuild;

	HANDLE parget;
	HANDLE mutex;
	HANDLE semaphore;
};
