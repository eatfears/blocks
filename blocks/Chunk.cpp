#include "Chunk.h"

Chunk::Chunk(ChunkInWorld x, ChunkInWorld z, MaterialLibrary& MLib, Landscape& lLand)
	: MaterialLib(MLib), lLandscape(lLand)
{
	Chunk::x = x; Chunk::z = z;

	bBlocks = new Block[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];
	SkyLight = new char[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];

	DisplayedTiles = new std::list<Block *>[6];
	
	for(int i = 0; i < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; i++)
	{	
		SkyLight[i] = 0;
		bBlocks[i].cMaterial = MAT_NO;
		for(int N = 0; N < 6; N++)
			bBlocks[i].bVisible[N] = false;
	}
	bVisible = false;

	mutex = CreateMutex(NULL, false, NULL);
}

Chunk::~Chunk(void)
{
	delete[] bBlocks;
	delete[] SkyLight;

	delete[] DisplayedTiles;
}

int Chunk::AddBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z, char mat)
{
	x = x%CHUNK_SIZE_XZ;
	y = y%CHUNK_SIZE_Y;
	z = z%CHUNK_SIZE_XZ;
	if((GetBlockMaterial(x, y, z) != MAT_NO)||(GetBlockMaterial(x, y, z) == -1)) return -1;

	BlockInChunk index = SetBlockMaterial(x, y, z, mat);
	
	return index;
}

int Chunk::RemoveBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z)
{
	if((GetBlockMaterial(x, y, z) == MAT_NO)||(GetBlockMaterial(x, y, z) == -1)) return -1;

	BlockInChunk index = SetBlockMaterial(x, y, z, MAT_NO);

	return index;
}

void Chunk::ShowTile(Block *bBlock, char N)
{
	if(!bBlock) return;
	if(bBlock->cMaterial == MAT_NO) return;
	if(bBlock->bVisible[N]) return;
	
	DisplayedTiles[N].push_back(bBlock);
		
	bBlock->bVisible[N] = true;
}

void Chunk::HideTile(Block *bBlock, char N)
{
	if(!bBlock) return;
	if(bBlock->cMaterial == MAT_NO) return;
	if(!bBlock->bVisible[N]) return;

	auto it = DisplayedTiles[N].begin();

	while(it != DisplayedTiles[N].end())
	{
		if(*it == bBlock) break;
		++it;
	}
	if(it == DisplayedTiles[N].end()) return;

	(*it)->bVisible[N] = false;
	DisplayedTiles[N].erase(it);
	return;
}

char Chunk::GetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z)
{
	if((x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
	{
		return -1;
	}
	return bBlocks[x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ].cMaterial;
}

int Chunk::SetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z, char cMat)
{
	if((x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
		return -1;

	int index = GetIndexByPosition(x, y, z);
	bBlocks[index].cMaterial = cMat;
	return index;
}

int Chunk::GetBlockPositionByPointer(Block *tCurrentBlock, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z) 
	const
{
	int t = tCurrentBlock - bBlocks;

	if(GetBlockPositionByIndex(t, x, y, z) == -1) 
		return -1;

	return 0;
}

inline int Chunk::GetBlockPositionByIndex(int index, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z)
{
	if((index < 0)||(index >= CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y))
		return -1;

	*z  = index%CHUNK_SIZE_XZ;
	index/=CHUNK_SIZE_XZ;
	*x = index%CHUNK_SIZE_XZ;
	index/=CHUNK_SIZE_XZ;
	*y = index;

	return 0;
}

int Chunk::GetIndexByPosition( BlockInChunk x, BlockInChunk y, BlockInChunk z )
{
	return x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
}

void Chunk::DrawLoadedBlocks()
{
	int index = 0;
	BlockInChunk xx, yy, zz;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		GetBlockPositionByPointer(bBlocks + index, &xx, &yy, &zz);

		if(bBlocks[index].cMaterial != MAT_NO)
		{

			if(bBlocks[index].cMaterial == MAT_WATER)
			{
				if(GetBlockMaterial(xx, yy + 1, zz) == MAT_NO) ShowTile(bBlocks + index, TOP);
				if(GetBlockMaterial(xx, yy - 1, zz) == MAT_NO) ShowTile(bBlocks + index, BOTTOM);
				if(GetBlockMaterial(xx + 1, yy, zz) == MAT_NO) ShowTile(bBlocks + index, RIGHT);
				if(GetBlockMaterial(xx - 1, yy, zz) == MAT_NO) ShowTile(bBlocks + index, LEFT);
				if(GetBlockMaterial(xx, yy, zz + 1) == MAT_NO) ShowTile(bBlocks + index, BACK);
				if(GetBlockMaterial(xx, yy, zz - 1) == MAT_NO) ShowTile(bBlocks + index, FRONT);
			}
			else
			{
				if((GetBlockMaterial(xx, yy + 1, zz) == MAT_NO)||(GetBlockMaterial(xx, yy + 1, zz) == MAT_WATER))
					ShowTile(bBlocks + index, TOP);
				if((GetBlockMaterial(xx, yy - 1, zz) == MAT_NO)||(GetBlockMaterial(xx, yy - 1, zz) == MAT_WATER))
					ShowTile(bBlocks + index, BOTTOM);
				if((GetBlockMaterial(xx + 1, yy, zz) == MAT_NO)||(GetBlockMaterial(xx + 1, yy, zz) == MAT_WATER))
					ShowTile(bBlocks + index, RIGHT);
				if((GetBlockMaterial(xx - 1, yy, zz) == MAT_NO)||(GetBlockMaterial(xx - 1, yy, zz) == MAT_WATER))
					ShowTile(bBlocks + index, LEFT);
				if((GetBlockMaterial(xx, yy, zz + 1) == MAT_NO)||(GetBlockMaterial(xx, yy, zz + 1) == MAT_WATER))
					ShowTile(bBlocks + index, BACK);
				if((GetBlockMaterial(xx, yy, zz - 1) == MAT_NO)||(GetBlockMaterial(xx, yy, zz - 1) == MAT_WATER))
					ShowTile(bBlocks + index, FRONT);
			}
		}
		index++;
	}
}


void Chunk::Generate()
{
	lLandscape.Generate(*this);
}

void Chunk::FillSkyLight(char bright)
{
	BlockInChunk y;
	int index;

	for(BlockInChunk x = 0; x < CHUNK_SIZE_XZ; x++)
	{
		for(BlockInChunk z = 0; z < CHUNK_SIZE_XZ; z++)
		{
			y = CHUNK_SIZE_Y - 1;
			while (y >= 0)
			{
				index = GetIndexByPosition(x, y, z);
				if(bBlocks[index].cMaterial != MAT_NO)
					break;

				SkyLight[index] = bright;
				y--;
			}
		}
	}
}