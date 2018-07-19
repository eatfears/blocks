#pragma once

#include <list>

#include "position_items.h"
#include <GL/freeglut.h>

class World;

typedef struct
{
    ChunkCoord x;
    ChunkCoord z;
} ChunkPosition;

typedef struct
{
    char cMaterial;
    char bVisible;
} Block;

class Chunk
{
public:
    Chunk(ChunkCoord x, ChunkCoord z, World& wrld);
    ~Chunk();

    Block *bBlocks;
    char *SkyLight;
    char *TorchLight;
    World& wWorld;
    std::list<Block *> *DisplayedTiles;
    std::list<Block *> *DisplayedWaterTiles;
    ChunkCoord x;
    ChunkCoord z;
    char NeedToRender[2];

    int	addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat);
    int removeBlock(BlockCoord x, BlockCoord y, BlockCoord z);

    void showTile(Block *bBlock, char side);
    void hideTile(Block *bBlock, char side);

    char getBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z);

    int getBlockPositionByPointer(Block *bCurrentBlock, BlockCoord *x, BlockCoord *y, BlockCoord *z) const;
    static int getBlockPositionByIndex(int index, BlockCoord *x, BlockCoord *y, BlockCoord *z);
    static int getIndexByPosition(BlockCoord x, BlockCoord y, BlockCoord z);

    void drawLoadedBlocks();

    void open();
    void save();
    bool m_LightToUpdate;

    void render(char mat, int *rendered);

private:
    int setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char cMat);
    void drawTile(BlockInWorld pos, Block* block, char side);

    GLuint RenderList;
    bool listgen;
};
