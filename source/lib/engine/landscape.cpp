#include "landscape.h"

#include <fstream>
#include <zlib.h>

#include "world.h"


Landscape::Landscape()
{
    m_WaterLevel = CHUNK_SIZE_Y/2;

    m_ScaleHeightMapXZ	= 128.0;
    m_ScaleRoughness		= 32.0;
    m_ScaleDetails		= 128.0;
    m_ScaleBubblesXZ		= 32.0;
    m_ScaleBubblesY		= 16.0;
    m_ScaleTemperature = 60.0;

    m_AmpHeghtMap			= 64;
    m_AmpBubbles			= 54;
    m_AmpRoughness		= 1.7;
    m_AmpDetails			= 64;

    m_OctavesHeghtMap		= 13;
    m_OctavesBubbles		= 5;
    m_OctavesRoughness		= 9;
    m_OctavesTemperature = 4;

    m_NoiseHeightMap			= PerlinNoise(0.5, m_OctavesHeghtMap);
    m_NoiseBubbles			= PerlinNoise(0.5, m_OctavesBubbles);
    m_NoiseRoughness			= PerlinNoise(0.5, m_OctavesRoughness);
    m_NoiseDetails			= PerlinNoise(0.5, m_OctavesBubbles);
    m_NoiseTemperature			= PerlinNoise(0.5, m_OctavesTemperature);
}

void Landscape::init(unsigned int seed)
{
    //CreateDirectory("save//", NULL);

    std::fstream savefile;

    savefile.open ("save//conf.wld", std::fstream::in | std::fstream::binary);
    if (savefile.is_open())
    {
        savefile.read((char*)&seed, sizeof(seed));
        savefile.close();
    }
    else
    {
        savefile.open ("save//conf.wld", std::fstream::out | std::fstream::binary);

        if (savefile.is_open())
        {
            savefile.write((char*)&seed, sizeof(seed));
            savefile.close();
        }
    }

    logger.notice() << "Landscape seed:" << seed;
    m_Generator.seed(seed);

    m_NoiseBubbles.initNoise(m_Generator);
    m_NoiseHeightMap.initNoise(m_Generator);
    m_NoiseRoughness.initNoise(m_Generator);
    m_NoiseDetails.initNoise(m_Generator);
    m_NoiseTemperature.initNoise(m_Generator);
}

void Landscape::generate(Chunk &chunk) const
{
    ChunkCoord chunk_x = chunk.cx;
    ChunkCoord chunk_z = chunk.cz;

    double height;
    double density;
    double hx, hz;
    double rx, rz;
    double dx, dz;
    double bx, by, bz;
    double details;
    double temp;

    double dens[CHUNK_SIZE_XZ][CHUNK_SIZE_Y][CHUNK_SIZE_XZ];

    for (int i = chunk_x*CHUNK_SIZE_XZ; i < (chunk_x + 1)*CHUNK_SIZE_XZ; i++)
    {
        bx = i/m_ScaleBubblesXZ;
        for (int k = chunk_z*CHUNK_SIZE_XZ; k < (chunk_z + 1)*CHUNK_SIZE_XZ; k++)
        {
            bz = k/m_ScaleBubblesXZ;
            for (int j = 0; j < CHUNK_SIZE_Y; j++)
            {
                by = j/m_ScaleBubblesY;
                dens[i%CHUNK_SIZE_XZ][j%CHUNK_SIZE_Y][k%CHUNK_SIZE_XZ] = m_AmpBubbles*m_NoiseBubbles.perlinNoise3d(bx, by, bz);
            }
        }
    }

    for (int i = chunk_x*CHUNK_SIZE_XZ; i < (chunk_x + 1)*CHUNK_SIZE_XZ; i++)
    {
        hx = i/m_ScaleHeightMapXZ;
        rx = i/m_ScaleRoughness;
        dx = i/m_ScaleDetails;

        for (int k = chunk_z*CHUNK_SIZE_XZ; k < (chunk_z + 1)*CHUNK_SIZE_XZ; k++)
        {
            hz = k/m_ScaleHeightMapXZ;
            rz = k/m_ScaleRoughness;
            dz = k/m_ScaleDetails;

            details = m_NoiseDetails.perlinNoise2d(dx, dz);
            height = m_AmpHeghtMap/1.5*(m_NoiseHeightMap.perlinNoise2d(hx, hz) + m_AmpRoughness*m_NoiseRoughness.perlinNoise2d(rx, rz)*details) + m_WaterLevel;

            for (int j = 0; j < CHUNK_SIZE_Y; j++)
            {
                temp = dens[i%CHUNK_SIZE_XZ][j%CHUNK_SIZE_Y][k%CHUNK_SIZE_XZ];

                density = temp*(4*details + 0.3) + j;
                if (density < height)
                {
                    if (density < height - 3) chunk.addBlock(i%CHUNK_SIZE_XZ, j, k%CHUNK_SIZE_XZ, MAT_STONE);
                    else if (j < m_WaterLevel) chunk.addBlock(i%CHUNK_SIZE_XZ, j, k%CHUNK_SIZE_XZ, MAT_SAND);
                    else chunk.addBlock(i%CHUNK_SIZE_XZ, j, k%CHUNK_SIZE_XZ, MAT_DIRT);
                }
                else if (j < m_WaterLevel) chunk.addBlock(i%CHUNK_SIZE_XZ, j, k%CHUNK_SIZE_XZ, MAT_WATER);
            }
        }
    }

    for (int i = 0; i < CHUNK_SIZE_XZ; i++)
    {
        for (int k = 0; k < CHUNK_SIZE_XZ; k++)
        {
            double xx = (chunk_x*CHUNK_SIZE_XZ + i)/m_ScaleTemperature;
            double zz = (chunk_z*CHUNK_SIZE_XZ + k)/m_ScaleTemperature;
            double dd = m_NoiseTemperature.perlinNoise2d(xx, zz) + 0.15;

            for (int j = CHUNK_SIZE_Y - 1; j >= 0; j--)
            {
                int index = chunk.getIndexByPosition(i, j, k);
                if (chunk.m_pBlocks[index].material != MAT_NO)
                {
                    if (chunk.m_pBlocks[index].material == MAT_DIRT)
                    {
                        if (dd > 0.2)
                        {
                            chunk.m_pBlocks[index].visible |= (1 << GRASSCOVERED);
                        }
                        else if (dd < 0)
                        {
                            chunk.m_pBlocks[index].visible |= (1 << SNOWCOVERED);
                        }
                    }
                    break;
                }
            }
        }
    }
}

bool Landscape::load(Chunk& chunk, std::fstream &savefile) const
{
    int index = 0;
    int res;
    Bytef *buf;
    Bytef *bufcompress;
    uLongf uncompsize = CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y*2;
    uLongf compsize;
    Block *blocks = chunk.m_pBlocks;

    buf = new Bytef[uncompsize];
    bufcompress = new Bytef[compressBound(uncompsize)];

    savefile.seekg(0, std::ios::end);
    compsize = savefile.tellg();
    savefile.seekg(0);

    savefile.read((char*)bufcompress, compsize);

    res = uncompress(buf, &uncompsize, bufcompress, compsize);

    if ((uncompsize != CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y*2)||(res != Z_OK))
    {
        return false;
    }
    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        blocks[index].material = buf[index];
        blocks[index].visible = buf[index + CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];

        index++;
    }
    chunk.m_LightToUpdate = true;

    delete buf;
    delete bufcompress;

    return true;
}

void Landscape::save(const Chunk &chunk, std::fstream &savefile) const
{
    int index = 0;
    Bytef *buf;
    Bytef *bufcompress;
    uLongf uncompsize = CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y*2;
    uLongf compsize;
    Block *blocks = chunk.m_pBlocks;

    buf = new Bytef[uncompsize];
    bufcompress = new Bytef[compressBound(uncompsize)];

    while (index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
    {
        buf[index] = blocks[index].material;
        buf[index + CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y] = blocks[index].visible & ((1 << SNOWCOVERED) | (1 << GRASSCOVERED));
        index++;
    }
    compress(bufcompress, &compsize, buf, uncompsize);

    savefile.write((char*)bufcompress, compsize);

    delete buf;
    delete bufcompress;
}

void Landscape::fill(Chunk& chunk, char mat, double fillness, int height) const
{
    int material = mat;
    ChunkCoord chunk_x = chunk.cx;
    ChunkCoord chunk_z = chunk.cz;

    for (int i = chunk_x*CHUNK_SIZE_XZ; i < (chunk_x + 1)*CHUNK_SIZE_XZ; i++)
    {
        for (int k = chunk_z*CHUNK_SIZE_XZ; k < (chunk_z + 1)*CHUNK_SIZE_XZ; k++)
        {
            for (int j = 0; j < height; j++) {
                if ((double)rand()/(double)RAND_MAX < fillness)
                {
                    if (mat == 0) material = rand()%4+1;
                    chunk.addBlock(i, j, k, material);
                }
            }
        }
    }
}

