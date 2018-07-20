#pragma once

#include <fstream>

#include "noise/perlin_noise.h"
#include "common_include/definitions.h"

class World;
class Chunk;

class Landscape
{
public:
    Landscape();
    ~Landscape();

    void init(unsigned int seed);

    void generate(Chunk &chunk);
    bool load(Chunk &chunk, std::fstream &savefile) const;
    void save(const Chunk &chunk, std::fstream &savefile) const;
    void fill(Chunk& chunk, char mat, double fillness, int height);

    int m_Horizon;
    double m_ScaleHeightMapXZ;
    double m_ScaleRoughness;
    double m_ScaleDetails;
    double m_ScaleBubblesXZ;
    double m_ScaleBubblesY;

    double m_HeghtMapAmp;
    double m_RoughnessAmp;
    double m_DetailsAmp;
    double m_BubblesAmp;
    int m_HeghtMapOctaves;
    int m_BubblesOctaves;

    PerlinNoise m_NoiseBubbles;

    PerlinNoise m_NoiseHeightMap;
    PerlinNoise m_NoiseRoughness;
    PerlinNoise m_NoiseDetails;

    std::mt19937 m_Generator;
};
