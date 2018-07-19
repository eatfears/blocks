#pragma once

#include <fstream>

#include "perlin_noise.h"
#include "definitions.h"

class World;
class Chunk;

class Landscape
{
public:
    Landscape();
    ~Landscape();

    void init(unsigned int seed);

    void generate(Chunk &chunk);
    bool load(Chunk& chunk, std::fstream& savefile);
    void save(Chunk& chunk, std::fstream& savefile);
    void fill(Chunk& chunk, char mat, double fillness, int height);

    int horizon;
    double scaleHeightMapXZ;
    double scaleRoughness;
    double scaleDetails;
    double scaleBubblesXZ;
    double scaleBubblesY;

    double HeghtMapAmp;
    double RoughnessAmp;
    double DetailsAmp;
    double BubblesAmp;
    int HeghtMapOctaves;
    int BubblesOctaves;

    PerlinNoise pnBubbles;

    PerlinNoise pnHeightMap;
    PerlinNoise pnRoughness;
    PerlinNoise pnDetails;

    std::mt19937 generator;
};
