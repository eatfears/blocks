#pragma once

#include <list>

#include "common_include/freeglut_static.h"
#include "position_items.h"
#include "logger/logger.h"


class World;

typedef struct
{
    char material;
    char visible;
} Block;

class Chunk
{
public:
    Chunk(const ChunkInWorld &pos, World &world);
    ~Chunk();

    Block *m_pBlocks;
    char *m_SkyLight;
    char *m_TorchLight;
    const World &m_World;
    std::list<Block*> *m_pDisplayedTiles;
    std::list<Block*> *m_pDisplayedWaterTiles;
    ChunkCoord m_X, m_Z;

    char m_NeedToRender[2];

    unsigned int addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat);
    unsigned int removeBlock(BlockCoord x, BlockCoord y, BlockCoord z);

    void showTile(Block *bBlock, char side);
    void hideTile(Block *bBlock, char side);

    char getBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z) const;

    bool getBlockPositionByPointer(Block *p_current_block, BlockCoord *x, BlockCoord *y, BlockCoord *z) const;

    inline static bool getBlockPositionByIndex(unsigned int index, BlockCoord *x, BlockCoord *y, BlockCoord *z)
    {
        if (index >= CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
        {
            return false;
        }
        *z  = index%CHUNK_SIZE_XZ;
        index /= CHUNK_SIZE_XZ;
        *x = index%CHUNK_SIZE_XZ;
        index /= CHUNK_SIZE_XZ;
        *y = index;
        return true;
    }

    inline static unsigned int getIndexByPosition(BlockCoord x, BlockCoord y, BlockCoord z)
    {
        return x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
    }

    void drawLoadedBlocks();

    void open();
    void save() const;
    bool m_LightToUpdate;

    void render(char mat, int *rendered) /*const*/;

private:
    unsigned int setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char cMat);
    void drawTile(const BlockInWorld &pos, Block* block, char side) const;

    GLuint m_RenderList = 0;
    bool m_Listgen;

    DEFINE_LOGGER(CHUNK, logger)
};
