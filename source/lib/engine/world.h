#pragma once

#include <list>

#include "chunk.h"
#include "material.h"
#include "landscape.h"
#include "character.h"


typedef struct params
{
    ChunkInWorld pos;
    World *p_World;
} Param;

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
    bool findBlock(const BlockInWorld &pos);
    bool findBlock(const BlockInWorld &pos, Chunk **chunk, unsigned int *index);
    bool addBlock(const BlockInWorld &pos, char mat);
    bool removeBlock(const BlockInWorld &pos);

    void drawLoadedBlocksFinish(Chunk &chunk);
    void drawUnLoadedBlocks(const ChunkInWorld &pos);
    void loadChunk(ChunkCoord x, ChunkCoord z);
    void unLoadChunk(ChunkCoord x, ChunkCoord z);
    Chunk* getChunkByPosition(const ChunkInWorld &pos) const;

    void updateLight(Chunk &chunk) const;

    bool m_LightToRefresh;
    bool m_SoftLight;

    GLfloat m_SkyBright;
    GLfloat m_TorchBright;

    void saveChunks() const;

private:
    void showTile(Chunk *chunk, unsigned int index, char side) const;
    void hideTile(Chunk *chunk, unsigned int index, char side) const;
};
