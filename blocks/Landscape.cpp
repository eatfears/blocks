#include "Landscape.h"
#include "World.h"
#include <fstream>

Landscape::Landscape(World& ww)
	 : wWorld(ww)
{
	horizon				= LOCATION_SIZE_Y/2;
	scaleHeightMapXZ	= 128.0;
	scaleBubblesXZ		= 32.0;
	scaleBubblesY		= 16.0;
	HeghtMapAmp			= 64;
	BubblesAmp			= 164;
	HeghtMapOctaves		= 16;
	BubblesOctaves		= 6;

	pnBubbles			= PerlinNoise(0.5, BubblesOctaves);
	pnHeightMap			= PerlinNoise(0.5, HeghtMapOctaves);
}

Landscape::~Landscape(void)
{
}

void Landscape::Generate(LocInWorld locx, LocInWorld locz)
{
	double height;
	double density;

	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			double x = i/scaleHeightMapXZ;
			double z = k/scaleHeightMapXZ;
			height = HeghtMapAmp*pnHeightMap.PerlinNoise2d(x, z) + horizon;

			for(int j = 0; j < LOCATION_SIZE_Y; j++)
			{
				double x = i/scaleBubblesXZ;
				double y = j/scaleBubblesY;
				double z = k/scaleBubblesXZ;

				density = BubblesAmp*pnBubbles.PerlinNoise3d(x, y, z) + j;
				if(density < height)
					wWorld.AddTile(i, j, k, MAT_GRASS, false);
			}
		}
	}
}

void Landscape::Load( LocInWorld locx, LocInWorld locz )
{
	std::fstream filestr;

	filestr.open ("test", std::fstream::in | std::fstream::binary );
	
	
	if(filestr.is_open())
	{
		int index = 0;
		TileInLoc locx, locy, locz;
		char mat;
		/*
		while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
		{
			filestr >> mat;
			if (filestr.eof()) break;

			loc->GetTilePositionByIndex(index, &locx, &locy, &locz);
			loc->AddTile(locx, locy, locz, mat);

	//		if(rand()%500) filestr << (char) (rand()%4 + 1); else filestr << (char) 0;
			index++;
		}*/

		filestr.close();
	}
	
}

void Landscape::Fill( LocInWorld locx, LocInWorld locz, char mat, double fillness )
{
	int material = mat;

	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			for(int j = 0; j < LOCATION_SIZE_Y; j++)
			{
				if (mat == 0) material = rand()%4+1;
				if((double)rand()/(double)RAND_MAX < fillness) wWorld.AddTile(i, j, k, material, false);
			}
		}
	}
}

