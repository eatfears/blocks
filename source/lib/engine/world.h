#pragma once

#include <list>

#include "chunk.h"
#include "material.h"
#include "landscape.h"
#include "character.h"


typedef struct params
{
    ChunkCoord x;
    ChunkCoord z;
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
    int findBlock(const BlockInWorld &pos);
    int findBlock(const BlockInWorld &pos, Chunk **chunk, int *index);
    int addBlock(const BlockInWorld &pos, char mat);
    int removeBlock(const BlockInWorld &pos);

    void drawLoadedBlocksFinish(Chunk &chunk);
    void drawUnLoadedBlocks(ChunkCoord x, ChunkCoord z);
    void loadChunk(ChunkCoord x, ChunkCoord z);
    void unLoadChunk(ChunkCoord x, ChunkCoord z);
    Chunk* getChunkByPosition(ChunkCoord x, ChunkCoord z) const;

    void updateLight(Chunk& chunk) const;

    bool m_LightToRefresh;
    bool m_SoftLight;

    GLfloat m_SkyBright;
    GLfloat m_TorchBright;

    void saveChunks() const;

private:
    void showTile(Chunk *chunk, int index, char side) const;
    void hideTile(Chunk *chunk, int index, char side);
};
