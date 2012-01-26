#pragma once

class World;
#include "PerlinNoise.h"
#include "Blocks_Definitions.h"

class Landscape
{
public:
	Landscape(World& ww);
	~Landscape(void);

	void Generate(LocInWorld locx, LocInWorld locz);
	void Load(LocInWorld locx, LocInWorld locz);
	void Fill(LocInWorld locx, LocInWorld locz, char mat, double fillness, int height );
	
	int horizon;
	double scaleHeightMapXZ;
	double scaleRoughness;
	double scaleDetails;
	double scaleBubblesXZ;
	double scaleBubblesY;

	int HeghtMapAmp;
	int RoughnessAmp;
	int DetailsAmp;
	int BubblesAmp;
	int HeghtMapOctaves;
	int BubblesOctaves;

	World& wWorld;

	PerlinNoise pnBubbles;

	PerlinNoise pnHeightMap;
	PerlinNoise pnRoughness;
	PerlinNoise pnDetails;

};

