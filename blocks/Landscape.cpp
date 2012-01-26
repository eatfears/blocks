#include "Landscape.h"
#include "World.h"
#include <fstream>

Landscape::Landscape(World& ww)
	 : wWorld(ww)
{
	horizon				= LOCATION_SIZE_Y/2;
	scaleHeightMapXZ	= 128.0;
	scaleRoughness		= 32.0;
	scaleDetails		= 128.0;
	scaleBubblesXZ		= 32.0;
	scaleBubblesY		= 16.0;

	HeghtMapAmp			= 0*64;
	RoughnessAmp		= 1.7;
	DetailsAmp			= 64;
	BubblesAmp			= 54;
	HeghtMapOctaves		= 13;
	BubblesOctaves		= 5;

	pnBubbles			= PerlinNoise(0.5, BubblesOctaves);
	pnHeightMap			= PerlinNoise(0.5, HeghtMapOctaves);
	pnRoughness			= PerlinNoise(0.5, 9);
	pnDetails			= PerlinNoise(0.5, BubblesOctaves);

}

Landscape::~Landscape(void)
{
}

void Landscape::Generate(LocInWorld locx, LocInWorld locz)
{
	double height;
	double density;
	double hx, hz;
	double rx, rz;
	double dx, dz;
	double bx, by, bz;
	double details;


	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		hx = i/scaleHeightMapXZ;
		bx = i/scaleBubblesXZ;
		rx = i/scaleRoughness;
		dx = i/scaleDetails;

		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			hz = k/scaleHeightMapXZ;
			bz = k/scaleBubblesXZ;
			rz = k/scaleRoughness;
			dz = k/scaleDetails;


			details = pnDetails.PerlinNoise2d(dx, dz);
			height = HeghtMapAmp/2*(pnHeightMap.PerlinNoise2d(hx, hz) + RoughnessAmp*pnRoughness.PerlinNoise2d(rx, rz)*details) + horizon;

			for(int j = 0; j < LOCATION_SIZE_Y; j++)
			{
				by = j/scaleBubblesY;

				//density = j;
				//if(!(j%4)||!(i%8))
				density = (BubblesAmp)*pnBubbles.PerlinNoise3d(bx, by, bz)*(4*details + 0.3) + j;
				if(density < height) {if(j < 90) wWorld.AddTile(i, j, k, MAT_GRASS, false); else wWorld.AddTile(i, j, k, MAT_STONE, false);}
				//else if(j < horizon) wWorld.AddTile(i, j, k, MAT_SAND, false);
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

