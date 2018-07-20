#include "light.h"

#include "world.h"
#include "material.h"


// todo: first frame of game is in night
const char Light::m_InfluencingLight[6][4] = {
    4, 5, 6, 7, 4, 5, 6, 7,
    2, 3, 6, 7, 2, 3, 6, 7,
    1, 3, 5, 7, 1, 3, 5, 7
};

const float Light::m_LightTable[16] = {
    0.000f, 0.044f, 0.055f, 0.069f,
    0.086f, 0.107f, 0.134f, 0.168f,
    0.210f, 0.262f, 0.328f, 0.410f,
    0.512f, 0.640f, 0.800f, 1.000f
};

#include "platform.h"
double constr = 0;
double update = 0;

Light::Light(Chunk *ChnkArr[5][5])
{
    constr -= GetMillisecTime();

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            m_ChunkArray[i][j] = ChnkArr[i][j];

            if (m_ChunkArray[i][j] && (i > 0)&&(i < 4)&&(j > 0)&&(j < 4))
            {
                fillLight(*m_ChunkArray[i][j]);
            }
        }
    }
    constr += GetMillisecTime();
}

void Light::updateLight() const
{
    update -= GetMillisecTime();
    for (BlockCoord j = 0; j < CHUNK_SIZE_Y; j++)
    {
        for (BlockCoord i = CHUNK_SIZE_XZ - 1; i < 4*CHUNK_SIZE_XZ + 1; i++)
        {
            for (BlockCoord k = CHUNK_SIZE_XZ - 1; k < 4*CHUNK_SIZE_XZ + 1; k++)
            {
                auto a = BlockInWorld(i, j, k);
                recursiveDiffuse(i, j, k, getVal(BlockInWorld(i, j, k), NULL, NULL), true);
            }
        }
    }
    update += GetMillisecTime();
}

void Light::recursiveDiffuse(BlockCoord i, BlockCoord j, BlockCoord k, int val, bool initial) const
{
    if (val > 0 && val <= DAYLIGHT)
    {
        int temp_val;
        bool water_flag = false;
        bool wall_flag = false;

        if ((i >= CHUNK_SIZE_XZ - 1) && (j >= 0) && (k >= CHUNK_SIZE_XZ - 1)
                && (i < 4*CHUNK_SIZE_XZ + 1) && (j < CHUNK_SIZE_Y) && (k < 4*CHUNK_SIZE_XZ + 1))
        {
            temp_val = getVal(BlockInWorld(i, j, k), &water_flag, &wall_flag);

            if (!wall_flag || initial)
            {
                if ((temp_val < val) || (temp_val <= val) && initial)
                {
                    setVal(BlockInWorld(i, j, k), val);

                    if (water_flag) val -= 3;
                    else val--;

                    recursiveDiffuse(i - 1, j, k, val, false);
                    recursiveDiffuse(i + 1, j, k, val, false);
                    recursiveDiffuse(i, j - 1, k, val, false);
                    recursiveDiffuse(i, j + 1, k, val, false);
                    recursiveDiffuse(i, j, k - 1, val, false);
                    recursiveDiffuse(i, j, k + 1, val, false);
                }
            }
        }
    }
}

int Light::getVal(const BlockInWorld &pos, bool *water_flag, bool *wall_flag) const
{
    char ret = 0;
    if (wall_flag)
    {
        *wall_flag = true;
    }
    Chunk *p_temp_chunk = m_ChunkArray[pos.cx][pos.cz];
    if (p_temp_chunk)
    {
        int index = p_temp_chunk->getIndexByPosition(pos.bx, pos.by, pos.bz);
        char mat = p_temp_chunk->m_pBlocks[index].material;

        if ((mat == MAT_NO)||(mat == MAT_WATER))
        {
            if (wall_flag)
                *wall_flag = false;
        }
        if (m_Skylight)
        {
            ret = p_temp_chunk->m_SkyLight[index];
        }
        else
        {
            ret = p_temp_chunk->m_TorchLight[index];
        }
        if (water_flag)
        {
            if (mat == MAT_WATER) *water_flag = true;
        }
    }
    return ret;
}

void Light::setVal(const BlockInWorld &pos, int val) const
{
    Chunk *p_temp_chunk = m_ChunkArray[pos.cx][pos.cz];
    if (p_temp_chunk)
    {
        int index = p_temp_chunk->getIndexByPosition(pos.bx, pos.by, pos.bz);

        if (m_Skylight)
        {
            p_temp_chunk->m_SkyLight[index] = val;
        }
        else
        {
            p_temp_chunk->m_TorchLight[index] = val;
        }
    }
}

void Light::blockLight(const World &world, Chunk &chunk, char side, BlockCoord cx, BlockCoord cy, BlockCoord cz)
{
    if (!world.m_SoftLight)
    {
        static Chunk *temploc;
        static BlockInWorld pos, pos_side;
        static GLfloat res;

        pos = BlockInWorld(chunk.m_X, chunk.m_Z, cx, cy, cz);
        pos_side = pos.getSide(side);

        // if in neighbor chunk
        if (pos.cx != pos_side.cx || pos.cz != pos_side.cz)
        {
            temploc = world.getChunkByPosition(pos_side.cx, pos_side.cz);
        }
        else
        {
            temploc = &chunk;
        }

        if (pos.overflow()) temploc = NULL;
        if (temploc)
        {
            int index = temploc->getIndexByPosition(pos_side.bx, pos_side.by, pos_side.bz);
            res = Light::getLight(*temploc, index);
        }
        else
        {
            res = (1.0 - world.m_SkyBright);
        }

        if ((side == FRONT)||(side == BACK)) res *= 0.85f;
        if ((side == RIGHT)||(side == LEFT)) res *= 0.90f;

        glColor3f(res, res, res);
    }
}

void Light::softLight(const World &world, const BlockInWorld &pos, char side, int vertex)
{
    if (world.m_SoftLight)
    {
        static GLfloat res = 0;
        // todo: no repeats
        switch (vertex)
        {
        case 0:
        {
            int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
            int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
            int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        case 1:
        {
            int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
            int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
            int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        case 2:
        {
            int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
            int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
            int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        case 3:
        {
            int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
            int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
            int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        case 4:
        {
            int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
            int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
            int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        case 5:
        {
            int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
            int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
            int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        case 6:
        {
            int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
            int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
            int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        case 7:
        {
            int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
            int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
            int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
            res = getBrightAverage(world, pos, xx, yy, zz, side);
        } break;
        }

        if ((side == FRONT)||(side == BACK)) res *= 0.85f;
        if ((side == RIGHT)||(side == LEFT)) res *= 0.90f;

        glColor3f(res, res, res);
    }
}

float Light::getBrightAverage(const World &world, const BlockInWorld &pos, int xx[8], int yy[8], int zz[8], char side)
{
    GLfloat mat[4] = {0, 0, 0, 0};
    static Chunk *center;
    static Chunk *temploc;
    float res = 0;
    static int InflLight;
    BlockInWorld tempPos, tempPos2;

    bool DiagonalblockInfluate = true;

    center = world.getChunkByPosition(pos.cx, pos.cz);

    for (int i = 0; i < 4; i++)
    {
        InflLight = Light::m_InfluencingLight[side][i];
        tempPos2 = pos + BlockInChunk(xx[InflLight], yy[InflLight], zz[InflLight]);
        if ((tempPos2.cx != center->m_X)||(tempPos2.cz != center->m_Z))
            temploc = world.getChunkByPosition(tempPos2.cx, tempPos2.cz);
        else temploc = center;

        if (temploc)
        {
            tempPos = pos + BlockInChunk(xx[InflLight], yy[InflLight], zz[InflLight]); // todo: delete

            if (tempPos.by >= CHUNK_SIZE_Y)
            {
                mat[i] = 10.0f;
                continue;
            }

            int index = temploc->getIndexByPosition(tempPos.bx, tempPos.by, tempPos.bz);

            if ((i == 1)&&(temploc->m_pBlocks[index].material != MAT_NO)&&(temploc->m_pBlocks[index].material != MAT_WATER))
                DiagonalblockInfluate = false;
            if (i == 2)
            {
                if ((temploc->m_pBlocks[index].material == MAT_NO)||(temploc->m_pBlocks[index].material == MAT_WATER))
                    DiagonalblockInfluate = true;
            }

            mat[i] = Light::getLight(*temploc, index);

            if ((i == 3)&&(!DiagonalblockInfluate))
                mat[i] = 0.0f;

        } else mat[i] = 10.0f;
    }

    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (mat[i] != 10.0f) {
            res += mat[i];
            count ++;
        }
    }
    return res /= count;
}

GLfloat Light::getLight(const Chunk &chunk, int index)
{
    if (index < 0 || index >= CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        return 1.0 - chunk.m_World.m_SkyBright;
    }

    GLfloat sky_light = Light::m_LightTable[chunk.m_SkyLight[index]] * ( 1.0 - chunk.m_World.m_SkyBright);
    GLfloat torch_light = Light::m_LightTable[chunk.m_TorchLight[index]] * chunk.m_World.m_TorchBright;

    if (sky_light > torch_light) return sky_light;
    else return torch_light;
}

void Light::fillLight(Chunk &chunk) const
{
    BlockCoord y;
    int index;
    bool daylight;

    for (BlockCoord x = 0; x < CHUNK_SIZE_XZ; x++)
    {
        for (BlockCoord z = 0; z < CHUNK_SIZE_XZ; z++)
        {
            daylight = true;
            y = CHUNK_SIZE_Y - 1;

            while (y > 0)
            {
                index = Chunk::getIndexByPosition(x, y, z);
                if (daylight && chunk.m_pBlocks[index].material != MAT_NO)
                {
                    daylight = false;
                }

                if (daylight)
                {
                    chunk.m_SkyLight[index] = DAYLIGHT;
                }
                else
                {
                    chunk.m_SkyLight[index] = 0;
                }

                if (chunk.m_pBlocks[index].material == MAT_PUMPKIN_SHINE)
                {
                    chunk.m_TorchLight[index] = DAYLIGHT;
                }
                else
                {
                    chunk.m_TorchLight[index] = 0;
                }
                y--;
            }
        }
    }
}
