#include "Definitions.h"
#include "World.h"
#include "Landscape.h"

void LoadChunkThread(void* pParams)
{
    Param pParameters = *(Param*)pParams;
    ChunkCoord x = pParameters.x;
    ChunkCoord z = pParameters.z;
    World &wWorld = *pParameters.wWorld;

    //	SetEvent(wWorld.parget);
    //	WaitForSingleObject(wWorld.semaphore, INFINITE);

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    if(wWorld.GetChunkByPosition(x, z)) {
        //		ReleaseSemaphore(wWorld.semaphore, 1, NULL);
        //		_endthread();
        //		return;
    }

    //	DWORD dwWaitResult;
    Chunk *chunk = new Chunk(x, z, wWorld);

    chunk->open();
    chunk->drawLoadedBlocks();

    //	dwWaitResult = WaitForSingleObject(wWorld.mutex, INFINITE);

    unsigned long bin = wWorld.Hash(x, z);

    wWorld.Chunks[bin].push_front(chunk);
    //	ReleaseMutex(wWorld.mutex);

    wWorld.DrawLoadedBlocksFinish(*chunk);

    if(chunk->m_LightToUpdate) {
        wWorld.UpdateLight(*chunk);
        chunk->m_LightToUpdate = false;
    }

    // 	dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
    // 	LocationPosiion lp = {x, z};
    // 	auto locc = wWorld.LoadedLocations.begin();
    // 	while (locc != wWorld.LoadedLocations.end()) {
    // 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
    // 		++chunk;
    // 	}
    // 	if (locc == wWorld.LoadedLocations.end()) {
    // 		ReleaseMutex(wWorld.loading_mutex);
    // 		_endthread();
    // 		return;
    // 	}
    // 	wWorld.LoadedLocations.erase(locc);
    // 	ReleaseMutex(wWorld.loading_mutex);

    //	ReleaseSemaphore(wWorld.semaphore, 1, NULL);

    //	_endthread();
    return;
}

void UnLoadChunkThread(void* pParams)
{
    Param pParameters = *(Param*)pParams;
    ChunkCoord x = pParameters.x;
    ChunkCoord z = pParameters.z;
    World &wWorld = *pParameters.wWorld;

    //	SetEvent(wWorld.parget);
    //	WaitForSingleObject(wWorld.semaphore, INFINITE);

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    //	DWORD dwWaitResult;

    //	dwWaitResult = WaitForSingleObject(wWorld.mutex, INFINITE);

    unsigned long bin = wWorld.Hash(x, z);
    auto chunk = wWorld.Chunks[bin].begin();

    while(chunk != wWorld.Chunks[bin].end()) {
        if(((*chunk)->x == x)&&((*chunk)->z == z)) break;
        ++chunk;
    }
    if(chunk == wWorld.Chunks[bin].end()) {/*
        ReleaseMutex(wWorld.mutex);
        ReleaseSemaphore(wWorld.semaphore, 1, NULL);*/

        // 		dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
        // 		LocationPosiion lp = {x, z};
        // 		auto locc = wWorld.LoadedLocations.begin();
        // 		while (locc != wWorld.LoadedLocations.end()) {
        // 			if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
        // 			++chunk;
        // 		}
        // 		if (locc == wWorld.LoadedLocations.end()) {
        // 			ReleaseMutex(wWorld.loading_mutex);
        // 			_endthread();
        // 			return;
        // 		}
        // 		wWorld.LoadedLocations.erase(locc);
        // 		ReleaseMutex(wWorld.loading_mutex);
        //		_endthread();
        return;
    }

    //	dwWaitResult = WaitForSingleObject((*chunk)->mutex, INFINITE);

    delete *chunk;
    wWorld.Chunks[bin].erase(chunk);

    wWorld.DrawUnLoadedBlocks(x, z);
    //	ReleaseMutex(wWorld.mutex);

    //
    // 	dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
    // 	LocationPosiion lp = {x, z};
    // 	auto locc = wWorld.LoadedLocations.begin();
    // 	while (locc != wWorld.LoadedLocations.end()) {
    // 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
    // 		++chunk;
    // 	}
    // 	if (locc == wWorld.LoadedLocations.end()) {
    // 		ReleaseMutex(wWorld.loading_mutex);
    // 		_endthread();
    // 		return;
    // 	}
    // 	wWorld.LoadedLocations.erase(locc);
    // 	ReleaseMutex(wWorld.loading_mutex);
    //	ReleaseSemaphore(wWorld.semaphore, 1, NULL);
    //	_endthread();
    return;
}

void LoadNGenerate(void* pParams)
{
    Param pParameters = *(Param*)pParams;
    World &wWorld = *pParameters.wWorld;
    ChunkCoord x = pParameters.x;
    ChunkCoord z = pParameters.z;
    //	SetEvent(wWorld.parget2);

    int size = 4;

    for(int i = x*size; i < (x+1)*size*1; i++) {
        for(int j = z*size; j < (z+1)*size; j++) {
            wWorld.LoadChunk(i, j);
            //Sleep(30);
        }
    }
    //	_endthread();
    return;
}
