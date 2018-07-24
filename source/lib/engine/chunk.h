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

class Chunk : public ChunkInWorld
{
public:
    Chunk(const ChunkInWorld &pos, World &world);
    ~Chunk();

    Block *m_pBlocks;
    unsigned char *m_SkyLight;
    unsigned char *m_TorchLight;
    const World &m_World;
    std::list<Block*> *m_pDisplayedTiles;
    std::list<Block*> *m_pDisplayedWaterTiles;

    char m_NeedToRender[2];

    unsigned int addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat);
    unsigned int removeBlock(BlockCoord x, BlockCoord y, BlockCoord z);
    bool placeBlock(const BlockInChunk &pos, char mat);
    bool unplaceBlock(const BlockInChunk &pos);

    void showTile(Block *bBlock, unsigned char side);
    void hideTile(Block *bBlock, unsigned char side);
    void showTile(unsigned int index, unsigned char side);
    void hideTile(unsigned int index, unsigned char side);

    char getBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z) const;

    inline bool getBlockPositionByPointer(Block *p_current_block, BlockCoord *x, BlockCoord *y, BlockCoord *z) const
    {
        unsigned int t = p_current_block - m_pBlocks;
        return getBlockPositionByIndex(t, x, y, z);
    }

    inline static bool getBlockPositionByIndex(unsigned int index, BlockCoord *x, BlockCoord *y, BlockCoord *z)
    {
        if (index >= CHUNK_INDEX_MAX)
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

    inline static unsigned int getIndexByPosition(const BlockInChunk &pos)
    {
        return getIndexByPosition(pos.bx, pos.by, pos.bz);
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
    unsigned int setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char mat);
    void drawTile(const BlockInWorld &pos, Block* block, char side) const;
    bool findBlock(const BlockInWorld &pos, Chunk *&temp_chunk, unsigned int &index) const;

    GLuint m_RenderList = 0;
    bool m_Listgen;

    DEFINE_LOGGER(CHUNK, logger)
};
