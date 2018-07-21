#include "world.h"

#include <math.h>

#include "common_include/definitions.h"
#include "threads.h"
#include "light.h"

World::World()
    : m_Player(*this)
{
    m_Chunks = new std::list<Chunk*>[HASH_SIZE];

    //	parget = CreateEvent(NULL, false, false, NULL);
    //	parget2 = CreateEvent(NULL, false, false, NULL);
    //	mutex = CreateMutex(NULL, false, NULL);
    //	semaphore = CreateSemaphore(NULL, 4, 4, NULL);
    m_SoftLight = true;

    m_SkyBright = 1.0f;
    m_LightToRefresh = true;
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
        if (((*chunk)->m_X == Cx)&&((*chunk)->m_Z == Cz)) break;
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
    BlockCoord chnkx, chnky, chnkz;

    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        chunk.getBlockPositionByPointer(chunk.m_pBlocks + index, &chnkx, &chnky, &chnkz);

        if ((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
        {
            index++;
            continue;
        }

        BlockInWorld pos(chunk.m_X, chunk.m_Z, chnkx, chnky, chnkz);

        if (chunk.m_pBlocks[index].material != MAT_NO)
        {
            Chunk *p_temp_chunk;
            int temp_index;

            if (chunk.m_pBlocks[index].material == MAT_WATER)
            {
                if (!findBlock(pos.getSide(TOP), &p_temp_chunk, &temp_index)) showTile(&chunk, index, TOP);
                if (!findBlock(pos.getSide(BOTTOM), &p_temp_chunk, &temp_index)) { if (p_temp_chunk) showTile(&chunk, index, BOTTOM); }
                if (!findBlock(pos.getSide(RIGHT), &p_temp_chunk, &temp_index)) { if (p_temp_chunk) showTile(&chunk, index, RIGHT); }
                if (!findBlock(pos.getSide(LEFT), &p_temp_chunk, &temp_index)) { if (p_temp_chunk) showTile(&chunk, index, LEFT); }
                if (!findBlock(pos.getSide(BACK), &p_temp_chunk, &temp_index)) { if (p_temp_chunk) showTile(&chunk, index, BACK); }
                if (!findBlock(pos.getSide(FRONT), &p_temp_chunk, &temp_index)) { if (p_temp_chunk) showTile(&chunk, index, FRONT); }
            }
            else
            {
                if (!findBlock(pos.getSide(TOP), &p_temp_chunk, &temp_index)||(p_temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    showTile(&chunk, index, TOP); }
                if (!findBlock(pos.getSide(BOTTOM), &p_temp_chunk, &temp_index)||(p_temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (p_temp_chunk) showTile(&chunk, index, BOTTOM); }
                if (!findBlock(pos.getSide(RIGHT), &p_temp_chunk, &temp_index)||(p_temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (p_temp_chunk) showTile(&chunk, index, RIGHT); }
                if (!findBlock(pos.getSide(LEFT), &p_temp_chunk, &temp_index)||(p_temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (p_temp_chunk) showTile(&chunk, index, LEFT); }
                if (!findBlock(pos.getSide(BACK), &p_temp_chunk, &temp_index)||(p_temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (p_temp_chunk) showTile(&chunk, index, BACK); }
                if (!findBlock(pos.getSide(FRONT), &p_temp_chunk, &temp_index)||(p_temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (p_temp_chunk) showTile(&chunk, index, FRONT); }
            }
        }
        index++;
    }

    // draw boundary tiles
    index = 0;
    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        chunk.getBlockPositionByPointer(chunk.m_pBlocks + index, &chnkx, &chnky, &chnkz);

        if ((chnkx > 0)&&(chnkx < CHUNK_SIZE_XZ - 1)&&(chnkz > 0)&&(chnkz < CHUNK_SIZE_XZ - 1))
        {
            index++;
            continue;
        }

        BlockInWorld pos(chunk.m_X, chunk.m_Z, chnkx, chnky, chnkz);

        if ((chunk.m_pBlocks[index].material == MAT_NO)||(chunk.m_pBlocks[index].material == MAT_WATER))
        {
            Chunk *temp_chunk;
            Chunk *cur_chunk = &chunk;
            int temp_index;
            if (findBlock(pos.getSide(RIGHT), &temp_chunk, &temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, LEFT);
            if (findBlock(pos.getSide(LEFT), &temp_chunk, &temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, RIGHT);
            if (findBlock(pos.getSide(BACK), &temp_chunk, &temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, FRONT);
            if (findBlock(pos.getSide(FRONT), &temp_chunk, &temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, BACK);
        }
        index++;
    }
}

void World::drawUnLoadedBlocks(ChunkCoord x, ChunkCoord z)
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

        BlockInWorld pos(x, z, chnkx, chnky, chnkz);

        Chunk *temp_chunk;
        int temp_index;
        if (findBlock(pos.getSide(RIGHT), &temp_chunk, &temp_index)) hideTile(&*temp_chunk, temp_index, LEFT);
        if (findBlock(pos.getSide(LEFT), &temp_chunk, &temp_index)) hideTile(&*temp_chunk, temp_index, RIGHT);
        if (findBlock(pos.getSide(BACK), &temp_chunk, &temp_index)) hideTile(&*temp_chunk, temp_index, FRONT);
        if (findBlock(pos.getSide(FRONT), &temp_chunk, &temp_index)) hideTile(&*temp_chunk, temp_index, BACK);
        index++;
    }
}

int World::addBlock(const BlockInWorld &pos, char mat)
{
    if ((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) return 0;
    if (findBlock(pos)) return 0;

    Chunk *chunk = getChunkByPosition(pos.cx, pos.cz);
    if (chunk == NULL) return 0;

    int index = chunk->addBlock(pos.bx, pos.by, pos.bz, mat);

    Chunk *temp_chunk = 0;
    int temp_index;

    if (chunk->m_pBlocks[index].material == MAT_WATER)
    {
        if (!findBlock(pos.getSide(TOP), &temp_chunk, &temp_index)) showTile(chunk, index, TOP);
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(temp_chunk, temp_index, BOTTOM);
        if (!findBlock(pos.getSide(BOTTOM), &temp_chunk, &temp_index)) {if (temp_chunk) showTile(chunk, index, BOTTOM); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(temp_chunk, temp_index, TOP);
        if (!findBlock(pos.getSide(RIGHT), &temp_chunk, &temp_index)) {if (temp_chunk) showTile(chunk, index, RIGHT); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(temp_chunk, temp_index, LEFT);
        if (!findBlock(pos.getSide(LEFT), &temp_chunk, &temp_index)) {if (temp_chunk) showTile(chunk, index, LEFT); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(temp_chunk, temp_index, RIGHT);
        if (!findBlock(pos.getSide(BACK), &temp_chunk, &temp_index)) {if (temp_chunk) showTile(chunk, index, BACK); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(temp_chunk, temp_index, FRONT);
        if (!findBlock(pos.getSide(FRONT), &temp_chunk, &temp_index)) {if (temp_chunk) showTile(chunk, index, FRONT); }
        else if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(temp_chunk, temp_index, BACK);
    }
    else
    {
        if (!findBlock(pos.getSide(TOP), &temp_chunk, &temp_index))
            showTile(chunk, index, TOP);
        else {hideTile(temp_chunk, temp_index, BOTTOM); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) showTile(chunk, index, TOP); }
        if (!findBlock(pos.getSide(BOTTOM), &temp_chunk, &temp_index)) {
            if (temp_chunk) showTile(chunk, index, BOTTOM); }
        else {if ((temp_chunk)&&(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) showTile(chunk, index, BOTTOM); else hideTile(temp_chunk, temp_index, TOP); }
        if (!findBlock(pos.getSide(RIGHT), &temp_chunk, &temp_index)) {
            if (temp_chunk) showTile(chunk, index, RIGHT); }
        else {hideTile(temp_chunk, temp_index, LEFT); if ((temp_chunk)&&(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) showTile(chunk, index, RIGHT); }
        if (!findBlock(pos.getSide(LEFT), &temp_chunk, &temp_index)) {
            if (temp_chunk) showTile(chunk, index, LEFT); }
        else {hideTile(temp_chunk, temp_index, RIGHT); if ((temp_chunk)&&(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) showTile(chunk, index, LEFT); }
        if (!findBlock(pos.getSide(BACK), &temp_chunk, &temp_index)) {
            if (temp_chunk) showTile(chunk, index, BACK); }
        else {hideTile(temp_chunk, temp_index, FRONT); if ((temp_chunk)&&(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) showTile(chunk, index, BACK); }
        if (!findBlock(pos.getSide(FRONT), &temp_chunk, &temp_index)) {
            if (temp_chunk) showTile(chunk, index, FRONT); }
        else {hideTile(temp_chunk, temp_index, BACK); if ((temp_chunk)&&(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) showTile(chunk, index, FRONT); }
    }

    return 1;
}

int World::removeBlock(const BlockInWorld &pos)
{
    if ((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) return 0;
    if (!findBlock(pos)) return 0;

    Chunk *chunk = getChunkByPosition(pos.cx, pos.cz);
    if (chunk == NULL) return 0;

    int index = chunk->getIndexByPosition(pos.bx, pos.by, pos.bz);
    Chunk *temp_chunk = 0;
    int temp_index;

    if (!findBlock(pos.getSide(TOP), &temp_chunk, &temp_index)||chunk->m_pBlocks[index].material == MAT_WATER) hideTile(chunk, index, TOP);
    else {showTile(temp_chunk, temp_index, BOTTOM); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(chunk, index, TOP); }
    if (!findBlock(pos.getSide(BOTTOM), &temp_chunk, &temp_index)) hideTile(chunk, index, BOTTOM);
    else {showTile(temp_chunk, temp_index, TOP); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(chunk, index, BOTTOM); }
    if (!findBlock(pos.getSide(RIGHT), &temp_chunk, &temp_index)) hideTile(chunk, index, RIGHT);
    else {showTile(temp_chunk, temp_index, LEFT); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(chunk, index, RIGHT); }
    if (!findBlock(pos.getSide(LEFT), &temp_chunk, &temp_index)) hideTile(chunk, index, LEFT);
    else {showTile(temp_chunk, temp_index, RIGHT); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(chunk, index, LEFT); }
    if (!findBlock(pos.getSide(BACK), &temp_chunk, &temp_index)) hideTile(chunk, index, BACK);
    else {showTile(temp_chunk, temp_index, FRONT); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(chunk, index, BACK); }
    if (!findBlock(pos.getSide(FRONT), &temp_chunk, &temp_index)) hideTile(chunk, index, FRONT);
    else {showTile(temp_chunk, temp_index, BACK); if (temp_chunk->m_pBlocks[temp_index].material == MAT_WATER) hideTile(chunk, index, FRONT); }

    chunk->removeBlock(pos.bx, pos.by, pos.bz);
    return 1;
}

void World::showTile(Chunk *chunk, int index, char side) const
{
    if (!(chunk->m_pBlocks[index].visible & (1 << side)))
    {
        chunk->showTile(chunk->m_pBlocks + index, side);

        chunk->m_NeedToRender[0] = RENDER_NEED;
        chunk->m_NeedToRender[1] = RENDER_NEED;
    }
}

void World::hideTile(Chunk *chunk, int index, char side)
{
    if (chunk->m_pBlocks[index].visible & (1 << side))
    {
        chunk->hideTile(chunk->m_pBlocks + index, side);

        chunk->m_NeedToRender[0] = RENDER_NEED;
        chunk->m_NeedToRender[1] = RENDER_NEED;
    }
}

int World::findBlock(const BlockInWorld &pos, Chunk **chunk, int *index)
{
    if ((pos.by < 0)||(pos.by >= CHUNK_SIZE_Y)) { *chunk = NULL; *index = 0; return 0; }
    (*chunk) = getChunkByPosition(pos.cx, pos.cz);
    if ((*chunk) == NULL) { *index = 0; return 0; }
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

    if (chunk->getBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
        return 0;
    return 1;
}

void World::loadChunk(ChunkCoord x, ChunkCoord z)
{
    Param par = {x, z, this};
    LoadChunkThread(&par);
}

void World::unLoadChunk(ChunkCoord x, ChunkCoord z)
{
    Param par = {x, z, this};
    UnLoadChunkThread(&par);
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
                    p_chunk_array[i][j] = getChunkByPosition((chunk.m_X + i - 2), (chunk.m_Z + j - 2));
                }
            }
        }
    }

    Light light_fill(p_chunk_array);
    light_fill.m_Skylight = true;
    light_fill.updateLight();
    light_fill.m_Skylight = false;
    light_fill.updateLight();

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (p_chunk_array[i][j])
            {
                if (p_chunk_array[i][j]->m_NeedToRender[0] != RENDER_NEED)
                    p_chunk_array[i][j]->m_NeedToRender[0] = RENDER_MAYBE;
                if (p_chunk_array[i][j]->m_NeedToRender[1] != RENDER_NEED)
                    p_chunk_array[i][j]->m_NeedToRender[1] = RENDER_MAYBE;
            }
        }
    }
}

void World::saveChunks() const
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
