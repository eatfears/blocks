#pragma once

#include "common_include/definitions.h"
#include "chunk.h"

/*
Updating light in chunk will cause updating light in neighbor chunks
but clearing light in neighbor chunk clears light from its neighbor.. -_-
so i need to get light from neighbor neighbor chunk too

'OOO'
O***O
O*X*O
O***O
'OOO'

X - current chunk to update
* - also we should update this chunks
O - also need this chunks to fill * chunks
' - dont need this chunks
*/

class World;
// todo: rendered tiles dont saved

class Light
{
public:
    Light(Chunk *chunk_array[5][5]);

    void updateLight() const;
    static void blockLight(const World &world, const Chunk &chunk, char side, BlockCoord x, BlockCoord y, BlockCoord z);
    static void softLight(const World &world, const Chunk &chunk, const BlockInWorld &pos, char side, int vertex);
    bool m_Skylight = true;

    static GLfloat getLight(const Chunk &chunk, unsigned int index);
private:
    static const float m_LightTable[16];
    static const unsigned char m_InfluencingLight[6][4];
    static const int m_VertexX[8];
    static const int m_VertexY[8];
    static const int m_VertexZ[8];

    Chunk *m_ChunkArray[5][5];

    inline void setVal(const BlockInWorld &pos, unsigned char val) const;
    inline unsigned char getVal(const BlockInWorld &pos, bool *water_flag, bool *wall_flag) const;
    inline static float getBrightAverage(const World &world, const Chunk &chunk, const BlockInWorld &pos, bool inv_x, bool inv_y, bool inv_z, unsigned char side);
    inline void recursiveDiffuse(BlockCoord i, BlockCoord j, BlockCoord k, unsigned char val, bool initial) const;
    inline void fillLight(Chunk &chunk) const;

    DEFINE_LOGGER(LIGHT, logger)
};
