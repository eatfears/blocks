#include <process.h>

#include "World.h"
#include "Blocks_Definitions.h"
#include "Landscape.h"
#include "Threads.h"

World::World()
{
	skipbuild = false;

	parget = CreateEvent(NULL, false, false, NULL);
	parget2 = CreateEvent(NULL, false, false, NULL);
	mutex = CreateMutex(NULL, false, NULL);
	semaphore = CreateSemaphore(NULL, 10, 10, NULL);
}

World::~World()
{
	Chunks.clear();
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

void World::GetChunkByBlock(BlockInWorld x, BlockInWorld z, ChunkInWorld *locx, ChunkInWorld *locz)
{
	*locx = (ChunkInWorld) floor((double)x/CHUNK_SIZE_XZ);
	*locz = (ChunkInWorld) floor((double)z/CHUNK_SIZE_XZ);
}

void World::GetPosInChunkByWorld(BlockInWorld x, BlockInWorld y, BlockInWorld z, BlockInChunk *locx, BlockInChunk *locy, BlockInChunk *locz)
{
	while(x < 0) x += CHUNK_SIZE_XZ;
	while(z < 0) z += CHUNK_SIZE_XZ;
	*locx = x%CHUNK_SIZE_XZ;
	*locy = y;
	*locz = z%CHUNK_SIZE_XZ;
}

Chunk* World::GetChunkByBlock(BlockInWorld x, BlockInWorld z)
{
	ChunkInWorld locx, locz;
	GetChunkByBlock(x, z, &locx, &locz);

	auto loc = Chunks.begin();

	while(loc != Chunks.end())
	{
		if((loc->x == locx)&&(loc->z == locz)) break;
		++loc;
	}
	if(loc == Chunks.end())
		return NULL;

	return &*loc;
}

void World::DrawLoadedBlocksFinish(Chunk &loc)
{
	int index = 0;

 	DWORD dwWaitResult; 
 	dwWaitResult = WaitForSingleObject(loc.mutex, INFINITE);
	BlockInChunk x, y, z;
	BlockInWorld xx, yy, zz;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		loc.GetBlockPositionByPointer(loc.bBlocks + index, &x, &y, &z);

		if((x > 0)&&(x < CHUNK_SIZE_XZ - 1)&&(z > 0)&&(z < CHUNK_SIZE_XZ - 1))
		{
			index++;
			continue;
		}

		xx = x + CHUNK_SIZE_XZ*loc.x;
		yy = y;
		zz = z + CHUNK_SIZE_XZ*loc.z;

		if(loc.bBlocks[index].cMaterial != MAT_NO)
		{
			Chunk *tempLoc;
			int tempIndex;

			if(loc.bBlocks[index].cMaterial == MAT_WATER)
			{
				if(!FindBlock(xx, yy + 1, zz, &tempLoc, &tempIndex)) ShowTile(&loc, index, TOP);
				if(!FindBlock(xx, yy - 1, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(&loc, index, BOTTOM);}
				if(!FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(&loc, index, RIGHT);}
				if(!FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(&loc, index, LEFT);}
				if(!FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(&loc, index, BACK);}
				if(!FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(&loc, index, FRONT);}
			}
			else
			{
				if(!FindBlock(xx, yy + 1, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) 
					ShowTile(&loc, index, TOP);
				if(!FindBlock(xx, yy - 1, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&loc, index, BOTTOM);}
				if(!FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&loc, index, RIGHT);}
				if(!FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&loc, index, LEFT);}
				if(!FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&loc, index, BACK);}
				if(!FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&loc, index, FRONT);}
			}
		}
		index++;
	}
	/**/
	ReleaseMutex(loc.mutex);
	
	// draw boundary tiles
	index = 0;
	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		loc.GetBlockPositionByPointer(loc.bBlocks + index, &x, &y, &z);
		
		if((x > 0)&&(x < CHUNK_SIZE_XZ - 1)&&(z > 0)&&(z < CHUNK_SIZE_XZ - 1))
		{
			index++;
			continue;
		}

		xx = x + CHUNK_SIZE_XZ*loc.x;
		yy = y;
		zz = z + CHUNK_SIZE_XZ*loc.z;

		if((loc.bBlocks[index].cMaterial == MAT_NO)||(loc.bBlocks[index].cMaterial == MAT_WATER))
		{
			Chunk *tempLoc;
			Chunk *tCurLoc = &loc;
			int tempIndex;
			if(FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, LEFT);
			if(FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, RIGHT);
			if(FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, FRONT);
			if(FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc->bBlocks != tCurLoc->bBlocks)
				ShowTile(&*tempLoc, tempIndex, BACK);
		}
		index++;
	}
}

void World::DrawUnLoadedBlocks(ChunkInWorld Lx, ChunkInWorld Lz)
{
	int index = 0;

	BlockInChunk x, y, z;
	BlockInWorld xx, yy, zz;
	
	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		Chunk::GetBlockPositionByIndex(index, &x, &y, &z);

		if((x > 0)&&(x < CHUNK_SIZE_XZ - 1)&&(z > 0)&&(z < CHUNK_SIZE_XZ - 1))
		{
			index++;
			continue;
		}

		xx = x + CHUNK_SIZE_XZ*Lx;
		yy = y;
		zz = z + CHUNK_SIZE_XZ*Lz;
		
		Chunk *tempLoc;
		int tempIndex;
		if(FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, LEFT);
		if(FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, RIGHT);
		if(FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, FRONT);
		if(FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, BACK);
		index++;
	}
}

int World::AddBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, char mat, bool show)
{
	if((y < 0)||(y >= CHUNK_SIZE_Y)) return 0;

	if(FindBlock(x, y, z)) return 0;

	Chunk *loc = GetChunkByBlock(x, z);
	int index; 

	if(loc == NULL) return 0;

	BlockInChunk locx, locy, locz;

	GetPosInChunkByWorld(x, y, z, &locx, &locy, &locz);

	
	if(show)
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);
		index = loc->AddBlock(locx, locy, locz, mat);
		ReleaseMutex(loc->mutex);

		Chunk *lTempLoc = 0;
		int iTempIndex;

		if(loc->bBlocks[index].cMaterial == MAT_WATER)
		{
			if(!FindBlock(x, y + 1, z, &lTempLoc, &iTempIndex)) ShowTile(loc, index, TOP);
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, BOTTOM);
			if(!FindBlock(x, y - 1, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, BOTTOM);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, TOP);
			if(!FindBlock(x + 1, y, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, RIGHT);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, LEFT);
			if(!FindBlock(x - 1, y, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, LEFT);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, RIGHT);
			if(!FindBlock(x, y, z + 1, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, BACK);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, FRONT);
			if(!FindBlock(x, y, z - 1, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, FRONT);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, BACK);
		}
		else
		{
			if(!FindBlock(x, y + 1, z, &lTempLoc, &iTempIndex)) 
				ShowTile(loc, index, TOP);
			else {HideTile(lTempLoc, iTempIndex, BOTTOM); if (lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) ShowTile(loc, index, TOP);}
			if(!FindBlock(x, y - 1, z, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, BOTTOM);}
			else {HideTile(lTempLoc, iTempIndex, TOP); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, BOTTOM);}
			if(!FindBlock(x + 1, y, z, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, RIGHT);}
			else {HideTile(lTempLoc, iTempIndex, LEFT); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, RIGHT);}
			if(!FindBlock(x - 1, y, z, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, LEFT);}
			else {HideTile(lTempLoc, iTempIndex, RIGHT); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, LEFT);}
			if(!FindBlock(x, y, z + 1, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, BACK);}
			else {HideTile(lTempLoc, iTempIndex, FRONT); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, BACK);}
			if(!FindBlock(x, y, z - 1, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, FRONT);}
			else {HideTile(lTempLoc, iTempIndex, BACK); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, FRONT);}
		}
	}
	else 
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);
		loc->AddBlock(locx, locy, locz, mat);
		ReleaseMutex(loc->mutex);
	}

	return 1;
}

int World::RemoveBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, bool show)
{
	if((y < 0)||(y >= CHUNK_SIZE_Y)) return 0;

	if(!FindBlock(x, y, z)) return 0;

	Chunk *loc = GetChunkByBlock(x, z);
	int index; 

	if(loc == NULL) return 0;

	BlockInChunk locx, locy, locz;

	GetPosInChunkByWorld(x, y, z, &locx, &locy, &locz);

	index = loc->GetIndexByPosition(locx, locy, locz);

	if(show)
	{
		Chunk *lTempLoc = 0;
		int iTempIndex;

		if(!FindBlock(x, y + 1, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, TOP);
		else {ShowTile(lTempLoc, iTempIndex, BOTTOM); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, TOP);}
		if(!FindBlock(x, y - 1, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, BOTTOM);
		else {ShowTile(lTempLoc, iTempIndex, TOP); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, BOTTOM);}
		if(!FindBlock(x + 1, y, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, RIGHT);
		else {ShowTile(lTempLoc, iTempIndex, LEFT); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, RIGHT);}
		if(!FindBlock(x - 1, y, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, LEFT);
		else {ShowTile(lTempLoc, iTempIndex, RIGHT); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, LEFT);}
		if(!FindBlock(x, y, z + 1, &lTempLoc, &iTempIndex)) HideTile(loc, index, BACK);
		else {ShowTile(lTempLoc, iTempIndex, FRONT); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, BACK);}
		if(!FindBlock(x, y, z - 1, &lTempLoc, &iTempIndex)) HideTile(loc, index, FRONT);
		else {ShowTile(lTempLoc, iTempIndex, BACK); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, FRONT);}

		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);
		loc->RemoveBlock(locx, locy, locz);
		ReleaseMutex(loc->mutex);
	}
	else
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);
		loc->RemoveBlock(locx, locy, locz);
		ReleaseMutex(loc->mutex);
	}

	return 1;
}

void World::ShowTile(Chunk *loc, int index, char N)
{
	if(!loc->bBlocks[index].bVisible[N])
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);

		loc->ShowTile(loc->bBlocks + index, N);
		ReleaseMutex(loc->mutex);
	}
}

void World::HideTile(Chunk *loc, int index, char N)
{
	if(loc->bBlocks[index].bVisible[N])
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);

		loc->HideTile(loc->bBlocks + index, N);
		ReleaseMutex(loc->mutex);
	}
}
 
int World::FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, Chunk **loc, int *index)
{
	if((y < 0)||(y >= CHUNK_SIZE_Y)) { *loc = NULL; *index = 0; return 0;}

	(*loc) = GetChunkByBlock(x, z);

	if((*loc) == NULL) { *index = 0; return 0;}

	BlockInChunk locx, locy, locz;

	GetPosInChunkByWorld(x, y, z, &locx, &locy, &locz);

	*index = locx*CHUNK_SIZE_XZ + locz + locy*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;

	if((*loc)->GetBlockMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}

int World::FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z)
{
	if(y < 0) return 0;

	Chunk *loc = GetChunkByBlock(x, z);

	if(loc == NULL) return 0;

	BlockInChunk locx, locy, locz;

	GetPosInChunkByWorld(x, y, z, &locx, &locy, &locz);

	if(loc->GetBlockMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}

void World::LoadChunk(ChunkInWorld x, ChunkInWorld z)
{
// 	WaitForSingleObject(loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
// 
// 	auto loc = LoadedLocations.begin();
// 
// 	while (loc != LoadedLocations.end())
// 	{
// 		if (((*loc).x == lp.x)&&((*loc).z == lp.z)) 
// 		{
// 			ReleaseMutex(loading_mutex);
// 			return;
// 		}		
// 		++loc;
// 	}
// 	LoadedLocations.push_back(lp);
// 	ReleaseMutex(loading_mutex);


	Param par = {x, z, this};

	HANDLE hThread;
	hThread = (HANDLE) _beginthread(LoadChunkThread, 0, &par);//&Param(par));

	//WaitForSingleObject(hThread, 30);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
	/*
	par.x++;
	if(par.x == 10)
	{
		par.x = 0;
		par.z++;
	}
	/**/
}

void World::UnLoadChunk(ChunkInWorld x, ChunkInWorld z)
{
// 	WaitForSingleObject(loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
// 
// 	auto loc = LoadedLocations.begin();
// 
// 	while (loc != LoadedLocations.end())
// 	{
// 		if (((*loc).x == lp.x)&&((*loc).z == lp.z))
// 		{
// 			ReleaseMutex(loading_mutex);
// 			return;
// 		}
// 		++loc;
// 	}
// 	LoadedLocations.push_back(lp);
// 	ReleaseMutex(loading_mutex);

	Param par = {x, z, this};

	HANDLE hThread;

	hThread = (HANDLE) _beginthread(UnLoadChunkThread, 0, &par);//&Param(par));

	//WaitForSingleObject(hThread, 30);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
	/*
	par.x++;
	if(par.x == 10)
	{
		par.x = 0;
		par.z++;
	}
	/**/
}
