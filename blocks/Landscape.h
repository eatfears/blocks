#pragma once

class World;
#include "PerlinNoise.h"
#include "Blocks_Definitions.h"

class Chunk;
typedef struct chnkpos ChunkPosition;

class Landscape
{
public:
	Landscape(void);
	~Landscape(void);

	void Generate(Chunk &chunk);
	void Load(ChunkPosition chpos);
	void Fill(Chunk& chunk, char mat, double fillness, int height );
	
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

};

