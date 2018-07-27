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

    char m_NeedToRender[2] = {RENDER_NO_NEED, RENDER_NO_NEED};

    unsigned int addBlock(BlockCoord x, BlockCoord y, BlockCoord z, char mat);
    unsigned int removeBlock(BlockCoord x, BlockCoord y, BlockCoord z);
    bool placeBlock(const BlockInChunk &pos, char mat);
    bool unplaceBlock(const BlockInChunk &pos);

    inline char getBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z) const
    {
        if (x < 0 || z < 0 || y < 0 || x >= CHUNK_SIZE_XZ || z >= CHUNK_SIZE_XZ || y >= CHUNK_SIZE_Y)
        {
            return -1;
        }
        return m_pBlocks[x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ].material;
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
    void load();
    void save() const;
    bool m_LightToUpdate = true;

    void render(char mat, int *rendered) /*const*/;

private:
    friend class Landscape;

    inline unsigned int setBlockMaterial(BlockCoord x, BlockCoord y, BlockCoord z, char mat)
    {
        if (x < 0 || z < 0 || y < 0 || x >= CHUNK_SIZE_XZ || z >= CHUNK_SIZE_XZ || y >= CHUNK_SIZE_Y)
        {
            return CHUNK_INDEX_MAX;
        }
        unsigned int index = getIndexByPosition(x, y, z);
        m_pBlocks[index].material = mat;
        m_pBlocks[index].visible = 0;
        return index;
    }

    inline bool findBlock(const BlockInWorld &pos, Chunk *&temp_chunk, unsigned int &index) const;
    void drawTile(const BlockInWorld &pos, Block* block, char side) const;
    void drawNeighboringChunks();

    void showTile(Block *bBlock, unsigned char side);
    void hideTile(Block *bBlock, unsigned char side);
    inline void showTile(unsigned int index, unsigned char side)
    {
        showTile(m_pBlocks + index, side);
    }
    inline void hideTile(unsigned int index, unsigned char side)
    {
        hideTile(m_pBlocks + index, side);
    }

    inline bool getBlockPositionByPointer(Block *p_current_block, BlockCoord *x, BlockCoord *y, BlockCoord *z) const
    {
        return getBlockPositionByIndex(p_current_block - m_pBlocks, x, y, z);
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

    GLuint m_RenderList = 0;
    bool m_Listgen = false;
    bool m_NeedSave = false;

    DEFINE_LOGGER(CHUNK, logger)
};
