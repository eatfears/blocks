#pragma once
#include <list>

#include "Chunk.h"
#include "Material.h"
#include "Landscape.h"
#include "Character.h"

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
    Landscape lLandscape;
    Character player;

    void BuildWorld();
    int FindBlock(BlockInWorld pos);
    int FindBlock(BlockInWorld pos, Chunk **chunk, int *index);
    int AddBlock(BlockInWorld pos, char mat, bool show);
    int RemoveBlock(BlockInWorld pos, bool show);

    void DrawLoadedBlocksFinish(Chunk &chunk);
    void DrawUnLoadedBlocks(ChunkCoord Cx, ChunkCoord Cz);
    void LoadChunk(ChunkCoord x, ChunkCoord z);
    void UnLoadChunk(ChunkCoord x, ChunkCoord z);
    Chunk* GetChunkByPosition(ChunkCoord Cx, ChunkCoord Cz);

    unsigned long Hash(ChunkCoord x, ChunkCoord z) {return (x + z*HASH_SIZE)&(HASH_SIZE-1);}
    void UpdateLight(Chunk& chunk);
    bool LightToRefresh;

    bool SoftLight;

    GLfloat SkyBright;
    GLfloat TorchBright;

    void SaveChunks();

private:
    void ShowTile(Chunk *chunk, int index, char side);
    void HideTile(Chunk *chunk, int index, char side);

    std::list<ChunkPosition> LoadedChunks;
};
