#pragma once

class World;
#include "PerlinNoise.h"
#include "Blocks_Definitions.h"

class Chunk;

class Landscape
{
public:
	Landscape(void);
	~Landscape(void);

	void Generate(Chunk &loc);
	void Load(LocInWorld locx, LocInWorld locz);
	void Fill(Chunk& loc, char mat, double fillness, int height );
	
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

