#pragma once

#include <fstream>

#include "noise/perlin_noise.h"
#include "common_include/definitions.h"
#include "logger/logger.h"


class Chunk;

class Landscape
{
public:
    Landscape();

    void init(unsigned int seed);

    void generate(Chunk &chunk) const;
    bool load(Chunk &chunk, std::fstream &savefile) const;
    void save(const Chunk &chunk, std::fstream &savefile) const;
    void fill(Chunk &chunk, char mat, double fillness, int height) const;

private:
    int m_WaterLevel;

    double m_ScaleHeightMapXZ;
    double m_ScaleRoughness;
    double m_ScaleDetails;
    double m_ScaleBubblesXZ;
    double m_ScaleBubblesY;
    double m_ScaleTemperature;

    double m_AmpHeghtMap;
    double m_AmpRoughness;
    double m_AmpDetails;
    double m_AmpBubbles;

    int m_OctavesHeghtMap;
    int m_OctavesBubbles;
    int m_OctavesRoughness;
    int m_OctavesTemperature;

    PerlinNoise m_NoiseHeightMap;
    PerlinNoise m_NoiseBubbles;
    PerlinNoise m_NoiseRoughness;
    PerlinNoise m_NoiseDetails;
    PerlinNoise m_NoiseTemperature;

    std::mt19937 m_Generator;

    DEFINE_LOGGER(LAND, logger)
};
