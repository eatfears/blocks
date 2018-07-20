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
    Light(Chunk *ChnkArr[5][5], bool m_Skylight);

    void updateLight() const;
    static void blockLight(const World &world, Chunk& chunk, char side, BlockCoord cx, BlockCoord cy, BlockCoord cz);
    static void softLight(const World &world, const BlockInWorld &pos, char side, int vertex);
    bool m_Skylight;

    static GLfloat getLight(const Chunk &chunk, int index);
private:
    static const float m_LightTable[16];
    static const char m_InfluencingLight[6][4];

    Chunk *m_ChunkArray[5][5];

    inline void setVal(const BlockInWorld &pos, int val) const;
    inline int getVal(const BlockInWorld &pos, bool *water_flag, bool *wall_flag) const;
    static float getBrightAverage(const World &world, const BlockInWorld &pos, int xx[8], int yy[8], int zz[8], char side);
    void recursiveDiffuse(BlockCoord i, BlockCoord j, BlockCoord k, int val, bool initial) const;
    void fillLight(Chunk &chunk, char bright, bool m_Skylight) const;
};
