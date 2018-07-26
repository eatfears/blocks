#include "light.h"

#include "world.h"
#include "material.h"


// todo: first frame of game is in night
const unsigned char Light::m_InfluencingLight[6][4] = {
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

const int Light::m_VertexX[8] = {0, 0, 1, 1, 0, 0, 1, 1};
const int Light::m_VertexY[8] = {0, 0, 0, 0, 1, 1, 1, 1};
const int Light::m_VertexZ[8] = {0, 1, 0, 1, 0, 1, 0, 1};

#include "platform.h"
double constr = 0;
double update = 0;

Light::Light(Chunk *chunk_array[3][3], bool init)
    : m_Init(init)
{
    constr -= GetMillisecTime();

    BlockCoord y;
    unsigned int index;
    bool daylight;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if ((m_ChunkArray[i][j] = chunk_array[i][j]) == nullptr)
            {
                continue;
            }
            Chunk &chunk = *m_ChunkArray[i][j];

            if (m_Init && (i != 1 || j != 1))
            {
                continue;
            }

            /* Filling light */
            for (BlockCoord x = 0; x < CHUNK_SIZE_XZ; x++)
            {
                if ((i == 0 && x == 0) || (i == 2 && x == CHUNK_SIZE_XZ - 1)) continue;

                for (BlockCoord z = 0; z < CHUNK_SIZE_XZ; z++)
                {
                    if ((j == 0 && z == 0) || (j == 2 && z == CHUNK_SIZE_XZ - 1)) continue;

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
    }

    constr += GetMillisecTime();
}

void Light::updateLight() const
{
    update -= GetMillisecTime();

    if (m_Init)
    {
        for (BlockCoord j = 0; j < CHUNK_SIZE_Y; j++)
        {
            for (BlockCoord i = CHUNK_SIZE_XZ-1; i < 2*CHUNK_SIZE_XZ + 1; i++)
            {
                for (BlockCoord k = CHUNK_SIZE_XZ-1; k < 2*CHUNK_SIZE_XZ + 1; k++)
                {
                    recursiveDiffuse(i, j, k, getVal(BlockInWorld(i, j, k), nullptr, nullptr), true);
                }
            }
        }
    }
    else
    {
        for (BlockCoord j = 0; j < CHUNK_SIZE_Y; j++)
        {
            for (BlockCoord i = 0; i < 3*CHUNK_SIZE_XZ; i++)
            {
                for (BlockCoord k = 0; k < 3*CHUNK_SIZE_XZ; k++)
                {
                    recursiveDiffuse(i, j, k, getVal(BlockInWorld(i, j, k), nullptr, nullptr), true);
                }
            }
        }
    }

    update += GetMillisecTime();
}

void Light::recursiveDiffuse(BlockCoord i, BlockCoord j, BlockCoord k, unsigned char val, bool initial) const
{
    if (val <= DAYLIGHT)
    {
        unsigned char temp_val;
        bool water_flag = false;
        bool wall_flag = false;

        if (i > 0 && j >= 0 && k > 0
                && i < 3*CHUNK_SIZE_XZ - 1 && j < CHUNK_SIZE_Y && k < 3*CHUNK_SIZE_XZ - 1)
        {
            temp_val = getVal(BlockInWorld(i, j, k), &water_flag, &wall_flag);

            if (!wall_flag || initial)
            {
                if (temp_val < val || (temp_val == val && initial))
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

unsigned char Light::getVal(const BlockInWorld &pos, bool *water_flag, bool *wall_flag) const
{
    unsigned char ret = 0;
    if (wall_flag)
    {
        *wall_flag = true;
    }
    Chunk *p_temp_chunk = m_ChunkArray[pos.cx][pos.cz];
    if (p_temp_chunk)
    {
        unsigned int index = p_temp_chunk->getIndexByPosition(pos);
        char mat = p_temp_chunk->m_pBlocks[index].material;

        if (mat == MAT_NO || mat == MAT_WATER)
        {
            if (wall_flag)
            {
                *wall_flag = false;
            }
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
            if (mat == MAT_WATER)
            {
                *water_flag = true;
            }
        }
    }
    return ret;
}

void Light::setVal(const BlockInWorld &pos, unsigned char val) const
{
    Chunk *p_temp_chunk = m_ChunkArray[pos.cx][pos.cz];
    if (p_temp_chunk)
    {
        unsigned int index = p_temp_chunk->getIndexByPosition(pos);

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

void Light::blockLight(const World &world, const Chunk &chunk, char side, BlockCoord x, BlockCoord y, BlockCoord z)
{
    if (!world.m_SoftLight)
    {
        const BlockInWorld pos = BlockInWorld(chunk, x, y, z);
        const BlockInWorld pos_side = pos.getSide(side);

        static const Chunk *infl_chunk;

        // if in neighbor chunk
        if (pos.cx == pos_side.cx && pos.cz == pos_side.cz)
        {
            infl_chunk = &chunk;
        }
        else
        {
            infl_chunk = world.getChunkByPosition(pos_side);
        }

        if (pos.overflow()) infl_chunk = nullptr;

        GLfloat res;
        if (infl_chunk)
        {
            unsigned int index = Chunk::getIndexByPosition(pos_side);
            res = Light::getLight(*infl_chunk, index);
        }
        else
        {
            res = 1.0 - world.m_SkyBright;
        }

        if (side == FRONT || side == BACK) res *= 0.85f;
        if (side == RIGHT || side == LEFT) res *= 0.90f;

        glColor3f(res, res, res);
    }
}

void Light::softLight(const World &world, const Chunk &chunk, const BlockInWorld &pos, char side, int vertex)
{
    if (world.m_SoftLight)
    {
        static GLfloat res = 0.0f;
        // todo: no repeats
        switch (vertex)
        {
        case 0:
        {
            res = getBrightAverage(world, chunk, pos, true, false, true, side);
        } break;
        case 1:
        {
            res = getBrightAverage(world, chunk, pos, true, false, false, side);
        } break;
        case 2:
        {
            res = getBrightAverage(world, chunk, pos, false, false, false, side);
        } break;
        case 3:
        {
            res = getBrightAverage(world, chunk, pos, false, false, true, side);
        } break;
        case 4:
        {
            res = getBrightAverage(world, chunk, pos, true, true, true, side);
        } break;
        case 5:
        {
            res = getBrightAverage(world, chunk, pos, true, true, false, side);
        } break;
        case 6:
        {
            res = getBrightAverage(world, chunk, pos, false, true, false, side);
        } break;
        case 7:
        {
            res = getBrightAverage(world, chunk, pos, false, true, true, side);
        } break;
        }

        if (side == FRONT || side == BACK) res *= 0.85f;
        if (side == RIGHT || side == LEFT) res *= 0.90f;

        glColor3f(res, res, res);
    }
}

float Light::getBrightAverage(const World &world, const Chunk &chunk, const BlockInWorld &pos, bool inv_x, bool inv_y, bool inv_z, unsigned char side)
{
    GLfloat max = 10.0f;
    GLfloat mat[4] = {0, 0, 0, 0};
    float res = 0;
    const Chunk *infl_chunk;
    unsigned char infl_light;
    BlockInWorld temp_pos;
    bool diagonal_block_influate = true;

    for (int i = 0; i < 4; i++)
    {
        infl_light = Light::m_InfluencingLight[side][i];
        temp_pos = pos + BlockInChunk(inv_x ? -m_VertexX[infl_light] : m_VertexX[infl_light],
                                      inv_y ? -m_VertexY[infl_light] : m_VertexY[infl_light],
                                      inv_z ? -m_VertexZ[infl_light] : m_VertexZ[infl_light]);

        if (temp_pos.overflow())
        {
            mat[i] = max;
            continue;
        }

        if (temp_pos.cx == chunk.cx && temp_pos.cz == chunk.cz)
        {
            infl_chunk = &chunk;
        }
        else
        {
            infl_chunk = world.getChunkByPosition(temp_pos);
        }

        if (infl_chunk)
        {
            unsigned int index = infl_chunk->getIndexByPosition(temp_pos);
            mat[i] = Light::getLight(*infl_chunk, index);

            if (i == 1)
            {
                if (infl_chunk->m_pBlocks[index].material != MAT_NO && infl_chunk->m_pBlocks[index].material != MAT_WATER)
                {
                    diagonal_block_influate = false;
                }
            }
            else if (i == 2)
            {
                if (infl_chunk->m_pBlocks[index].material == MAT_NO || infl_chunk->m_pBlocks[index].material == MAT_WATER)
                {
                    diagonal_block_influate = true;
                }
            }
            else if (i == 3)
            {
                if (!diagonal_block_influate)
                {
                    mat[i] = 0.0f;
                }
            }

        }
        else
        {
            mat[i] = max;
        }
    }

    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (mat[i] != max)
        {
            res += mat[i];
            count ++;
        }
    }
    return res /= count;
}

GLfloat Light::getLight(const Chunk &chunk, unsigned int index)
{
    if (index >= CHUNK_INDEX_MAX)
    {
        return 1.0 - chunk.m_World.m_SkyBright;
    }

    GLfloat sky_light = Light::m_LightTable[chunk.m_SkyLight[index]] * (1.0 - chunk.m_World.m_SkyBright);
    GLfloat torch_light = Light::m_LightTable[chunk.m_TorchLight[index]] * chunk.m_World.m_TorchBright;

    if (sky_light > torch_light) return sky_light;
    else return torch_light;
}
