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
    char material;
    char visible;
} Block;

class Chunk
{
public:
    Chunk(ChunkCoord x, ChunkCoord z, World &wrld);
    ~Chunk();

    Block *m_pBlocks;
    char *m_SkyLight;
    char *m_TorchLight;
    World &m_World;
    std::list<Block *> *m_pDisplayedTiles;
    std::list<Block *> *m_pDisplayedWaterTiles;
    ChunkCoord m_X, m_Z;

    char m_NeedToRender[2];

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
    void save() const;
    bool m_LightToUpdate;

    void render(char mat, int *rendered);

private:
    int setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char cMat);
    void drawTile(const BlockInWorld &pos, Block* block, char side) const;

    GLuint m_RenderList;
    bool m_Listgen;
};
