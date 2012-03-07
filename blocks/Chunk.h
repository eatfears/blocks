#pragma once
#include <list>

#include "Blocks_Definitions.h"
#include <GL/freeglut.h>

class World;

typedef struct chnkpos
{
	ChunkInWorld x;
	ChunkInWorld z;
}ChunkPosition;

typedef struct blockstruct
{
	char cMaterial;
	char bVisible;
}Block;

class Chunk
{
public:
	Chunk(ChunkInWorld x, ChunkInWorld z, World& wrld);
	~Chunk();

	Block *bBlocks;
	char *SkyLight;
	char *TorchLight;
	World& wWorld;
	std::list<Block *> *DisplayedTiles;
	std::list<Block *> *DisplayedWaterTiles;
	ChunkInWorld x;
	ChunkInWorld z;
	char NeedToRender[2];

	int	AddBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z, char mat);
	int RemoveBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z);

	void ShowTile(Block *bBlock, char side);
	void HideTile(Block *bBlock, char side);

	char GetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z);

	int GetBlockPositionByPointer(Block *bCurrentBlock, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z) const;
	static int GetBlockPositionByIndex(int index, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z);
	static int GetIndexByPosition(BlockInChunk x, BlockInChunk y, BlockInChunk z);

	void DrawLoadedBlocks();

	void Open();
	void Save();
	bool LightToUpdate;

	void Render(char mat, int *rendered);

	HANDLE mutex;
	HANDLE loadmutex;

private:
	int SetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z, char cMat);
	void DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, Block* block, char side);

	GLuint RenderList;
	bool listgen;
};
