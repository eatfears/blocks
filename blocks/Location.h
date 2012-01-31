#pragma once

#include <list>

#include "Blocks_Definitions.h"
#include "Material.h"
#include "Landscape.h"

typedef struct locpos
{
	LocInWorld x;
	LocInWorld z;
}LocationPosiion;

typedef struct block
{
	char cMaterial;
	bool bVisible[6];
}Block;

class Location
{
public:
	Location(LocInWorld x, LocInWorld z, MaterialLibrary& MLib, Landscape& lLand);
	~Location(void);

	Block *bBlocks;
	MaterialLibrary& MaterialLib;
	Landscape& lLandscape;
	std::list<Block *> *DisplayedTiles;

	LocInWorld x;
	LocInWorld z;
	bool bVisible;
	
	int	AddBlock(BlockInLoc x, BlockInLoc y, BlockInLoc z, char mat);
	int RemoveBlock(BlockInLoc x, BlockInLoc y, BlockInLoc z);
	
	void ShowTile(Block *bBlock, char N);
	void HideTile(Block *bBlock, char N);

	char GetBlockMaterial(BlockInLoc x, BlockInLoc y, BlockInLoc z);
	int SetBlockMaterial(BlockInLoc x, BlockInLoc y, BlockInLoc z, char cMat);
	
	int GetBlockPositionByPointer(Block *bCurrentBlock, BlockInLoc *x, BlockInLoc *y, BlockInLoc *z);
	int GetBlockPositionByIndex(int index, BlockInLoc *x, BlockInLoc *y, BlockInLoc *z);
	int GetIndexByPosition(BlockInLoc x, BlockInLoc y, BlockInLoc z);

	void Generate();

	HANDLE mutex;
};
