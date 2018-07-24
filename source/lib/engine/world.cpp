#include "world.h"

#include <math.h>

#include "common_include/definitions.h"
#include "light.h"

World::World()
    : m_Player(*this)
{
    m_SoftLight = true;

    m_SkyBright = 1.0f;
    m_LightToRefresh = true;
}

World::~World()
{
    for (auto const &it : m_Chunks)
    {
        delete it.second;
    }
}

void World::buildWorld()
{
    ChunkCoord x = 0, z = 0;
    int size = 4;

    for(ChunkCoord i = x*size; i < (x+1)*size*1; i++)
    {
        for(ChunkCoord j = z*size; j < (z+1)*size; j++)
        {
            loadChunk(i, j);
        }
    }
}

Chunk* World::getChunkByPosition(const ChunkInWorld &pos) const
{
    try
    {
        return m_Chunks.at(pos);
    }
    catch (std::out_of_range &) {}
    return nullptr;
}

// todo: loaded block under water hides water tile
// todo: rewrite tiling engine!
void World::drawLoadedBlocksFinish(Chunk &chunk) const
{
    unsigned int index = 0;
    unsigned int temp_index;
    BlockCoord chunk_x, chunk_y, chunk_z;
    Chunk *temp_chunk;

    while (index < CHUNK_INDEX_MAX)
    {
        chunk.getBlockPositionByIndex(index, &chunk_x, &chunk_y, &chunk_z);

        if (chunk_x > 0 && chunk_x < CHUNK_SIZE_XZ - 1 && chunk_z > 0 && chunk_z < CHUNK_SIZE_XZ - 1)
        {
            index++;
            continue;
        }

        BlockInWorld pos(chunk, chunk_x, chunk_y, chunk_z);

        if (chunk.m_pBlocks[index].material != MAT_NO)
        {
            if (chunk.m_pBlocks[index].material == MAT_WATER)
            {
                if (!findBlock(pos.getSide(TOP), temp_chunk, temp_index)) showTile(&chunk, index, TOP);
                if (!findBlock(pos.getSide(BOTTOM), temp_chunk, temp_index)) { if (temp_chunk) showTile(&chunk, index, BOTTOM); }
                if (!findBlock(pos.getSide(RIGHT), temp_chunk, temp_index)) { if (temp_chunk) showTile(&chunk, index, RIGHT); }
                if (!findBlock(pos.getSide(LEFT), temp_chunk, temp_index)) { if (temp_chunk) showTile(&chunk, index, LEFT); }
                if (!findBlock(pos.getSide(BACK), temp_chunk, temp_index)) { if (temp_chunk) showTile(&chunk, index, BACK); }
                if (!findBlock(pos.getSide(FRONT), temp_chunk, temp_index)) { if (temp_chunk) showTile(&chunk, index, FRONT); }
            }
            else
            {
                if (!findBlock(pos.getSide(TOP), temp_chunk, temp_index)||(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    showTile(&chunk, index, TOP); }
                if (!findBlock(pos.getSide(BOTTOM), temp_chunk, temp_index)||(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(&chunk, index, BOTTOM); }
                if (!findBlock(pos.getSide(RIGHT), temp_chunk, temp_index)||(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(&chunk, index, RIGHT); }
                if (!findBlock(pos.getSide(LEFT), temp_chunk, temp_index)||(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(&chunk, index, LEFT); }
                if (!findBlock(pos.getSide(BACK), temp_chunk, temp_index)||(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(&chunk, index, BACK); }
                if (!findBlock(pos.getSide(FRONT), temp_chunk, temp_index)||(temp_chunk->m_pBlocks[temp_index].material == MAT_WATER)) {
                    if (temp_chunk) showTile(&chunk, index, FRONT); }
            }
        }
        index++;
    }

    // draw boundary tiles
    index = 0;
    while (index < CHUNK_INDEX_MAX)
    {
        chunk.getBlockPositionByIndex(index, &chunk_x, &chunk_y, &chunk_z);

        if (chunk_x > 0 && chunk_x < CHUNK_SIZE_XZ - 1 && chunk_z > 0 && chunk_z < CHUNK_SIZE_XZ - 1)
        {
            index++;
            continue;
        }

        BlockInWorld pos(chunk, chunk_x, chunk_y, chunk_z);

        if ((chunk.m_pBlocks[index].material == MAT_NO)||(chunk.m_pBlocks[index].material == MAT_WATER))
        {
            Chunk *cur_chunk = &chunk;
            if (findBlock(pos.getSide(RIGHT), temp_chunk, temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, LEFT);
            if (findBlock(pos.getSide(LEFT), temp_chunk, temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, RIGHT);
            if (findBlock(pos.getSide(BACK), temp_chunk, temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, FRONT);
            if (findBlock(pos.getSide(FRONT), temp_chunk, temp_index)&&(temp_chunk->m_pBlocks[temp_index].material != MAT_WATER)) if (temp_chunk->m_pBlocks != cur_chunk->m_pBlocks)
                showTile(&*temp_chunk, temp_index, BACK);
        }
        index++;
    }
}

void World::drawUnLoadedBlocks(const ChunkInWorld &pos) const
{
    unsigned int index = 0;
    BlockCoord chunk_x, chunk_y, chunk_z;

    while (index < CHUNK_INDEX_MAX)
    {
        Chunk::getBlockPositionByIndex(index, &chunk_x, &chunk_y, &chunk_z);

        if (chunk_x > 0 && chunk_x < CHUNK_SIZE_XZ - 1 && chunk_z > 0 && chunk_z < CHUNK_SIZE_XZ - 1)
        {
            index++;
            continue;
        }

        BlockInWorld block_pos(pos, chunk_x, chunk_y, chunk_z);
        Chunk *temp_chunk;
        unsigned int temp_index;
        if (findBlock(block_pos.getSide(RIGHT), temp_chunk, temp_index)) hideTile(&*temp_chunk, temp_index, LEFT);
        if (findBlock(block_pos.getSide(LEFT), temp_chunk, temp_index)) hideTile(&*temp_chunk, temp_index, RIGHT);
        if (findBlock(block_pos.getSide(BACK), temp_chunk, temp_index)) hideTile(&*temp_chunk, temp_index, FRONT);
        if (findBlock(block_pos.getSide(FRONT), temp_chunk, temp_index)) hideTile(&*temp_chunk, temp_index, BACK);
        index++;
    }
}

bool World::addBlock(const BlockInWorld &pos, char mat) const
{
    Chunk *chunk;
    if (pos.overflow() || findBlock(pos, chunk) || !chunk) return false;
    return chunk->placeBlock(pos, mat);
}

bool World::removeBlock(const BlockInWorld &pos) const
{
    Chunk *chunk;
    if (pos.overflow() || !findBlock(pos, chunk) || !chunk) return false;
    return chunk->unplaceBlock(pos);
}

void World::showTile(Chunk *chunk, unsigned int index, char side) const
{
    if (!(chunk->m_pBlocks[index].visible & (1 << side)))
    {
        chunk->showTile(chunk->m_pBlocks + index, side);

        chunk->m_NeedToRender[0] = RENDER_NEED;
        chunk->m_NeedToRender[1] = RENDER_NEED;
    }
}

void World::hideTile(Chunk *chunk, unsigned int index, char side) const
{
    if (chunk->m_pBlocks[index].visible & (1 << side))
    {
        chunk->hideTile(chunk->m_pBlocks + index, side);

        chunk->m_NeedToRender[0] = RENDER_NEED;
        chunk->m_NeedToRender[1] = RENDER_NEED;
    }
}

bool World::findBlock(const BlockInWorld &pos, Chunk *&chunk, unsigned int &index) const
{
    if (pos.overflow()) { chunk = nullptr; index = 0; return false; }
    chunk = getChunkByPosition(pos);
    if (chunk == nullptr) { index = 0; return false; }
    index = pos.bx*CHUNK_SIZE_XZ + pos.bz + pos.by*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;

    if (chunk->getBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
        return false;
    return true;
}

bool World::findBlock(const BlockInWorld &pos, Chunk *&chunk) const
{
    if (pos.overflow()) { chunk = nullptr; return false; }
    chunk = getChunkByPosition(pos);
    if (chunk == nullptr) { return false; }

    if (chunk->getBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
        return false;
    return true;
}

bool World::findBlock(const BlockInWorld &pos) const
{
    if (pos.overflow()) return false;
    Chunk *chunk = getChunkByPosition(pos);
    if (chunk == nullptr) return false;

    if (chunk->getBlockMaterial(pos.bx, pos.by, pos.bz) == MAT_NO)
        return false;
    return true;
}

void World::loadChunk(ChunkCoord x, ChunkCoord z)
{
    auto pos = ChunkInWorld(x, z);

    Chunk *chunk = getChunkByPosition(pos);
    if(!chunk)
    {
        chunk = new Chunk(pos, *this);
    }

    chunk->open();
    chunk->drawLoadedBlocks();

    //todo: leak if already exists
    m_Chunks[pos] = chunk;
    drawLoadedBlocksFinish(*chunk);

    if(chunk->m_LightToUpdate)
    {
        updateLight(*chunk);
        chunk->m_LightToUpdate = false;
    }
}

void World::unLoadChunk(ChunkCoord x, ChunkCoord z)
{
    auto pos = ChunkInWorld(x, z);
    auto it = m_Chunks.find(pos);
    if (it != m_Chunks.end())
    {
        delete it->second;
        m_Chunks.erase(it);
    }
    drawUnLoadedBlocks(pos);
}

void World::updateLight(Chunk &chunk) const
{
    Chunk *p_chunk_array[5][5];
    p_chunk_array[2][2] = &chunk;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (i!=2 || j!=2)
            {
                if ((i == 0 && j == 0) || (i == 4 && j == 0) || (i == 0 && j == 4) || (i == 4 && j == 4))
                {
                    p_chunk_array[i][j] = nullptr;
                }
                else
                {
                    p_chunk_array[i][j] = getChunkByPosition(ChunkInWorld(chunk.cx + i - 2, chunk.cz + j - 2));
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
    for (auto const &it : m_Chunks)
    {
        auto chunk = it.second;
        chunk->save();
    }
}
