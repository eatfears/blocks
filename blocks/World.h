#pragma once
#include <list>

#include "Chunk.h"
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
	std::list<Chunk> lLocations;
	Landscape lLandscape;

	void BuildWorld();
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z);
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, Chunk **loc, int *index);
	int AddBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, char mat, bool show);
	int RemoveBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, bool show);

	void DrawLoadedBlocksFinish(Chunk &loc);
	void DrawUnLoadedBlocks(LocInWorld x, LocInWorld z);
	void LoadLocation(LocInWorld x, LocInWorld z);
	void UnLoadLocation(LocInWorld x, LocInWorld z);
	std::list<LocationPosiion> LoadedLocations;

	void GetLocByBlock(BlockInWorld x, BlockInWorld z, LocInWorld *locx, BlockInWorld *locz);
	Chunk* GetLocByBlock(BlockInWorld x, BlockInWorld z);
	void GetPosInLocByWorld(BlockInWorld x, BlockInWorld y, BlockInWorld z, BlockInLoc *locx, BlockInLoc *locy, BlockInLoc *locz);
//private:
	void ShowTile(Chunk *loc, int index, char N);
	void HideTile(Chunk *loc, int index, char N);

	bool building;
	bool skipbuild;

	HANDLE parget;
	HANDLE parget2;
	HANDLE mutex;
	HANDLE semaphore;
};
