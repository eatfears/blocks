#include "world.h"

#include <math.h>

#include "common_include/definitions.h"
#include "threads.h"
#include "light.h"

World::World()
    : m_Player(*this)
{
    m_Chunks = new std::list<Chunk *>[HASH_SIZE];

    //	parget = CreateEvent(NULL, false, false, NULL);
    //	parget2 = CreateEvent(NULL, false, false, NULL);
    //	mutex = CreateMutex(NULL, false, NULL);
    //	semaphore = CreateSemaphore(NULL, 4, 4, NULL);
    SoftLight = true;

    SkyBright = 1.0f;
    LightToRefresh = true;
}

World::~World()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        m_Chunks[i].clear();
    }
    delete[] m_Chunks;
}

void World::buildWorld()
{
    Param par = {0, 0, this};
    LoadNGenerate(&par);
    //	_beginthread(LoadNGenerate, 0, &par);

    //	WaitForSingleObject(parget2, INFINITE);
    //	ResetEvent(parget2);
}

Chunk* World::getChunkByPosition(ChunkCoord Cx, ChunkCoord Cz) const
{
    unsigned long bin = hash(Cx, Cz);
    auto chunk = m_Chunks[bin].begin();

    while (chunk != m_Chunks[bin].end())
    {
        if (((*chunk)->x == Cx)&&((*chunk)->z == Cz)) break;
        ++chunk;
    }
    if (chunk == m_Chunks[bin].end())
    {
        return NULL;
    }

    return *chunk;
}
// todo: loaded block under water hides water tile
// todo: rewrite tiling engine!
void World::drawLoadedBlocksFinish(Chunk &chunk)
{
    int index = 0;
    // 	DWORD dwWaitResult;
    // 	dwWaitResult = WaitForSingleObject(chunk.mutex, INFINITE);
    BlockCoord chnkx, chnky, chnkz;
    BlockInWorld pos;

    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        chunk.getBlockPositionByPointer(chunk.bBlocks + index, &chnkx, &chnky, &chnkz);

        if ((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
        {
            index++;
            continue;
        }

        BlockInWorld pos(chunk.x, chunk.z, chnkx, chnky, chnkz);

        if (chunk.bBlocks[index].cMaterial != MAT_NO)
        {
            Chunk *TempChunk;
            int TempIndex;

            if (chunk.bBlocks[index].cMaterial == MAT_WATER)
            {
                if (!findBlock(pos.getSide(TOP), &TempChunk, &TempIndex)) showTile(&chunk, index, TOP);
                if (!findBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)) { if (TempChunk) showTile(&chunk, index, BOTTOM); }
                if (!findBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) { if (TempChunk) showTile(&chunk, index, RIGHT); }
                if (!findBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) { if (TempChunk) showTile(&chunk, index, LEFT); }
                if (!findBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) { if (TempChunk) showTile(&chunk, index, BACK); }
                if (!findBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) { if (TempChunk) showTile(&chunk, index, FRONT); }
            }
            else
            {
                if (!findBlock(pos.getSide(TOP), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
                    showTile(&chunk, index, TOP);}
                if (!findBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
                    if (TempChunk) showTile(&chunk, index, BOTTOM);}
                if (!findBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
                    if (TempChunk) showTile(&chunk, index, RIGHT);}
                if (!findBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
                    if (TempChunk) showTile(&chunk, index, LEFT);}
                if (!findBlock(pos.getSide(BACK), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
                    if (TempChunk) showTile(&chunk, index, BACK);}
                if (!findBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)||(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) {
                    if (TempChunk) showTile(&chunk, index, FRONT);}
            }
        }
        index++;
    }
    //	ReleaseMutex(chunk.mutex);

    // draw boundary tiles
    index = 0;
    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        chunk.getBlockPositionByPointer(chunk.bBlocks + index, &chnkx, &chnky, &chnkz);

        if ((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
        {
            index++;
            continue;
        }

        BlockInWorld pos(chunk.x, chunk.z, chnkx, chnky, chnkz);

        if ((chunk.bBlocks[index].cMaterial == MAT_NO)||(chunk.bBlocks[index].cMaterial == MAT_WATER))
        {
            Chunk *tempLoc;
            Chunk *tCurLoc = &chunk;
            int tempIndex;
            if (findBlock(pos.getSide(RIGHT), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if (tempLoc->bBlocks != tCurLoc->bBlocks)
                showTile(&*tempLoc, tempIndex, LEFT);
            if (findBlock(pos.getSide(LEFT), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if (tempLoc->bBlocks != tCurLoc->bBlocks)
                showTile(&*tempLoc, tempIndex, RIGHT);
            if (findBlock(pos.getSide(BACK), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if (tempLoc->bBlocks != tCurLoc->bBlocks)
                showTile(&*tempLoc, tempIndex, FRONT);
            if (findBlock(pos.getSide(FRONT), &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if (tempLoc->bBlocks != tCurLoc->bBlocks)
                showTile(&*tempLoc, tempIndex, BACK);
        }
        index++;
    }
}

void World::drawUnLoadedBlocks(ChunkCoord Cx, ChunkCoord Cz)
{
    int index = 0;

    BlockCoord chnkx, chnky, chnkz;

    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        Chunk::getBlockPositionByIndex(index, &chnkx, &chnky, &chnkz);

        if ((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
        {
            index++;
            continue;
        }

        BlockInWorld pos(Cx, Cz, chnkx, chnky, chnkz);

        Chunk *TempChunk;
        int TempIndex;
        if (findBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) hideTile(&*TempChunk, TempIndex, LEFT);
        if (findBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) hideTile(&*TempChunk, TempIndex, RIGHT);
        if (findBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) hideTile(&*TempChunk, TempIndex, FRONT);
        if (findBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) hideTile(&*TempChunk, TempIndex, BACK);
        index++;
    }
}

int World::addBlock(const BlockInWorld &pos, char mat, bool show)
{
    if ((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) return 0;
    if (findBlock(pos)) return 0;

    Chunk *chunk = getChunkByPosition(pos.cx, pos.cz);

    if (chunk == NULL) return 0;

    //	DWORD dwWaitResult;
    //	dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);
    int index = chunk->addBlock(pos.bx, pos.by, pos.bz, mat);
    //	ReleaseMutex(chunk->mutex);

    Chunk *TempChunk = 0;
    int TempIndex;

    if (chunk->bBlocks[index].cMaterial == MAT_WATER)
    {
        if (!findBlock(pos.getSide(TOP), &TempChunk, &TempIndex)) showTile(chunk, index, TOP);
        else if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(TempChunk, TempIndex, BOTTOM);
        if (!findBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)) {if (TempChunk) showTile(chunk, index, BOTTOM);}
        else if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(TempChunk, TempIndex, TOP);
        if (!findBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) {if (TempChunk) showTile(chunk, index, RIGHT);}
        else if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(TempChunk, TempIndex, LEFT);
        if (!findBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) {if (TempChunk) showTile(chunk, index, LEFT);}
        else if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(TempChunk, TempIndex, RIGHT);
        if (!findBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) {if (TempChunk) showTile(chunk, index, BACK);}
        else if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(TempChunk, TempIndex, FRONT);
        if (!findBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) {if (TempChunk) showTile(chunk, index, FRONT);}
        else if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(TempChunk, TempIndex, BACK);
    }
    else
    {
        if (!findBlock(pos.getSide(TOP), &TempChunk, &TempIndex))
            showTile(chunk, index, TOP);
        else {hideTile(TempChunk, TempIndex, BOTTOM); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) showTile(chunk, index, TOP);}
        if (!findBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)) {
            if (TempChunk) showTile(chunk, index, BOTTOM);}
        else {if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) showTile(chunk, index, BOTTOM); else hideTile(TempChunk, TempIndex, TOP); }
        if (!findBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) {
            if (TempChunk) showTile(chunk, index, RIGHT);}
        else {hideTile(TempChunk, TempIndex, LEFT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) showTile(chunk, index, RIGHT);}
        if (!findBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) {
            if (TempChunk) showTile(chunk, index, LEFT);}
        else {hideTile(TempChunk, TempIndex, RIGHT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) showTile(chunk, index, LEFT);}
        if (!findBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) {
            if (TempChunk) showTile(chunk, index, BACK);}
        else {hideTile(TempChunk, TempIndex, FRONT); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) showTile(chunk, index, BACK);}
        if (!findBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) {
            if (TempChunk) showTile(chunk, index, FRONT);}
        else {hideTile(TempChunk, TempIndex, BACK); if ((TempChunk)&&(TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER)) showTile(chunk, index, FRONT);}
    }

    return 1;
}

int World::removeBlock(const BlockInWorld &pos, bool show)
{
    if ((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) return 0;
    if (!findBlock(pos)) return 0;

    Chunk *chunk = getChunkByPosition(pos.cx, pos.cz);
    int index;

    if (chunk == NULL) return 0;

    index = chunk->getIndexByPosition(pos.bx, pos.by, pos.bz);
    Chunk *TempChunk = 0;
    int TempIndex;

    if (!findBlock(pos.getSide(TOP), &TempChunk, &TempIndex)||chunk->bBlocks[index].cMaterial == MAT_WATER) hideTile(chunk, index, TOP);
    else {showTile(TempChunk, TempIndex, BOTTOM); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(chunk, index, TOP);}
    if (!findBlock(pos.getSide(BOTTOM), &TempChunk, &TempIndex)) hideTile(chunk, index, BOTTOM);
    else {showTile(TempChunk, TempIndex, TOP); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(chunk, index, BOTTOM);}
    if (!findBlock(pos.getSide(RIGHT), &TempChunk, &TempIndex)) hideTile(chunk, index, RIGHT);
    else {showTile(TempChunk, TempIndex, LEFT); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(chunk, index, RIGHT);}
    if (!findBlock(pos.getSide(LEFT), &TempChunk, &TempIndex)) hideTile(chunk, index, LEFT);
    else {showTile(TempChunk, TempIndex, RIGHT); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(chunk, index, LEFT);}
    if (!findBlock(pos.getSide(BACK), &TempChunk, &TempIndex)) hideTile(chunk, index, BACK);
    else {showTile(TempChunk, TempIndex, FRONT); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(chunk, index, BACK);}
    if (!findBlock(pos.getSide(FRONT), &TempChunk, &TempIndex)) hideTile(chunk, index, FRONT);
    else {showTile(TempChunk, TempIndex, BACK); if (TempChunk->bBlocks[TempIndex].cMaterial == MAT_WATER) hideTile(chunk, index, FRONT);}

    //	DWORD dwWaitResult;
    //	dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);
    chunk->removeBlock(pos.bx, pos.by, pos.bz);
    //	ReleaseMutex(chunk->mutex);

    return 1;
}

void World::showTile(Chunk *chunk, int index, char side)
{
    if (!(chunk->bBlocks[index].bVisible & (1 << side)))
    {
        //		DWORD dwWaitResult;
        //		dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);

        chunk->showTile(chunk->bBlocks + index, side);

        chunk->NeedToRender[0] = RENDER_NEED;
        chunk->NeedToRender[1] = RENDER_NEED;
        //		ReleaseMutex(chunk->mutex);
    }
}

void World::hideTile(Chunk *chunk, int index, char side)
{
    if (chunk->bBlocks[index].bVisible & (1 << side))
    {
        //		DWORD dwWaitResult;
        //		dwWaitResult = WaitForSingleObject(chunk->mutex, INFINITE);

        chunk->hideTile(chunk->bBlocks + index, side);

        chunk->NeedToRender[0] = RENDER_NEED;
        chunk->NeedToRender[1] = RENDER_NEED;
        //		ReleaseMutex(chunk->mutex);
    }
}

int World::findBlock(const BlockInWorld &pos, Chunk **chunk, int *index)
{
    if ((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) { *chunk = NULL; *index = 0; return 0;}
    (*chunk) = getChunkByPosition(pos.cx, pos.cz);
    if ((*chunk) == NULL) { *index = 0; return 0;}
    *index = pos.bx*CHUNK_SIZE_XZ + pos.bz + pos.by*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
    if ((*chunk)->getBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
        return 0;
    return 1;
}

int World::findBlock(const BlockInWorld &pos)
{
    if (pos.by < 0) return 0;
    Chunk *chunk = getChunkByPosition(pos.cx, pos.cz);
    if (chunk == NULL) return 0;
    BlockCoord chnkx, chnky, chnkz;
    if (chunk->getBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
        return 0;
    return 1;
}

void World::loadChunk(ChunkCoord x, ChunkCoord z)
{
    Param par = {x, z, this};
    //	HANDLE hThread;
    //	hThread = (HANDLE) _beginthread(LoadChunkThread, 0, &par);
    LoadChunkThread(&par);
    //	WaitForSingleObject(parget, INFINITE);
    //	ResetEvent(parget);
}

void World::unLoadChunk(ChunkCoord x, ChunkCoord z)
{
    Param par = {x, z, this};
    //	HANDLE hThread;
    //	hThread = (HANDLE) _beginthread(UnLoadChunkThread, 0, &par);
    //	WaitForSingleObject(parget, INFINITE);
    //	ResetEvent(parget);
}

void World::updateLight(Chunk& chunk) const
{
    Chunk *p_chunk_array[5][5];
    p_chunk_array[2][2] = &chunk;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if ((i!=2)||(j!=2))
            {
                if ((i == 0)&&(j == 0) || (i == 4)&&(j == 0) || (i == 0)&&(j == 4) || (i == 4)&&(j == 4))
                {
                    p_chunk_array[i][j] = NULL;
                }
                else
                {
                    p_chunk_array[i][j] = getChunkByPosition((chunk.x + i - 2), (chunk.z + j - 2));
                }
            }
        }
    }

    Light lightfill(p_chunk_array, true);
    lightfill.updateLight();
    lightfill.skylight = false;
    lightfill.updateLight();

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (p_chunk_array[i][j])
            {
                if (p_chunk_array[i][j]->NeedToRender[0] != RENDER_NEED)
                    p_chunk_array[i][j]->NeedToRender[0] = RENDER_MAYBE;
                if (p_chunk_array[i][j]->NeedToRender[1] != RENDER_NEED)
                    p_chunk_array[i][j]->NeedToRender[1] = RENDER_MAYBE;
            }
        }
    }
}

void World::saveChunks()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        auto chunk = m_Chunks[i].begin();
        while (chunk != m_Chunks[i].end())
        {
            (*chunk)->save();
            ++chunk;
        }
    }
}
