#pragma once

class World;
#include "PerlinNoise.h"
#include "Blocks_Definitions.h"

class Location;

class Landscape
{
public:
	Landscape(void);
	~Landscape(void);

	void Generate(Location &loc);
	void Load(LocInWorld locx, LocInWorld locz);
	void Fill(Location& loc, char mat, double fillness, int height );
	
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
	double HeghtMapOctaves;
	double BubblesOctaves;

	PerlinNoise pnBubbles;

	PerlinNoise pnHeightMap;
	PerlinNoise pnRoughness;
	PerlinNoise pnDetails;

};

