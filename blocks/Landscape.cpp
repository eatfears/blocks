#include "Landscape.h"
#include "World.h"
#include <fstream>

Landscape::Landscape(World& ww)
	 : wWorld(ww)
{
	horizon				= LOCATION_SIZE_Y/2;
	scaleHeightMapXZ	= 256.0;
	scaleBubblesXZ		= 32.0;
	scaleBubblesY		= 16.0;
	HeghtMapAmp			= 50;
	BubblesAmp			= 0*54;
	HeghtMapOctaves		= 13;
	BubblesOctaves		= 5;

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
	double hx, hz;
	double bx, by, bz;
	double pers;
	double perssc = 16;

	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		hx = i/scaleHeightMapXZ;
		bx = i/scaleBubblesXZ;

		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			hz = k/scaleHeightMapXZ;
			bz = k/scaleBubblesXZ;

			pers = 0.1*pnBubbles.PerlinNoise2d(hx/perssc, hz/perssc);
			pnHeightMap.persistence = 0.5 + pers;

			height = HeghtMapAmp*pnHeightMap.PerlinNoise2d(hx, hz) + horizon;

			for(int j = 0; j < LOCATION_SIZE_Y; j++)
			{
				by = j/scaleBubblesY;

				density = j;
				//density = BubblesAmp*pnBubbles.PerlinNoise3d(bx, by, bz) + j;
				if(density < height)
					wWorld.AddTile(i, j, k, MAT_GRASS, false);
				else if(j < horizon)
					wWorld.AddTile(i, j, k, MAT_SAND, false);
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
		/*int index = 0;
		TileInLoc locx, locy, locz;
		char mat;
		
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

void Landscape::Fill( LocInWorld locx, LocInWorld locz, char mat, double fillness, int height )
{
	int material = mat;

	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			for(int j = 0; j < height; j++)
			{
				if((double)rand()/(double)RAND_MAX < fillness) 
				{
					if (mat == 0) material = rand()%4+1;
					wWorld.AddTile(i, j, k, material, false);
				}
			}
		}
	}
}

