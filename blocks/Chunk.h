#pragma once
#include <list>

#include "Blocks_Definitions.h"
#include <GL/freeglut.h>

class World;

typedef struct chnkpos
{
	ChunkCoord x;
	ChunkCoord z;
}ChunkPosition;

typedef struct blockstruct
{
	char cMaterial;
	char bVisible;
}Block;

class Chunk
{
public:
	Chunk(ChunkCoord x, ChunkCoord z, World& wrld);
	~Chunk();

	Block *bBlocks;
	char *SkyLight;
	char *TorchLight;
	World& wWorld;
	std::list<Block *> *DisplayedTiles;
	std::list<Block *> *DisplayedWaterTiles;
	ChunkCoord x;
	ChunkCoord z;
	char NeedToRender[2];

	int	AddBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat);
	int RemoveBlock(BlockCoord x, BlockCoord y, BlockCoord z);

	void ShowTile(Block *bBlock, char side);
	void HideTile(Block *bBlock, char side);

	char GetBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z);

	int GetBlockPositionByPointer(Block *bCurrentBlock, BlockCoord *x, BlockCoord *y, BlockCoord *z) const;
	static int GetBlockPositionByIndex(int index, BlockCoord *x, BlockCoord *y, BlockCoord *z);
	static int GetIndexByPosition(BlockCoord x, BlockCoord y, BlockCoord z);

	void DrawLoadedBlocks();

	void Open();
	void Save();
	bool LightToUpdate;

	void Render(char mat, int *rendered);

	HANDLE mutex;
	HANDLE loadmutex;

private:
	int SetBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char cMat);
	void DrawTile(BlockInWorld pos, Block* block, char side);

	GLuint RenderList;
	bool listgen;
};
