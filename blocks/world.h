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
    World *wWorld;
} Param;

class World
{
public:
    World();
    ~World();

    MaterialLibrary MaterialLib;
    std::list<Chunk *> *Chunks;
    Landscape m_Landscape;
    Character m_Player;

    void buildWorld();
    int findBlock(BlockInWorld pos);
    int findBlock(BlockInWorld pos, Chunk **chunk, int *index);
    int addBlock(BlockInWorld pos, char mat, bool show);
    int removeBlock(BlockInWorld pos, bool show);

    void drawLoadedBlocksFinish(Chunk &chunk);
    void drawUnLoadedBlocks(ChunkCoord Cx, ChunkCoord Cz);
    void loadChunk(ChunkCoord x, ChunkCoord z);
    void unLoadChunk(ChunkCoord x, ChunkCoord z);
    Chunk* getChunkByPosition(ChunkCoord Cx, ChunkCoord Cz);

    unsigned long hash(ChunkCoord x, ChunkCoord z) {return (x + z*HASH_SIZE)&(HASH_SIZE-1);}
    void updateLight(Chunk& chunk);
    bool LightToRefresh;

    bool SoftLight;

    GLfloat SkyBright;
    GLfloat TorchBright;

    void saveChunks();

private:
    void showTile(Chunk *chunk, int index, char side);
    void hideTile(Chunk *chunk, int index, char side);

    std::list<ChunkPosition> LoadedChunks;
};
