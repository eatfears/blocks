#include "Landscape.h"
#include <fstream>
#include "World.h"
#include "zlib.h"

Landscape::Landscape()
{
	generator = new boost::mt19937;

	horizon				= CHUNK_SIZE_Y/2;
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

Landscape::~Landscape()
{
	delete generator;
}

void Landscape::Init(unsigned int seed)
{
	CreateDirectory("save//", NULL);

	std::fstream savefile;

	savefile.open ("save//conf.wld", std::fstream::in | std::fstream::binary);
	if(savefile.is_open())
	{
		savefile.read((char*)&seed, sizeof(seed));
		savefile.close();
	}
	else
	{
		savefile.open ("save//conf.wld", std::fstream::out | std::fstream::binary);

		if(savefile.is_open())
		{
			savefile.write((char*)&seed, sizeof(seed));
			savefile.close();
		}
	}

	generator->seed(seed);

	pnBubbles.InitNoise(generator);
	pnHeightMap.InitNoise(generator);
	pnRoughness.InitNoise(generator);
	pnDetails.InitNoise(generator);
}

void Landscape::Generate(Chunk &chunk)
{
	ChunkInWorld chunkx = chunk.x;
	ChunkInWorld chunkz = chunk.z;
	double height;
	double density;
	double hx, hz;
	double rx, rz;
	double dx, dz;
	double bx, by, bz;
	double details;
	double temp;
	double temp2 = 0;

	double dens[CHUNK_SIZE_XZ][CHUNK_SIZE_Y][CHUNK_SIZE_XZ];
	for(int i = chunkx*CHUNK_SIZE_XZ; i < (chunkx + 1)*CHUNK_SIZE_XZ; i++)
	{
		bx = i/scaleBubblesXZ;
		for(int k = chunkz*CHUNK_SIZE_XZ; k < (chunkz + 1)*CHUNK_SIZE_XZ; k++)
		{
			bz = k/scaleBubblesXZ;
			for(int j = 0; j < CHUNK_SIZE_Y; j++)
			{
				by = j/scaleBubblesY;
				dens[i%CHUNK_SIZE_XZ][j%CHUNK_SIZE_Y][k%CHUNK_SIZE_XZ] = (BubblesAmp)*pnBubbles.PerlinNoise3d(bx, by, bz);
			}
		}
	}

	for(int i = chunkx*CHUNK_SIZE_XZ; i < (chunkx + 1)*CHUNK_SIZE_XZ; i++)
	{
		hx = i/scaleHeightMapXZ;
		rx = i/scaleRoughness;
		dx = i/scaleDetails;

		for(int k = chunkz*CHUNK_SIZE_XZ; k < (chunkz + 1)*CHUNK_SIZE_XZ; k++)
		{
			hz = k/scaleHeightMapXZ;
			rz = k/scaleRoughness;
			dz = k/scaleDetails;


			details = pnDetails.PerlinNoise2d(dx, dz);
			height = HeghtMapAmp/1.5*(pnHeightMap.PerlinNoise2d(hx, hz) + RoughnessAmp*pnRoughness.PerlinNoise2d(rx, rz)*details) + horizon;

			for(int j = 0; j < CHUNK_SIZE_Y; j++)
			{

				//density = j;
				temp = dens[i%CHUNK_SIZE_XZ][j%CHUNK_SIZE_Y][k%CHUNK_SIZE_XZ];
				//temp2 = dens[i%LOCATION_SIZE_XZ][(j+1)%LOCATION_SIZE_Y][k%LOCATION_SIZE_XZ];

				density = temp*(4*details + 0.3) + j;
				if(density < height) {if(density < height - 3) chunk.AddBlock(i, j, k, MAT_STONE); else if(j < horizon) chunk.AddBlock(i, j, k, MAT_SAND); else chunk.AddBlock(i, j, k, MAT_DIRT);}
				else if(j < horizon) chunk.AddBlock(i, j, k, MAT_WATER);
			}
		}
	}

	for(int i = 0; i < CHUNK_SIZE_XZ; i++)
	{
		for(int k = 0; k < CHUNK_SIZE_XZ; k++)
		{
			for(int j = CHUNK_SIZE_Y - 1; j >= 0; j--)
			{
				int index = chunk.GetIndexByPosition(i, j, k);
				if(chunk.bBlocks[index].cMaterial != MAT_NO)
				{
					if(chunk.bBlocks[index].cMaterial == MAT_DIRT)
						chunk.bBlocks[index].bVisible |= (1 << SNOWCOVERED);
					break;
				}
			}
		}
	}
}

bool Landscape::Load(Chunk& chunk, std::fstream& savefile)
{
	int index = 0;
	int res;
	Bytef *buf;
	Bytef *bufcompress;
	uLongf uncompsize = CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y*2;
	uLongf compsize;
	Block *blocks = chunk.bBlocks;

	buf = new Bytef[uncompsize];
	bufcompress = new Bytef[compressBound(uncompsize)];

	savefile.seekg(0, std::ios::end);
	compsize = savefile.tellg();
	savefile.seekg(0);

	savefile.read((char*)bufcompress, compsize);

	res = uncompress(buf, &uncompsize, bufcompress, compsize);

	if((uncompsize != CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y*2)||(res != Z_OK))
		return false;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		blocks[index].cMaterial = buf[index];
		blocks[index].bVisible = buf[index + CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];

		index++;
	}
	chunk.LightToUpdate = true;

	delete buf;
	delete bufcompress;

	return true;
}

void Landscape::Save(Chunk& chunk, std::fstream& savefile)
{
	int index = 0;
	Bytef *buf;
	Bytef *bufcompress;
	uLongf uncompsize = CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y*2;
	uLongf compsize;
	Block *blocks = chunk.bBlocks;

	buf = new Bytef[uncompsize];
	bufcompress = new Bytef[compressBound(uncompsize)];

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		buf[index] = blocks[index].cMaterial;
		buf[index + CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y] = blocks[index].bVisible & ((1 << SNOWCOVERED) | (1 << GRASSCOVERED));
		index++;
	}
	compress(bufcompress, &compsize, buf, uncompsize);

	savefile.write((char*)bufcompress, compsize);

	delete buf;
	delete bufcompress;
}

void Landscape::Fill(Chunk& chunk, char mat, double fillness, int height)
{
	int material = mat;
	ChunkInWorld chunkx = chunk.x;
	ChunkInWorld chunkz = chunk.z;

	for(int i = chunkx*CHUNK_SIZE_XZ; i < (chunkx + 1)*CHUNK_SIZE_XZ; i++)
	{
		for(int k = chunkz*CHUNK_SIZE_XZ; k < (chunkz + 1)*CHUNK_SIZE_XZ; k++)
		{
			for(int j = 0; j < height; j++)
			{
				if((double)rand()/(double)RAND_MAX < fillness)
				{
					if (mat == 0) material = rand()%4+1;
					chunk.AddBlock(i, j, k, material);
				}
			}
		}
	}
}

