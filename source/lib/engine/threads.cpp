#include "common_include/definitions.h"
#include "world.h"
#include "landscape.h"


void LoadChunkThread(void *pParams)
{
    Param parameters = *(Param*)pParams;
    ChunkInWorld pos(parameters.x, parameters.z);
    ChunkCoord x = parameters.x;
    ChunkCoord z = parameters.z;
    World &world = *parameters.p_World;

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    Chunk *chunk = world.getChunkByPosition(x, z);
    if(!chunk)
    {
        chunk = new Chunk(pos, world);
    }

    chunk->open();
    chunk->drawLoadedBlocks();

    unsigned long bin = world.hash(x, z);

    world.m_Chunks[bin].push_front(chunk);
    world.drawLoadedBlocksFinish(*chunk);

    if(chunk->m_LightToUpdate)
    {
        world.updateLight(*chunk);
        chunk->m_LightToUpdate = false;
    }
}

void UnLoadChunkThread(void *pParams)
{
    Param parameters = *(Param*)pParams;
    ChunkCoord x = parameters.x;
    ChunkCoord z = parameters.z;
    World &world = *parameters.p_World;

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    unsigned long bin = world.hash(x, z);
    auto chunk = world.m_Chunks[bin].begin();

    while(chunk != world.m_Chunks[bin].end())
    {
        if(((*chunk)->m_X == x)&&((*chunk)->m_Z == z)) break;
        ++chunk;
    }

    delete *chunk;

    world.m_Chunks[bin].erase(chunk);
    world.drawUnLoadedBlocks(x, z);
}

void LoadNGenerate(void *pParams)
{
    Param parameters = *(Param*)pParams;
    World &world = *parameters.p_World;
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
}