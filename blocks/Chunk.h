#pragma once
#include <list>

#include "Blocks_Definitions.h"
#include <GLblocks\blocksglut.h>

class World;

typedef struct chnkpos
{
	ChunkInWorld x;
	ChunkInWorld z;
}ChunkPosition;

typedef struct block
{
	char cMaterial;
	bool bVisible[6];
}Block;

class Chunk
{
public:
	Chunk(ChunkInWorld x, ChunkInWorld z, World& wrld);
	~Chunk();

	Block *bBlocks;
	char *SkyLight;
	World& wWorld;
	std::list<Block *> *DisplayedTiles;
	std::list<Block *> *DisplayedWaterTiles;
	ChunkInWorld x;
	ChunkInWorld z;
	char NeedToRender[2];
	
	int	AddBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z, char mat);
	int RemoveBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z);
	
	void ShowTile(Block *bBlock, char N);
	void HideTile(Block *bBlock, char N);

	char GetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z);
	int SetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z, char cMat);
	
	int GetBlockPositionByPointer(Block *bCurrentBlock, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z) const;
	static inline int GetBlockPositionByIndex(int index, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z);
	int GetIndexByPosition(BlockInChunk x, BlockInChunk y, BlockInChunk z);

	void DrawLoadedBlocks();

	void Generate();
	void FillSkyLight(char bright);

	void Render(GLenum mode, char mat);
	void DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char N);

	HANDLE mutex;

	GLuint RenderList;
	bool listgen;
	bool LightToUpdate;
};
