#include "Landscape.h"
#include "World.h"
#include <fstream>

Landscape::Landscape(void)
{
	horizon				= LOCATION_SIZE_Y/2;
	scaleHeightMapXZ	= 128.0;
	scaleRoughness		= 32.0;
	scaleDetails		= 128.0;
	scaleBubblesXZ		= 32.0;
	scaleBubblesY		= 16.0;

	HeghtMapAmp			= 64;
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

void Landscape::Generate(Location &loc)
{
	LocInWorld locx = loc.x;
	LocInWorld locz = loc.z;
	double height;
	double density;
	double hx, hz;
	double rx, rz;
	double dx, dz;
	double bx, by, bz;
	double details;
	double temp;
	double temp2 = 0;
	double temp3;

	double dens[LOCATION_SIZE_XZ][LOCATION_SIZE_Y][LOCATION_SIZE_XZ];
	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		bx = i/scaleBubblesXZ;
		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			bz = k/scaleBubblesXZ;
			for(int j = 0; j < LOCATION_SIZE_Y; j++)
			{
				by = j/scaleBubblesY;
				dens[i%LOCATION_SIZE_XZ][j%LOCATION_SIZE_Y][k%LOCATION_SIZE_XZ] = (BubblesAmp)*pnBubbles.PerlinNoise3d(bx, by, bz);
			}
		}
	}

	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		hx = i/scaleHeightMapXZ;
		rx = i/scaleRoughness;
		dx = i/scaleDetails;

		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			hz = k/scaleHeightMapXZ;
			rz = k/scaleRoughness;
			dz = k/scaleDetails;


			details = pnDetails.PerlinNoise2d(dx, dz);
			height = HeghtMapAmp/1.5*(pnHeightMap.PerlinNoise2d(hx, hz) + RoughnessAmp*pnRoughness.PerlinNoise2d(rx, rz)*details) + horizon;

			for(int j = 0; j < LOCATION_SIZE_Y; j++)
			{

				//density = j;
				temp = dens[i%LOCATION_SIZE_XZ][j%LOCATION_SIZE_Y][k%LOCATION_SIZE_XZ];
				//temp2 = dens[i%LOCATION_SIZE_XZ][(j+1)%LOCATION_SIZE_Y][k%LOCATION_SIZE_XZ];

				density = temp*(4*details + 0.3) + j;
				if(density < height) {if(density < height - 3) loc.AddBlock(i, j, k, MAT_STONE); else loc.AddBlock(i, j, k, MAT_GRASS);}
				else if(j < horizon) loc.AddBlock(i, j, k, MAT_WATER);
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

void Landscape::Fill( Location& loc, char mat, double fillness, int height )
{
	int material = mat;
	LocInWorld locx = loc.x;
	LocInWorld locz = loc.z;

	for(int i = locx*LOCATION_SIZE_XZ; i < (locx + 1)*LOCATION_SIZE_XZ; i++)
	{
		for(int k = locz*LOCATION_SIZE_XZ; k < (locz + 1)*LOCATION_SIZE_XZ; k++)
		{
			for(int j = 0; j < height; j++)
			{
				if((double)rand()/(double)RAND_MAX < fillness) 
				{
					if (mat == 0) material = rand()%4+1;
					loc.AddBlock(i, j, k, material);
				}
			}
		}
	}
}

