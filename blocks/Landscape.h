#pragma once
#include "PerlinNoise.h"
#include "Blocks_Definitions.h"

class World;

class Chunk;
typedef struct chnkpos ChunkPosition;

class Landscape
{
public:
	Landscape();
	~Landscape();

	void Init(unsigned int seed);

	void Generate(Chunk &chunk);
	bool Load(Chunk& chunk, std::fstream& savefile);
	void Save(Chunk& chunk, std::fstream& savefile);
	void Fill(Chunk& chunk, char mat, double fillness, int height);

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

	boost::mt19937 *generator;
};

