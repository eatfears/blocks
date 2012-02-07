#include <process.h>

#include "World.h"
#include "Blocks_Definitions.h"
#include "Landscape.h"

void LoadChunkThread(void* pParams)
{
	Param pParameters = *(Param*)pParams;
	ChunkInWorld x = pParameters.x;
	ChunkInWorld z = pParameters.z;
	World &wWorld = *pParameters.wWorld;

	SetEvent(wWorld.parget);
	WaitForSingleObject(wWorld.semaphore, INFINITE);

#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

	if(wWorld.GetChunkByBlock(x*CHUNK_SIZE_XZ, z*CHUNK_SIZE_XZ))
	{
		ReleaseSemaphore(wWorld.semaphore, 1, NULL);
		_endthread();
	}

	DWORD dwWaitResult; 
	//auto loc = wWorld.AddLocation(x,z);

	Chunk *loc = new Chunk(x, z, wWorld);

	loc->Generate();

	loc->DrawLoadedBlocks();
	loc->FillSkyLight(15);

	dwWaitResult = WaitForSingleObject(wWorld.mutex, INFINITE);
	
	unsigned long bin = wWorld.Hash(x, z);
	wWorld.Chunks[bin].push_front(loc);
	ReleaseMutex(wWorld.mutex);

	wWorld.DrawLoadedBlocksFinish(*loc);
	loc->NeedToRender[0] = 1;
	loc->NeedToRender[1] = 1;

	// 	dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
	// 	LocationPosiion lp = {x, z};
	// 	auto locc = wWorld.LoadedLocations.begin();
	// 	while (locc != wWorld.LoadedLocations.end())
	// 	{
	// 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
	// 		++loc;
	// 	}
	// 	if (locc == wWorld.LoadedLocations.end()) 
	// 	{
	// 		ReleaseMutex(wWorld.loading_mutex);
	// 		_endthread();
	// 		return;
	// 	}
	// 	wWorld.LoadedLocations.erase(locc);
	// 	ReleaseMutex(wWorld.loading_mutex);

	ReleaseSemaphore(wWorld.semaphore, 1, NULL);

	_endthread();
}

void UnLoadChunkThread(void* pParams)
{
	Param pParameters = *(Param*)pParams;
	ChunkInWorld x = pParameters.x;
	ChunkInWorld z = pParameters.z;
	World &wWorld = *pParameters.wWorld;

	SetEvent(wWorld.parget);
	WaitForSingleObject(wWorld.semaphore, INFINITE);

#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

	DWORD dwWaitResult; 

	dwWaitResult = WaitForSingleObject(wWorld.mutex, INFINITE);

	unsigned long bin = wWorld.Hash(x, z);
	auto loc = wWorld.Chunks[bin].begin();

	while(loc != wWorld.Chunks[bin].end())
	{
		if(((*loc)->x == x)&&((*loc)->z == z)) break;
		++loc;
	}
	if(loc == wWorld.Chunks[bin].end()) 
	{	
		ReleaseMutex(wWorld.mutex);
		ReleaseSemaphore(wWorld.semaphore, 1, NULL);

		// 		dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
		// 		LocationPosiion lp = {x, z};
		// 		auto locc = wWorld.LoadedLocations.begin();
		// 		while (locc != wWorld.LoadedLocations.end())
		// 		{
		// 			if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
		// 			++loc;
		// 		}
		// 		if (locc == wWorld.LoadedLocations.end())
		// 		{
		// 			ReleaseMutex(wWorld.loading_mutex);
		// 			_endthread(); 
		// 			return;
		// 		}
		// 		wWorld.LoadedLocations.erase(locc);
		// 		ReleaseMutex(wWorld.loading_mutex);
		_endthread(); 
		return;
	}

	dwWaitResult = WaitForSingleObject((*loc)->mutex, INFINITE);

	delete *loc;
	wWorld.Chunks[bin].erase(loc);

	wWorld.DrawUnLoadedBlocks(x, z);
	ReleaseMutex(wWorld.mutex);

	// 
	// 	dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
	// 	LocationPosiion lp = {x, z};
	// 	auto locc = wWorld.LoadedLocations.begin();
	// 	while (locc != wWorld.LoadedLocations.end())
	// 	{
	// 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
	// 		++loc;
	// 	}
	// 	if (locc == wWorld.LoadedLocations.end())
	// 	{
	// 		ReleaseMutex(wWorld.loading_mutex);
	// 		_endthread(); 
	// 		return;
	// 	}
	// 	wWorld.LoadedLocations.erase(locc);
	// 	ReleaseMutex(wWorld.loading_mutex);
	ReleaseSemaphore(wWorld.semaphore, 1, NULL);
	_endthread();
}

void LoadNGenerate(void* pParams)
{
	Param pParameters = *(Param*)pParams;
	World &wWorld = *pParameters.wWorld;
	ChunkInWorld x = pParameters.x;
	ChunkInWorld z = pParameters.z;
	SetEvent(wWorld.parget2);

	int size = 16;

	for(int i = x*size; i < (x+1)*size*1; i++)
	{
		for(int j = z*size; j < (z+1)*size; j++)
		{
			wWorld.LoadChunk(i, j);
			//Sleep(30);
		}
	}
	_endthread();
	return;
}