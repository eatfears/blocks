#include "Light.h"
/*
#include "PlatformDefinitions.h"
#include <fstream>
*/
#include "World.h"
#include "Material.h"

char Light::InfluencingLight[6][4] = {
	4, 5, 6, 7, 4, 5, 6, 7,
	2, 3, 6, 7, 2, 3, 6, 7,
	1, 3, 5, 7, 1, 3, 5, 7
};

float Light::LightTable[16] = {
	0.000f, 0.044f, 0.055f, 0.069f,
	0.086f, 0.107f, 0.134f, 0.168f,
	0.210f, 0.262f, 0.328f, 0.410f,
	0.512f, 0.640f, 0.800f, 1.000f
};

Light::Light(Chunk *ChnkArr[5][5], bool skylight)
{
	this->skylight = skylight;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			ChunkArray[i][j] = ChnkArr[i][j];

			if(ChunkArray[i][j] && (i > 0)&&(i < 4)&&(j > 0)&&(j < 4))
			{
				FillLight(*ChunkArray[i][j], DAYLIGHT, skylight);
			}
		}
	}
}

Light::~Light(void)
{
}

void Light::UpdateLight(void)
{
	/*
	unsigned int start = GetMillisecTime();
	unsigned int dur;
	*/

	for (BlockInWorld i = CHUNK_SIZE_XZ - 1; i < 4*CHUNK_SIZE_XZ + 1; i++)
	{
		for (BlockInWorld j = 0; j < CHUNK_SIZE_Y; j++)
		{
			for (BlockInWorld k = CHUNK_SIZE_XZ - 1; k < 4*CHUNK_SIZE_XZ + 1; k++)
			{
				rec_diffuse(i, j, k, GetVal(i, j, k, NULL, NULL), true);
			}
		}
	}


	/*
	dur = GetMillisecTime() - start;

	std::fstream ds("out.txt", std::ios_base::out);

	ds << dur << "\t";
	ds << std::endl;
	*/
}

void Light::rec_diffuse( BlockInWorld i, BlockInWorld j, BlockInWorld k, int val, bool initial )
{
	if((val > 0)&&(val <= DAYLIGHT))
	{
		int temp_val;
		bool water_flag = false;
		bool wall_flag = false;

		if(	(i >= CHUNK_SIZE_XZ - 1)&&(j >= 0)&&(k >= CHUNK_SIZE_XZ - 1)&&
			(i < 4*CHUNK_SIZE_XZ + 1)&&(j < CHUNK_SIZE_Y)&&(k < 4*CHUNK_SIZE_XZ + 1))
		{
			temp_val = GetVal(i, j, k, &water_flag, &wall_flag);
		
			if(!wall_flag || initial)
			if((temp_val < val) || (temp_val <= val)&&initial )
			{	
				SetVal(i, j, k, val);

				if(water_flag) val -= 3;
				else val--;

				rec_diffuse(i - 1, j, k, val, false);
				rec_diffuse(i + 1, j, k, val, false);
				rec_diffuse(i, j - 1, k, val, false);
				rec_diffuse(i, j + 1, k, val, false);
				rec_diffuse(i, j, k - 1, val, false);
				rec_diffuse(i, j, k + 1, val, false);
			}
		}
	}
}

int Light::GetVal( BlockInWorld i, BlockInWorld j, BlockInWorld k, bool *water_flag, bool *wall_flag )
{
	BlockInChunk x, y, z;
	char ret = 0;
	if(wall_flag)
		*wall_flag = true;
	
	x = i%CHUNK_SIZE_XZ;
	y = j%CHUNK_SIZE_Y;
	z = k%CHUNK_SIZE_XZ;

	Chunk *TempChunk = ChunkArray[i/CHUNK_SIZE_XZ][k/CHUNK_SIZE_XZ];
	if(TempChunk)
	{
		int index = TempChunk->GetIndexByPosition(x, y, z);
		char mat = TempChunk->bBlocks[index].cMaterial;
		
		if((mat == MAT_NO)||(mat == MAT_WATER))
		{
			if(wall_flag)
				*wall_flag = false;
		}
		if(skylight)
			ret = TempChunk->SkyLight[index];
		else
			ret = TempChunk->TorchLight[index];

		if(water_flag)
		{	
			if(mat == MAT_WATER) *water_flag = true;
		}
	}
	return ret;
}

void Light::SetVal( BlockInWorld i, BlockInWorld j, BlockInWorld k, int val )
{
	BlockInChunk x, y, z;

	x = i%CHUNK_SIZE_XZ;
	y = j%CHUNK_SIZE_Y;
	z = k%CHUNK_SIZE_XZ;
	
	Chunk *TempChunk = ChunkArray[i/CHUNK_SIZE_XZ][k/CHUNK_SIZE_XZ];
	if(TempChunk)
	{
		int index = TempChunk->GetIndexByPosition(x, y, z);

		if(skylight)
			TempChunk->SkyLight[index] = val;
		else
			TempChunk->TorchLight[index] = val;
	}
}

void Light::BlockLight(World& wWorld, Chunk& chunk, int side, BlockInChunk cx, BlockInChunk cy, BlockInChunk cz)
{
	if(!wWorld.SoftLight)
	{
		static Chunk *temploc;
		static BlockInWorld xlight, ylight, zlight;
		static GLfloat res;
		static BlockInChunk
			xloclight,
			yloclight,
			zloclight;
		static BlockInWorld blckwx, blckwz;

		blckwx = chunk.x*CHUNK_SIZE_XZ;
		blckwz = chunk.z*CHUNK_SIZE_XZ;


		switch(side)
		{
		case TOP:
			xlight = cx;
			ylight = cy + 1;
			zlight = cz;
			break;
		case BOTTOM:
			xlight = cx;
			ylight = cy - 1;
			zlight = cz;
			break;
		case RIGHT:
			xlight = cx + 1;
			ylight = cy;
			zlight = cz;
			break;
		case LEFT:
			xlight = cx - 1;
			ylight = cy;
			zlight = cz;
			break;
		case FRONT:
			xlight = cx;
			ylight = cy;
			zlight = cz - 1;
			break;
		case BACK:
			xlight = cx;
			ylight = cy;
			zlight = cz + 1;
			break;
		}

		if((xlight >= CHUNK_SIZE_XZ)||(xlight < 0)||(zlight >= CHUNK_SIZE_XZ)||(zlight < 0))
			temploc = wWorld.GetChunkByBlock(xlight + blckwx, zlight + blckwz);
		else temploc = &chunk;
		if((ylight >= CHUNK_SIZE_Y)||(ylight < 0)) temploc = NULL;
		if (temploc)
		{
			wWorld.GetPosInChunkByWorld(xlight, ylight, zlight, &xloclight, &yloclight, &zloclight);
			int index = temploc->GetIndexByPosition(xloclight, yloclight, zloclight);

			Light::GetLight(*temploc, index, res);
		}else res = (1.0 - wWorld.SkyBright);

		if ((side == FRONT)||(side == BACK)) res *= 0.85f;
		if ((side == RIGHT)||(side == LEFT)) res *= 0.90f;

		glColor3f(res, res, res);
	}
}

void Light::SoftLight(World& wWorld, BlockInWorld X, BlockInWorld Y, BlockInWorld Z, char side, int vertex)
{
	if(wWorld.SoftLight)
	{
		static GLfloat res = 0;

		switch (vertex)
		{
		case 0:
			{
				int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
				int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
				int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		case 1:
			{
				int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
				int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
				int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		case 2:
			{
				int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
				int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
				int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		case 3:
			{
				int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
				int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
				int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		case 4:
			{
				int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
				int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
				int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		case 5:
			{
				int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
				int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
				int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		case 6:
			{
				int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
				int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
				int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		case 7:
			{
				int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
				int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
				int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
				res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
			}
			break;
		}
		
		if ((side == FRONT)||(side == BACK)) res *= 0.85f;
		if ((side == RIGHT)||(side == LEFT)) res *= 0.90f;

		glColor3f(res, res, res);
	}
}

float Light::GetBrightAverage(World& wWorld, BlockInWorld X, BlockInWorld Y, BlockInWorld Z, int xx[8], int yy[8], int zz[8], char side)
{
	GLfloat mat[4] = {0, 0, 0, 0};
	static Chunk *center;
	static Chunk *temploc;
	float res = 0;
	static int InflLight;
	static ChunkInWorld Cx, Cz;

	static BlockInChunk
		xloclight,
		yloclight,
		zloclight;

	bool DiagonalblockInfluate = true;

	center = wWorld.GetChunkByBlock(X, Z);

	for(int i = 0; i < 4; i++)
	{
		InflLight = Light::InfluencingLight[side][i];
		wWorld.GetChunkByBlock(X + xx[InflLight], Z + zz[InflLight], &Cx, &Cz);
		if((Cx != center->x)||(Cz != center->z))
			temploc = wWorld.GetChunkByPosition(Cx, Cz);
		else temploc = center;

		if (temploc)
		{
			wWorld.GetPosInChunkByWorld(X + xx[InflLight], Y + yy[InflLight], Z + zz[InflLight], &xloclight, &yloclight, &zloclight);

			if(yloclight >= CHUNK_SIZE_Y)
			{
				mat[i] = 10.0f;
				continue;
			}

			int index = temploc->GetIndexByPosition(xloclight, yloclight, zloclight);

			if((i == 1)&&(temploc->bBlocks[index].cMaterial != MAT_NO)&&(temploc->bBlocks[index].cMaterial != MAT_WATER))
				DiagonalblockInfluate = false;
			if(i == 2)
			{
				if((temploc->bBlocks[index].cMaterial == MAT_NO)||(temploc->bBlocks[index].cMaterial == MAT_WATER))
					DiagonalblockInfluate = true;
			}

			Light::GetLight(*temploc, index, mat[i]);

			if((i == 3)&&(!DiagonalblockInfluate))
				mat[i] = 0.0f;

		}else mat[i] = 10.0f;
	}

	int count = 0;
	for(int i = 0; i < 4; i++)
	{
		if(mat[i] != 10.0f)
		{
			res += mat[i];
			count ++;
		}
	}
	return res /= count;
}

void Light::GetLight( Chunk& chunk, int index, GLfloat& br )
{
	br = Light::LightTable[chunk.SkyLight[index]] * ( 1.0 - chunk.wWorld.SkyBright);

	GLfloat torch_light = Light::LightTable[chunk.TorchLight[index]];
	torch_light *= chunk.wWorld.TorchBright;

	if(br < torch_light) br = torch_light;
}

void Light::FillLight( Chunk& chunk, char bright, bool skylight )
{
	if (skylight)
	{
		BlockInChunk y;
		int index;

		for(int i = 0; i < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; i++)
		{
			chunk.SkyLight[i] = 0;
			if(chunk.bBlocks[i].cMaterial == MAT_PUMPKIN_SHINE)
				chunk.TorchLight[i] = 15;
			else
				chunk.TorchLight[i] = 0;
		}

		for(BlockInChunk x = 0; x < CHUNK_SIZE_XZ; x++)
		{
			for(BlockInChunk z = 0; z < CHUNK_SIZE_XZ; z++)
			{
				y = CHUNK_SIZE_Y - 1;
				while (y > 0)
				{
					index = Chunk::GetIndexByPosition(x, y, z);
					if(chunk.bBlocks[index].cMaterial != MAT_NO)
						break;

					chunk.SkyLight[index] = bright;
					y--;
				}
			}
		}
	} 
	else
	{
		for(int i = 0; i < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; i++)
		{
			if(chunk.bBlocks[i].cMaterial == MAT_PUMPKIN_SHINE)
				chunk.TorchLight[i] = 15;
			else
				chunk.TorchLight[i] = 0;
		}
	}
}
