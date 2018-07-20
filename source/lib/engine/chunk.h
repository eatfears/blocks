#pragma once

#include <list>

#include "common_include/freeglut_static.h"
#include "position_items.h"


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
    std::list<Block*> *m_pDisplayedTiles;
    std::list<Block*> *m_pDisplayedWaterTiles;
    ChunkCoord m_X, m_Z;

    char m_NeedToRender[2];

    int	addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat);
    int removeBlock(BlockCoord x, BlockCoord y, BlockCoord z);

    void showTile(Block *bBlock, char side);
    void hideTile(Block *bBlock, char side);

    char getBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z);

    int getBlockPositionByPointer(Block *bCurrentBlock, BlockCoord *x, BlockCoord *y, BlockCoord *z) const;

    inline static int getBlockPositionByIndex(int index, BlockCoord *x, BlockCoord *y, BlockCoord *z)
    {
        if ((index < 0)||(index >= CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y))
        {
            return -1;
        }
        *z  = index%CHUNK_SIZE_XZ;
        index /= CHUNK_SIZE_XZ;
        *x = index%CHUNK_SIZE_XZ;
        index /= CHUNK_SIZE_XZ;
        *y = index;
        return 0;
    }

    inline static int getIndexByPosition(BlockCoord x, BlockCoord y, BlockCoord z)
    {
        return x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
    }

    void drawLoadedBlocks();

    void open();
    void save() const;
    bool m_LightToUpdate;

    void render(char mat, int *rendered);

private:
    int setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char cMat);
    void drawTile(const BlockInWorld &pos, Block* block, char side) const;

    GLuint m_RenderList = 0;
    bool m_Listgen;
};
