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

    for (int i = 0; i < CHUNK_SIZE_XZ; i++)
    {
        bx = (chunk_x*CHUNK_SIZE_XZ+i)/m_ScaleBubblesXZ;
        for (int k = 0; k < CHUNK_SIZE_XZ; k++)
        {
            bz = (chunk_z*CHUNK_SIZE_XZ+k)/m_ScaleBubblesXZ;
            for (int j = 0; j < CHUNK_SIZE_Y; j++)
            {
                by = j/m_ScaleBubblesY;
                dens[i][j][k] = m_AmpBubbles*m_NoiseBubbles.perlinNoise3d(bx, by, bz);
            }
        }
    }

    for (int i = 0; i < CHUNK_SIZE_XZ; i++)
    {
        hx = (chunk_x*CHUNK_SIZE_XZ+i)/m_ScaleHeightMapXZ;
        rx = (chunk_x*CHUNK_SIZE_XZ+i)/m_ScaleRoughness;
        dx = (chunk_x*CHUNK_SIZE_XZ+i)/m_ScaleDetails;

        for (int k = 0; k < CHUNK_SIZE_XZ; k++)
        {
            hz = (chunk_z*CHUNK_SIZE_XZ+k)/m_ScaleHeightMapXZ;
            rz = (chunk_z*CHUNK_SIZE_XZ+k)/m_ScaleRoughness;
            dz = (chunk_z*CHUNK_SIZE_XZ+k)/m_ScaleDetails;

            details = m_NoiseDetails.perlinNoise2d(dx, dz);
            height = m_AmpHeghtMap/1.5*(m_NoiseHeightMap.perlinNoise2d(hx, hz) + m_AmpRoughness*m_NoiseRoughness.perlinNoise2d(rx, rz)*details) + m_WaterLevel;

            for (int j = 0; j < CHUNK_SIZE_Y; j++)
            {
                temp = dens[i][j][k];

                density = temp*(4*details + 0.3) + j;
                if (density < height)
                {
                    if (density < height - 3) chunk.setBlockMaterial(i, j, k, MAT_STONE);
                    else if (j < m_WaterLevel) chunk.setBlockMaterial(i, j, k, MAT_SAND);
                    else chunk.setBlockMaterial(i, j, k, MAT_DIRT);
                }
                else if (j < m_WaterLevel) chunk.setBlockMaterial(i, j, k, MAT_WATER);
            }
        }
    }

    for (BlockCoord i = 0; i < CHUNK_SIZE_XZ; i++)
    {
        for (BlockCoord k = 0; k < CHUNK_SIZE_XZ; k++)
        {
            double xx = (chunk_x*CHUNK_SIZE_XZ + i)/m_ScaleTemperature;
            double zz = (chunk_z*CHUNK_SIZE_XZ + k)/m_ScaleTemperature;
            double dd = m_NoiseTemperature.perlinNoise2d(xx, zz) + 0.15;

            for (BlockCoord j = CHUNK_SIZE_Y - 1; j >= 0; j--)
            {
                unsigned int index = Chunk::getIndexByPosition(i, j, k);
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
                    chunk.setBlockMaterial(i, j, k, material);
                }
            }
        }
    }
}

