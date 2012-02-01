#pragma once
#include <list>

#include "Location.h"
#include "Material.h"
#include "Landscape.h"

typedef struct params
{
	LocInWorld x;
	LocInWorld z;
	World *wWorld;
} Param;

class World
{
public:
	World();
	~World();

	MaterialLibrary MaterialLib;
	std::list<Location> lLocations;
	Landscape lLandscape;

	void BuildWorld();
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z);
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, Location **loc, int *index);
	int AddBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, char mat, bool show);
	int RemoveBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, bool show);

	void DrawLoadedBlocks(Location &loc);
	void DrawUnLoadedBlocks(LocInWorld x, LocInWorld z);
	void LoadLocation(LocInWorld x, LocInWorld z);
	void UnLoadLocation(LocInWorld x, LocInWorld z);
	std::list<LocationPosiion> LoadedLocations;

	void GetLocByBlock(BlockInWorld x, BlockInWorld z, LocInWorld *locx, BlockInWorld *locz);
	Location* GetLocByBlock(BlockInWorld x, BlockInWorld z);
	void GetPosInLocByWorld(BlockInWorld x, BlockInWorld y, BlockInWorld z, BlockInLoc *locx, BlockInLoc *locy, BlockInLoc *locz);
//private:
	void ShowTile(Location *loc, int index, char N);
	void HideTile(Location *loc, int index, char N);

	bool building;
	bool skipbuild;

	HANDLE parget;
	HANDLE parget2;
	HANDLE mutex;
	HANDLE semaphore;
};
