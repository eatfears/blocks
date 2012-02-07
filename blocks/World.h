#pragma once
#include <list>

#include "Chunk.h"
#include "Material.h"
#include "Landscape.h"

typedef struct params
{
	ChunkInWorld x;
	ChunkInWorld z;
	World *wWorld;
} Param;

class World
{
public:
	World();
	~World();

	MaterialLibrary MaterialLib;
	std::list<Chunk *> Chunks;
	Landscape lLandscape;

	void BuildWorld();
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z);
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, Chunk **loc, int *index);
	int AddBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, char mat, bool show);
	int RemoveBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, bool show);

	void DrawLoadedBlocksFinish(Chunk &loc);
	void DrawUnLoadedBlocks(ChunkInWorld x, ChunkInWorld z);
	void LoadChunk(ChunkInWorld x, ChunkInWorld z);
	void UnLoadChunk(ChunkInWorld x, ChunkInWorld z);
	std::list<ChunkPosition> LoadedChunks;

	void GetChunkByBlock(BlockInWorld x, BlockInWorld z, ChunkInWorld *locx, BlockInWorld *locz);
	Chunk* GetChunkByBlock(BlockInWorld x, BlockInWorld z);
	void GetPosInChunkByWorld(BlockInWorld x, BlockInWorld y, BlockInWorld z, BlockInChunk *locx, BlockInChunk *locy, BlockInChunk *locz);
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
