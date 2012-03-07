#include <process.h>

#include "Blocks_Definitions.h"
#include "World.h"
#include "Threads.h"
#include "Light.h"
#include <math.h>

World::World()
	:player(*this)
{
	Chunks = new std::list<Chunk *>[HASH_SIZE];

	parget = CreateEvent(NULL, false, false, NULL);
	parget2 = CreateEvent(NULL, false, false, NULL);
	mutex = CreateMutex(NULL, false, NULL);
	semaphore = CreateSemaphore(NULL, 4, 4, NULL);
	SoftLight = true;

	SkyBright = 1.0f;
	LightToRefresh = true;
}

World::~World()
{
	for(int i = 0; i < HASH_SIZE; i++)
		Chunks[i].clear();

	delete[] Chunks;
}

void World::BuildWorld()
{
	Param par = {0, 0, this};

	_beginthread(LoadNGenerate, 0, &par);

	WaitForSingleObject(parget2, INFINITE);
	ResetEvent(parget2);
	/**/
	/*
	for(int i = -10; i < 10; i++)
	for(int j = -10; j < 10; j++)
	{
		AddLocation(i,j);
	}
	//AddLocation(0,0);

	for(int j = 0; j < 20; j++)
	{
		for(int i = -60; i < 60; i++)
		{
			for(int k = -60; k < 60; k++)
			{
				AddTile(i, j, k, MAT_GRASS, true);
				//AddTile(i, j, k, rand()%4+1, true);
			}
		}
	}
	/**/
	/*
	for(int k = 20; k > 0; k--)
	{
		//AddTile(i, j, k, MAT_GRASS, true);
		//if(rand()%2)AddTile(0, k, 0, rand()%4+1, true);
	}

	//AddTile(0,3,0,MAT_GRASS, true);
	//AddTile(0,1,0,MAT_DIRT, true);
	//AddTile(0,0,0,MAT_STONE, true);
	/**/

	/*
	AddTile(10,0,0,MAT_DIRT);
	AddTile(0,1,0,MAT_DIRT);
	AddTile(0,-3,0,MAT_STONE);
	AddTile(1,0,0,MAT_STONE);
	AddTile(-1,0,0,MAT_STONE);
	/**/
	/*
	for(int i = 0 ; i< 40 ; i++)
	{
		for(int k = 0 ; k < 1 ; k++)
		{
			AddTile(rand()%100-50,k,rand()%100-50,MAT_STONE);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_SAND);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_DIRT);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_GRASS);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_NO);
		}
	}
	/**/
	/*
	AddTile(1,-1,0,MAT_DIRT);
	AddTile(2,-1,0,MAT_STONE);
	AddTile(3,-1,0,MAT_STONE);
	AddTile(4,-1,0,MAT_DIRT);
	/**/
}

void World::GetChunkByBlock(BlockInWorld x, BlockInWorld z, ChunkInWorld *Cx, ChunkInWorld *Cz)
{
	*Cx = (ChunkInWorld) floor((double)x/CHUNK_SIZE_XZ);
	*Cz = (ChunkInWorld) floor((double)z/CHUNK_SIZE_XZ);
}

void World::GetPosInChunkByWorld(BlockInWorld x, BlockInWorld y, BlockInWorld z, BlockInChunk *chnkx, BlockInChunk *chnky, BlockInChunk *chnkz)
{
	while(x < 0) x += CHUNK_SIZE_XZ;
	while(z < 0) z += CHUNK_SIZE_XZ;
	*chnkx = x%CHUNK_SIZE_XZ;
	*chnky = y;
	*chnkz = z%CHUNK_SIZE_XZ;
}

Chunk* World::GetChunkByBlock(BlockInWorld x, BlockInWorld z)
{
	ChunkInWorld Cx, Cz;
	GetChunkByBlock(x, z, &Cx, &Cz);

	return GetChunkByPosition(Cx, Cz);
}

Chunk* World::GetChunkByPosition(ChunkInWorld Cx, ChunkInWorld Cz)
{
	unsigned long bin = Hash(Cx, Cz);
	auto chunk = Chunks[bin].begin();

	while(chunk != Chunks[bin].end())
	{
		if(((*chunk)->x == Cx)&&((*chunk)->z == Cz)) break;
		++chunk;
	}
	if(chunk == Chunks[bin].end())
		return NULL;

	return *chunk;
}

void World::DrawLoadedBlocksFinish(Chunk &chunk)
{
	int index = 0;

 	DWORD dwWaitResult;
 	dwWaitResult = WaitForSingleObject(chunk.mutex, INFINITE);
	BlockInChunk chnkx, chnky, chnkz;
	BlockInWorld x, y, z;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		chunk.GetBlockPositionByPointer(chunk.bBlocks + index, &chnkx, &chnky, &chnkz);

		if((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
		{
			index++;
			continue;
		}

		x = chnkx + CHUNK_SIZE_XZ*chunk.x;
		y = chnky;
		z = chnkz + CHUNK_SIZE_XZ*chunk.z;

		if(chunk.bBlocks[index].cMaterial != MAT_NO)
		{
			Chunk *TempChunk;
			int TempIndex;

			if(chunk.bBlocks[index].cMaterial == MAT_WATER)
			{
				if(!FindBlock(x, y + 1, z, &TempChunk, &TempIndex)) ShowTile(&chunk, index, TOP);
				if(!FindBlock(x, y - 1, z, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(&chunk, index, BOTTOM);}
				if(!FindBlock(x + 1, y, z, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(&chunk, index, RIGHT);}
				if(!FindBlock(x - 1, y, z, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(&chunk, index, LEFT);}
				if(!FindBlock(x, y, z + 1, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(&chunk, index, BACK);}
				if(!FindBlock(x, y, z - 1, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(&chunk, index, FRONT);}
			}
			else
			{
				if(!FindBlock(x, y + 1, z, &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER))
					ShowTile(&chunk, index, TOP);
				if(!FindBlock(x, y - 1, z, &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, BOTTOM);}
				if(!FindBlock(x + 1, y, z, &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, RIGHT);}
				if(!FindBlock(x - 1, y, z, &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, LEFT);}
				if(!FindBlock(x, y, z + 1, &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, BACK);}
				if(!FindBlock(x, y, z - 1, &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
					if(TempChunk) ShowTile(&chunk, index, FRONT);}
			}
		}
		index++;
	}
	/**/
	ReleaseMutex(chunk.mutex);

	// draw boundary tiles
	index = 0;
	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		chunk.GetBlockPositionByPointer(chunk.bBlocks + index, &chnkx, &chnky, &chnkz);

		if((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
		{
			index++;
			continue;
		}

		x = chnkx + CHUNK_SIZE_XZ*chunk.x;
		y = chnky;
		z = chnkz + CHUNK_SIZE_XZ*chunk.z;

		if((chunk.bBlocks[index].cMaterial == MAT_NO)||(chunk.bBlocks[index].cMaterial == MAT_WATER))
		{
			Chunk *tempLoc;
			Chunk *tCurLoc = &chunk;
			int tempIndex;
			if(FindBlock(x + 1, y, z, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, LEFT);
			if(FindBlock(x - 1, y, z, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, RIGHT);
			if(FindBlock(x, y, z + 1, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, FRONT);
			if(FindBlock(x, y, z - 1, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, BACK);
		}
		index++;
	}
}

void World::DrawUnLoadedBlocks(ChunkInWorld Cx, ChunkInWorld Cz)
{
	int index = 0;

	BlockInChunk chnkx, chnky, chnkz;
	BlockInWorld x, y, z;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		Chunk::GetBlockPositionByIndex(index, &chnkx, &chnky, &chnkz);

		if((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
		{
			index++;
			continue;
		}

		x = chnkx + CHUNK_SIZE_XZ*Cx;
		y = chnky;
		z = chnkz + CHUNK_SIZE_XZ*Cz;

		Chunk *TempChunk;
		int TempIndex;
		if(FindBlock(x + 1, y, z, &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, LEFT);
		if(FindBlock(x - 1, y, z, &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, RIGHT);
		if(FindBlock(x, y, z + 1, &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, FRONT);
		if(FindBlock(x, y, z - 1, &TempChunk, &TempIndex)) HideTile(&*TempChunk, TempIndex, BACK);
		index++;
	}
}

int World::AddBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, char mat, bool show)
{
	if((y < 0)||(y >= CHUNK_SIZE_Y)) return 0;

	if(FindBlock(x, y, z)) return 0;

	Chunk *chunk = GetChunkByBlock(x, z);

	if(chunk == NULL) return 0;

	BlockInChunk chnkx, chnky, chnkz;

	GetPosInChunkByWorld(x, y, z, &chnkx, &chnky, &chnkz);

	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);
	int index = chunk->AddBlock(chnkx, chnky, chnkz, mat);
	ReleaseMutex(chunk->mutex);

	Chunk *TempChunk = 0;
	int TempIndex;

	if(chunk->bBlocks[index].cMaterial == MAT_WATER)
	{
		if(!FindBlock(x, y + 1, z, &TempChunk, &TempIndex)) ShowTile(chunk, index, TOP);
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, BOTTOM);
		if(!FindBlock(x, y - 1, z, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, BOTTOM);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, TOP);
		if(!FindBlock(x + 1, y, z, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, RIGHT);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, LEFT);
		if(!FindBlock(x - 1, y, z, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, LEFT);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, RIGHT);
		if(!FindBlock(x, y, z + 1, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, BACK);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, FRONT);
		if(!FindBlock(x, y, z - 1, &TempChunk, &TempIndex)) {if(TempChunk) ShowTile(chunk, index, FRONT);}
		else if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(TempChunk, TempIndex, BACK);
	}
	else
	{
		if(!FindBlock(x, y + 1, z, &TempChunk, &TempIndex))
			ShowTile(chunk, index, TOP);
		else {HideTile(TempChunk, TempIndex, BOTTOM); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) ShowTile(chunk, index, TOP);}
		if(!FindBlock(x, y - 1, z, &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, BOTTOM);}
		else {if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, BOTTOM); else HideTile(TempChunk, TempIndex, TOP); }
		if(!FindBlock(x + 1, y, z, &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, RIGHT);}
		else {HideTile(TempChunk, TempIndex, LEFT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, RIGHT);}
		if(!FindBlock(x - 1, y, z, &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, LEFT);}
		else {HideTile(TempChunk, TempIndex, RIGHT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, LEFT);}
		if(!FindBlock(x, y, z + 1, &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, BACK);}
		else {HideTile(TempChunk, TempIndex, FRONT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, BACK);}
		if(!FindBlock(x, y, z - 1, &TempChunk, &TempIndex)) {
			if(TempChunk) ShowTile(chunk, index, FRONT);}
		else {HideTile(TempChunk, TempIndex, BACK); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) ShowTile(chunk, index, FRONT);}
	}

	return 1;
}

int World::RemoveBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, bool show)
{
	if((y < 0)||(y >= CHUNK_SIZE_Y)) return 0;

	if(!FindBlock(x, y, z)) return 0;

	Chunk *chunk = GetChunkByBlock(x, z);
	int index;

	if(chunk == NULL) return 0;

	BlockInChunk chnkx, chnky, chnkz;

	GetPosInChunkByWorld(x, y, z, &chnkx, &chnky, &chnkz);

	index = chunk->GetIndexByPosition(chnkx, chnky, chnkz);

	Chunk *TempChunk = 0;
	int TempIndex;

	if(!FindBlock(x, y + 1, z, &TempChunk, &TempIndex)||chunk->bBlocks[index].cMaterial == MAT_WATER) HideTile(chunk, index, TOP);
	else {ShowTile(TempChunk, TempIndex, BOTTOM); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, TOP);}
	if(!FindBlock(x, y - 1, z, &TempChunk, &TempIndex)) HideTile(chunk, index, BOTTOM);
	else {ShowTile(TempChunk, TempIndex, TOP); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, BOTTOM);}
	if(!FindBlock(x + 1, y, z, &TempChunk, &TempIndex)) HideTile(chunk, index, RIGHT);
	else {ShowTile(TempChunk, TempIndex, LEFT); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, RIGHT);}
	if(!FindBlock(x - 1, y, z, &TempChunk, &TempIndex)) HideTile(chunk, index, LEFT);
	else {ShowTile(TempChunk, TempIndex, RIGHT); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, LEFT);}
	if(!FindBlock(x, y, z + 1, &TempChunk, &TempIndex)) HideTile(chunk, index, BACK);
	else {ShowTile(TempChunk, TempIndex, FRONT); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, BACK);}
	if(!FindBlock(x, y, z - 1, &TempChunk, &TempIndex)) HideTile(chunk, index, FRONT);
	else {ShowTile(TempChunk, TempIndex, BACK); if(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) HideTile(chunk, index, FRONT);}

	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);
	chunk->RemoveBlock(chnkx, chnky, chnkz);
	ReleaseMutex(chunk->mutex);

	return 1;
}

void World::ShowTile(Chunk *chunk, int index, char side)
{
	if(!(chunk->bBlocks[index].bVisible & (1 << side)))
	{
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);

		chunk->ShowTile(chunk->bBlocks + index, side);

		chunk->NeedToRender[0] = RENDER_NEED;
		chunk->NeedToRender[1] = RENDER_NEED;
		ReleaseMutex(chunk->mutex);
	}
}

void World::HideTile(Chunk *chunk, int index, char side)
{
	if(chunk->bBlocks[index].bVisible & (1 << side))
	{
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);

		chunk->HideTile(chunk->bBlocks + index, side);

		chunk->NeedToRender[0] = RENDER_NEED;
		chunk->NeedToRender[1] = RENDER_NEED;
		ReleaseMutex(chunk->mutex);
	}
}

int World::FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, Chunk **chunk, int *index)
{
	if((y < 0)||(y >= CHUNK_SIZE_Y)) { *chunk = NULL; *index = 0; return 0;}

	(*chunk) = GetChunkByBlock(x, z);

	if((*chunk) == NULL) { *index = 0; return 0;}

	BlockInChunk chnkx, chnky, chnkz;

	GetPosInChunkByWorld(x, y, z, &chnkx, &chnky, &chnkz);

	*index = chnkx*CHUNK_SIZE_XZ + chnkz + chnky*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;

	if((*chunk)->GetBlockMaterial(chnkx, chnky, chnkz) == MAT_NO)
		return 0;

	return 1;
}

int World::FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z)
{
	if(y < 0) return 0;

	Chunk *chunk = GetChunkByBlock(x, z);

	if(chunk == NULL) return 0;

	BlockInChunk chnkx, chnky, chnkz;

	GetPosInChunkByWorld(x, y, z, &chnkx, &chnky, &chnkz);

	if(chunk->GetBlockMaterial(chnkx, chnky, chnkz) == MAT_NO)
		return 0;

	return 1;
}

void World::LoadChunk(ChunkInWorld x, ChunkInWorld z)
{
// 	WaitForSingleObject(loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
//
// 	auto chunk = LoadedLocations.begin();
//
// 	while (chunk != LoadedLocations.end())
// 	{
// 		if (((*chunk).x == lp.x)&&((*chunk).z == lp.z))
// 		{
// 			ReleaseMutex(loading_mutex);
// 			return;
// 		}
// 		++chunk;
// 	}
// 	LoadedLocations.push_back(lp);
// 	ReleaseMutex(loading_mutex);


	Param par = {x, z, this};

	HANDLE hThread;
	hThread = (HANDLE) _beginthread(LoadChunkThread, 0, &par);//&Param(par));

	//WaitForSingleObject(hThread, 30);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
}

void World::UnLoadChunk(ChunkInWorld x, ChunkInWorld z)
{
// 	WaitForSingleObject(loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
//
// 	auto chunk = LoadedLocations.begin();
//
// 	while (chunk != LoadedLocations.end())
// 	{
// 		if (((*chunk).x == lp.x)&&((*chunk).z == lp.z))
// 		{
// 			ReleaseMutex(loading_mutex);
// 			return;
// 		}
// 		++chunk;
// 	}
// 	LoadedLocations.push_back(lp);
// 	ReleaseMutex(loading_mutex);

	Param par = {x, z, this};

	HANDLE hThread;

	hThread = (HANDLE) _beginthread(UnLoadChunkThread, 0, &par);//&Param(par));

	//WaitForSingleObject(hThread, 30);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
}

void World::UpdateLight(Chunk& chunk)
{
	Chunk *ChunkArray[5][5];
	ChunkArray[2][2] = &chunk;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if((i!=2)||(j!=2))
			{
				if((i == 0)&&(j == 0) || (i == 4)&&(j == 0) || (i == 0)&&(j == 4) || (i == 4)&&(j == 4))
					ChunkArray[i][j] = NULL;
				else
					ChunkArray[i][j] = GetChunkByPosition((chunk.x + i - 2), (chunk.z + j - 2));
			}
		}
	}

	Light lightfill(ChunkArray, true);
	lightfill.UpdateLight();
	lightfill.skylight = false;
	lightfill.UpdateLight();

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if(ChunkArray[i][j])
			{
				if(ChunkArray[i][j]->NeedToRender[0] != RENDER_NEED)
					ChunkArray[i][j]->NeedToRender[0] = RENDER_MAYBE;
				if(ChunkArray[i][j]->NeedToRender[1] != RENDER_NEED)
					ChunkArray[i][j]->NeedToRender[1] = RENDER_MAYBE;
			}
		}
	}
}

void World::SaveChunks()
{
	for(int i = 0; i < HASH_SIZE; i++)
	{
		auto chunk = Chunks[i].begin();

		while(chunk != Chunks[i].end())
		{
			(*chunk)->Save();
			++chunk;
		}
	}
}
