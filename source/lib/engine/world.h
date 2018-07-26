#pragma once

#include <list>

#include "chunk.h"
#include "material.h"
#include "landscape.h"
#include "character.h"


class World
{
public:
    World();
    ~World();

    MaterialLibrary m_MaterialLib;

    std::map<ChunkInWorld, Chunk*> m_Chunks;
    Landscape m_Landscape;
    Character m_Player;

    void buildWorld();
    bool findBlock(const BlockInWorld &pos, Chunk *&chunk, unsigned int &index) const;
    bool findBlock(const BlockInWorld &pos, Chunk *&chunk) const;
    bool findBlock(const BlockInWorld &pos) const;
    bool addBlock(const BlockInWorld &pos, char mat) const;
    bool removeBlock(const BlockInWorld &pos) const;

    bool loadChunk(ChunkCoord x, ChunkCoord z);
    bool unloadChunk(ChunkCoord x, ChunkCoord z);
    Chunk* getChunkByPosition(const ChunkInWorld &pos) const;

    void updateLight(Chunk &chunk, bool init) const;

    bool m_LightToRefresh;
    bool m_SoftLight;

    GLfloat m_SkyBright;
    GLfloat m_TorchBright;

    void saveChunks() const;

private:
    DEFINE_LOGGER(WORLD, logger)
};
