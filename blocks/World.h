#pragma once
#include <list>

#include "Chunk.h"
#include "Material.h"
#include "Landscape.h"
#include "Character.h"

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
	std::list<Chunk *> *Chunks;
	Landscape lLandscape;
	Character player;

	void BuildWorld();
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z);
	int FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, Chunk **chunk, int *index);
	int AddBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, char mat, bool show);
	int RemoveBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, bool show);

	void DrawLoadedBlocksFinish(Chunk &chunk);
	void DrawUnLoadedBlocks(ChunkInWorld Cx, ChunkInWorld Cz);
	void LoadChunk(ChunkInWorld x, ChunkInWorld z);
	void UnLoadChunk(ChunkInWorld x, ChunkInWorld z);
	void GetChunkByBlock(BlockInWorld x, BlockInWorld z, ChunkInWorld *Cx, ChunkInWorld *Cz);
	Chunk* GetChunkByBlock(BlockInWorld x, BlockInWorld z);
	Chunk* GetChunkByPosition(ChunkInWorld Cx, ChunkInWorld Cz);
	void GetPosInChunkByWorld(BlockInWorld x, BlockInWorld y, BlockInWorld z, BlockInChunk *chnkx, BlockInChunk *chnky, BlockInChunk *chnkz);

	unsigned long Hash(ChunkInWorld x, ChunkInWorld z) {return (x + z*HASH_SIZE)&(HASH_SIZE-1);}
	void UpdateLight(Chunk& chunk);
	bool LightToRefresh;

	HANDLE mutex;
	HANDLE parget;
	HANDLE parget2;
	HANDLE semaphore;

	bool SoftLight;

	GLfloat SkyBright;
	GLfloat TorchBright;

	void SaveChunks();

private:
	void ShowTile(Chunk *chunk, int index, char side);
	void HideTile(Chunk *chunk, int index, char side);

	std::list<ChunkPosition> LoadedChunks;
};
