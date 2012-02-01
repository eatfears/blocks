#pragma once

#include <list>

#include "Blocks_Definitions.h"
#include "Material.h"
#include "Landscape.h"

typedef struct locpos
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
	Chunk(ChunkInWorld x, ChunkInWorld z, MaterialLibrary& MLib, Landscape& lLand);
	~Chunk(void);

	Block *bBlocks;
	char *SkyLight;
	MaterialLibrary& MaterialLib;
	Landscape& lLandscape;
	std::list<Block *> *DisplayedTiles;

	ChunkInWorld x;
	ChunkInWorld z;
	bool bVisible;
	
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

	HANDLE mutex;
};
