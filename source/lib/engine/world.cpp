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
    auto it = m_Chunks.begin();
    while (it != m_Chunks.end())
    {
        it->second->save();
        delete it->second;
        it = m_Chunks.erase(it);
    }
}

void World::buildWorld()
{
    ChunkCoord size = 2;
    for (ChunkCoord i = -size; i < size; i++)
    {
        for (ChunkCoord j = -size; j < size; j++)
        {
            loadChunk(i, j);
        }
    }
}

Chunk* World::getChunkByPosition(const ChunkInWorld &pos) const
{
    auto it = m_Chunks.find(pos);
    if (it != m_Chunks.end())
    {
        return it->second;
    }
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

bool World::loadChunk(ChunkCoord x, ChunkCoord z)
{
    auto pos = ChunkInWorld(x, z);
    Chunk *chunk = getChunkByPosition(pos);
    if(!chunk)
    {
        chunk = new Chunk(pos, *this);
    }
    else
    {
//        logger.notice() << "Chunk" << pos << "already loaded";
        return false;
    }

    chunk->load();
    chunk->drawLoadedBlocks();
    m_Chunks[pos] = chunk;

    if(chunk->m_LightToUpdate)
    {
        updateLight(*chunk, true);
        chunk->m_LightToUpdate = false;
    }
    return true;
}

bool World::unloadChunk(ChunkCoord x, ChunkCoord z)
{
    auto pos = ChunkInWorld(x, z);
    auto it = m_Chunks.find(pos);
    if (it != m_Chunks.end())
    {
        it->second->save();
        delete it->second;
        m_Chunks.erase(it);
    }
    else
    {
//        logger.notice() << "Chunk" << pos << "already unloaded";
        return false;
    }
    return true;
}

void World::updateLight(Chunk &chunk, bool new_chunk) const
{
    Chunk *p_chunk_array[3][3];

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i==1 && j==1)
            {
                p_chunk_array[1][1] = &chunk;
            }
            else
            {
                p_chunk_array[i][j] = getChunkByPosition(ChunkInWorld(chunk.cx + i - 1, chunk.cz + j - 1));
            }
        }
    }

    Light light_fill(p_chunk_array, new_chunk);
    light_fill.m_Skylight = true;
    light_fill.updateLight();
    light_fill.m_Skylight = false;
    light_fill.updateLight();

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
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
