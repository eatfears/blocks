#include "Location.h"

Location::Location(LocInWorld x, LocInWorld z, MaterialLibrary& MLib, Landscape& lLand)
	: MaterialLib(MLib), lLandscape(lLand)
{
	Location::x = x; Location::z = z;

	bBlocks = new Block[LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y];

	DisplayedTiles = new std::list<Block *>[6];
	
	for(int i = 0; i < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y; i++)
	{	bBlocks[i].cMaterial = MAT_NO;
		for(int N = 0; N < 6; N++)
			bBlocks[i].bVisible[N] = false;
	}
	bVisible = false;

	mutex = CreateMutex(NULL, false, NULL);
}

Location::~Location(void)
{
	delete[] bBlocks;

	delete[] DisplayedTiles;
}

int Location::AddBlock(BlockInLoc x, BlockInLoc y, BlockInLoc z, char mat)
{
	x = x%LOCATION_SIZE_XZ;
	y = y%LOCATION_SIZE_Y;
	z = z%LOCATION_SIZE_XZ;
	if((GetBlockMaterial(x, y, z) != MAT_NO)||(GetBlockMaterial(x, y, z) == -1)) return -1;

	BlockInLoc index = SetBlockMaterial(x, y, z, mat);
	
	return index;
}

int Location::RemoveBlock(BlockInLoc x, BlockInLoc y, BlockInLoc z)
{
	if((GetBlockMaterial(x, y, z) == MAT_NO)||(GetBlockMaterial(x, y, z) == -1)) return -1;

	BlockInLoc index = SetBlockMaterial(x, y, z, MAT_NO);

	return index;
}

void Location::ShowTile(Block *tTile, char N)
{
	if(!tTile) return;
	if(tTile->cMaterial == MAT_NO) return;
	if(tTile->bVisible[N]) return;
	
	DisplayedTiles[N].push_back(tTile);
		
	tTile->bVisible[N] = true;
}

void Location::HideTile(Block *tTile, char N)
{
	if(!tTile) return;
	if(tTile->cMaterial == MAT_NO) return;
	if(!tTile->bVisible[N]) return;

	auto it = DisplayedTiles[N].begin();

	while(it != DisplayedTiles[N].end())
	{
		if(*it == tTile) break;
		++it;
	}
	if(it == DisplayedTiles[N].end()) return;

	(*it)->bVisible[N] = false;
	DisplayedTiles[N].erase(it);
	return;
}

char Location::GetBlockMaterial(BlockInLoc x, BlockInLoc y, BlockInLoc z)
{
	if((x >= LOCATION_SIZE_XZ)||(z >= LOCATION_SIZE_XZ)||(y >= LOCATION_SIZE_Y))
	{
		return -1;
	}
	return bBlocks[x*LOCATION_SIZE_XZ + z + y*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ].cMaterial;
}

int Location::SetBlockMaterial(BlockInLoc x, BlockInLoc y, BlockInLoc z, char cMat)
{
	if((x >= LOCATION_SIZE_XZ)||(z >= LOCATION_SIZE_XZ)||(y >= LOCATION_SIZE_Y))
		return -1;

	int index = GetIndexByPosition(x, y, z);
	bBlocks[index].cMaterial = cMat;
	return index;
}

int Location::GetBlockPositionByPointer(Block *tCurrentTile, BlockInLoc *x, BlockInLoc *y, BlockInLoc *z)
{
	int t = tCurrentTile - bBlocks;

	if(GetBlockPositionByIndex(t, x, y, z) == -1) 
		return -1;

	return 0;
}

int Location::GetBlockPositionByIndex(int index, BlockInLoc *x, BlockInLoc *y, BlockInLoc *z)
{
	if((index < 0)||(index >= LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y))
		return -1;

	*z  = index%LOCATION_SIZE_XZ;
	index/=LOCATION_SIZE_XZ;
	*x = index%LOCATION_SIZE_XZ;
	index/=LOCATION_SIZE_XZ;
	*y = index;

	return 0;
}

int Location::GetIndexByPosition( BlockInLoc x, BlockInLoc y, BlockInLoc z )
{
	return x*LOCATION_SIZE_XZ + z + y*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ;
}

void Location::Generate()
{
	lLandscape.Generate(*this);
}
