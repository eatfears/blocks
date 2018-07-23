#include "common_include/definitions.h"
#include "world.h"
#include "landscape.h"


void LoadChunkThread(void *pParams)
{
    Param parameters = *(Param*)pParams;
    World &world = *parameters.p_World;

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    Chunk *chunk = world.getChunkByPosition(parameters.pos);
    if(!chunk)
    {
        chunk = new Chunk(parameters.pos, world);
    }

    chunk->open();
    chunk->drawLoadedBlocks();

    //todo: leak if already exists
    world.m_Chunks[parameters.pos] = chunk;
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
    World &world = *parameters.p_World;

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif // _WIN32

    auto it = world.m_Chunks.find(parameters.pos);
    if (it != world.m_Chunks.end())
    {
        delete it->second;
        world.m_Chunks.erase(it);
    }
    world.drawUnLoadedBlocks(parameters.pos);
}

void LoadNGenerate(void *pParams)
{
    Param parameters = *(Param*)pParams;
    World &world = *parameters.p_World;
    ChunkCoord x = parameters.pos.cx;
    ChunkCoord z = parameters.pos.cz;

    int size = 4;

    for(int i = x*size; i < (x+1)*size*1; i++)
    {
        for(int j = z*size; j < (z+1)*size; j++)
        {
            world.loadChunk(i, j);
        }
    }
}
