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
