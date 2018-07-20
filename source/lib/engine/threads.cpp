#include "common_include/definitions.h"
#include "world.h"
#include "landscape.h"


void LoadChunkThread(void *pParams)
{
    Param parameters = *(Param*)pParams;
    ChunkCoord x = parameters.x;
    ChunkCoord z = parameters.z;
    World &world = *parameters.pWorld;

    //	SetEvent(world.parget);
    //	WaitForSingleObject(world.semaphore, INFINITE);

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    Chunk *chunk = world.getChunkByPosition(x, z);
    if(!chunk)
    {
        chunk = new Chunk(x, z, world);
    }

    chunk->open();
    chunk->drawLoadedBlocks();

    //	dwWaitResult = WaitForSingleObject(world.mutex, INFINITE);

    unsigned long bin = world.hash(x, z);

    world.m_Chunks[bin].push_front(chunk);
    //	ReleaseMutex(world.mutex);

    world.drawLoadedBlocksFinish(*chunk);

    if(chunk->m_LightToUpdate)
    {
        world.updateLight(*chunk);
        chunk->m_LightToUpdate = false;
    }

    // 	dwWaitResult = WaitForSingleObject(world.loading_mutex, INFINITE);
    // 	LocationPosiion lp = {x, z};
    // 	auto locc = world.LoadedLocations.begin();
    // 	while (locc != world.LoadedLocations.end()) {
    // 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
    // 		++chunk;
    // 	}
    // 	if (locc == world.LoadedLocations.end()) {
    // 		ReleaseMutex(world.loading_mutex);
    // 		_endthread();
    // 		return;
    // 	}
    // 	world.LoadedLocations.erase(locc);
    // 	ReleaseMutex(world.loading_mutex);

    //	ReleaseSemaphore(world.semaphore, 1, NULL);

    //	_endthread();
    return;
}

void UnLoadChunkThread(void *pParams)
{
    Param parameters = *(Param*)pParams;
    ChunkCoord x = parameters.x;
    ChunkCoord z = parameters.z;
    World &world = *parameters.pWorld;

    //	SetEvent(world.parget);
    //	WaitForSingleObject(world.semaphore, INFINITE);

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    //	DWORD dwWaitResult;

    //	dwWaitResult = WaitForSingleObject(world.mutex, INFINITE);

    unsigned long bin = world.hash(x, z);
    auto chunk = world.m_Chunks[bin].begin();

    while(chunk != world.m_Chunks[bin].end())
    {
        if(((*chunk)->m_X == x)&&((*chunk)->m_Z == z)) break;
        ++chunk;
    }
    if(chunk == world.m_Chunks[bin].end()) {/*
            ReleaseMutex(world.mutex);
            ReleaseSemaphore(world.semaphore, 1, NULL);*/

        // 		dwWaitResult = WaitForSingleObject(world.loading_mutex, INFINITE);
        // 		LocationPosiion lp = {x, z};
        // 		auto locc = world.LoadedLocations.begin();
        // 		while (locc != world.LoadedLocations.end()) {
        // 			if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
        // 			++chunk;
        // 		}
        // 		if (locc == world.LoadedLocations.end()) {
        // 			ReleaseMutex(world.loading_mutex);
        // 			_endthread();
        // 			return;
        // 		}
        // 		world.LoadedLocations.erase(locc);
        // 		ReleaseMutex(world.loading_mutex);
        //		_endthread();
        return;
    }

    //	dwWaitResult = WaitForSingleObject((*chunk)->mutex, INFINITE);

    delete *chunk;
    world.m_Chunks[bin].erase(chunk);

    world.drawUnLoadedBlocks(x, z);
    //	ReleaseMutex(world.mutex);

    //
    // 	dwWaitResult = WaitForSingleObject(world.loading_mutex, INFINITE);
    // 	LocationPosiion lp = {x, z};
    // 	auto locc = world.LoadedLocations.begin();
    // 	while (locc != world.LoadedLocations.end()) {
    // 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
    // 		++chunk;
    // 	}
    // 	if (locc == world.LoadedLocations.end()) {
    // 		ReleaseMutex(world.loading_mutex);
    // 		_endthread();
    // 		return;
    // 	}
    // 	world.LoadedLocations.erase(locc);
    // 	ReleaseMutex(world.loading_mutex);
    //	ReleaseSemaphore(world.semaphore, 1, NULL);
    //	_endthread();
    return;
}

void LoadNGenerate(void *pParams)
{
    Param parameters = *(Param*)pParams;
    World &world = *parameters.pWorld;
    ChunkCoord x = parameters.x;
    ChunkCoord z = parameters.z;

    int size = 4;

    for(int i = x*size; i < (x+1)*size*1; i++)
    {
        for(int j = z*size; j < (z+1)*size; j++)
        {
            world.loadChunk(i, j);
        }
    }
    return;
}
