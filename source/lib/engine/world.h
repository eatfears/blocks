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
    World *pWorld;
} Param;

class World
{
public:
    World();
    ~World();

    MaterialLibrary m_MaterialLib;
    std::list<Chunk*> *m_Chunks;
    Landscape m_Landscape;
    Character m_Player;

    void buildWorld();
    int findBlock(const BlockInWorld &pos);
    int findBlock(const BlockInWorld &pos, Chunk **chunk, int *index);
    int addBlock(const BlockInWorld &pos, char mat, bool show);
    int removeBlock(const BlockInWorld &pos, bool show);

    void drawLoadedBlocksFinish(Chunk &chunk);
    void drawUnLoadedBlocks(ChunkCoord x, ChunkCoord z);
    void loadChunk(ChunkCoord x, ChunkCoord z);
    void unLoadChunk(ChunkCoord x, ChunkCoord z);
    Chunk* getChunkByPosition(ChunkCoord x, ChunkCoord z) const;

    unsigned long hash(ChunkCoord x, ChunkCoord z) const { return (x + z*HASH_SIZE)&(HASH_SIZE-1); }
    void updateLight(Chunk& chunk) const;
    bool m_LightToRefresh;

    bool m_SoftLight;

    GLfloat m_SkyBright;
    GLfloat m_TorchBright;

    void saveChunks() const;

private:
    void showTile(Chunk *chunk, int index, char side) const;
    void hideTile(Chunk *chunk, int index, char side);

    std::list<ChunkPosition> m_LoadedChunks;
};
