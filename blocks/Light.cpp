#include "Light.h"

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

Light::	Light(Chunk *ChnkArr[5][5])
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			ChunkArray[i][j] = ChnkArr[i][j];

			if(ChunkArray[i][j]) 
			{
				if((i > 0)&&(i < 4)&&(j > 0)&&(j < 4))
				{
					ChunkArray[i][j]->FillSkyLight(DAYLIGHT);
					ChunkArray[i][j]->NeedToRender[0] = true;
					ChunkArray[i][j]->NeedToRender[1] = true;
				}
			}
		}
	}
}

Light::~Light(void)
{
}

void Light::UpdateLight(void)
{
	BlockInChunk tempx, tempy, tempz;

	for(int templight = DAYLIGHT; templight > 0; templight--)
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{

				if(ChunkArray[i][j])
				{
					for (int index = 0; index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; index++)
					{

						if(ChunkArray[i][j]->SkyLight[index] == templight)
						{
							ChunkArray[i][j]->GetBlockPositionByIndex(index, &tempx, &tempy, &tempz);
							if((i == 0)&&(tempx < CHUNK_SIZE_XZ - 1)) continue;
							if((i == 4)&&(tempx > 0)) continue;
							if((j == 0)&&(tempz < CHUNK_SIZE_XZ - 1)) continue;
							if((j == 4)&&(tempz > 0)) continue;

							DiffuseLight(i, j, tempx, tempy, tempz, templight);
						}
					}

				}

			}
		}
	}
}

void Light::DiffuseLight(int i, int j, BlockInChunk tempx, BlockInChunk tempy, BlockInChunk tempz, int templight)
{
	BlockInWorld tempWx, tempWy, tempWz;

	tempWx = tempx + 1;
	tempWy = tempy;
	tempWz = tempz;
	FindFillChunk(i, j, tempWx, tempWy, tempWz, templight);

	tempWx = tempx - 1;
	tempWy = tempy;
	tempWz = tempz;
	FindFillChunk(i, j, tempWx, tempWy, tempWz, templight);

	tempWx = tempx;
	tempWy = tempy;
	tempWz = tempz + 1;
	FindFillChunk(i, j, tempWx, tempWy, tempWz, templight);

	tempWx = tempx;
	tempWy = tempy;
	tempWz = tempz - 1;
	FindFillChunk(i, j, tempWx, tempWy, tempWz, templight);

	tempWx = tempx;
	tempWy = tempy + 1;
	tempWz = tempz;
	FindFillChunk(i, j, tempWx, tempWy, tempWz, templight);

	tempWx = tempx;
	tempWy = tempy - 1;
	tempWz = tempz;
	FindFillChunk(i, j, tempWx, tempWy, tempWz, templight);
}

void Light::FindFillChunk(int i, int j, BlockInWorld tempWx, BlockInWorld tempWy, BlockInWorld tempWz, int templight)
{
	int tempindex;
	Chunk *TempChunk;
	int tempI = i, tempJ = j;

	if(tempWx >= CHUNK_SIZE_XZ) {tempI++; tempWx -= CHUNK_SIZE_XZ;}
	if(tempWx < 0) {tempI--; tempWx += CHUNK_SIZE_XZ;}
	if(tempWz >= CHUNK_SIZE_XZ) {tempJ++; tempWz -= CHUNK_SIZE_XZ;}
	if(tempWz < 0) {tempJ--; tempWz += CHUNK_SIZE_XZ;}

	if((tempI >= 1)&&(tempI < 4)&&(tempJ >= 1)&&(tempJ < 4)&&(tempWy > 0)&&(tempWy < CHUNK_SIZE_Y))
		TempChunk = ChunkArray[tempI][tempJ];
	else TempChunk = NULL;

	if(TempChunk)
	{
		tempindex = TempChunk->GetIndexByPosition(tempWx, tempWy, tempWz);
		if(TempChunk->bBlocks[tempindex].cMaterial == MAT_NO)
			if(TempChunk->SkyLight[tempindex] < templight - 1)
				TempChunk->SkyLight[tempindex] = templight - 1;

		if(TempChunk->bBlocks[tempindex].cMaterial == MAT_WATER)
			if(TempChunk->SkyLight[tempindex] < templight - 3)
				TempChunk->SkyLight[tempindex] = templight - 3;
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
			//wWorld.lLocations.begin()->GetIndexByPosition(sXcoord, sXcoord, sXcoord);

			res = Light::LightTable[temploc->SkyLight[index]];
		}else res = 1.0f;
		if ((side == FRONT)||(side == BACK)) res *= 0.85f;
		if ((side == RIGHT)||(side == LEFT)) res *= 0.90f;

		res = res - wWorld.SkyBright;
		glColor3f(res, res, res);
	}
}

void Light::SoftLight(World& wWorld, BlockInWorld X, BlockInWorld Y, BlockInWorld Z, char side, int vertex)
{
	if(wWorld.SoftLight)
	{
		static GLfloat res = 0;

		if (vertex == 0)
		{
			int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
			int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
			int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		else if (vertex == 1)
		{
			int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
			int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
			int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		else if (vertex == 2)
		{
			int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
			int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
			int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		else if (vertex == 3)
		{
			int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
			int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
			int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		else if (vertex == 4)
		{
			int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
			int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
			int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		else if (vertex == 5)
		{
			int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
			int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
			int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		else if (vertex == 6)
		{
			int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
			int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
			int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		else if (vertex == 7)
		{
			int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
			int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
			int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
			res = GetBrightAverage(wWorld, X, Y, Z, xx, yy, zz, side);
		}
		if ((side == FRONT)||(side == BACK)) res *= 0.85f;
		if ((side == RIGHT)||(side == LEFT)) res *= 0.90f;
		res = res - wWorld.SkyBright;
		glColor3f(res, res, res);
	}
}

float Light::GetBrightAverage(World& wWorld, BlockInWorld X, BlockInWorld Y, BlockInWorld Z, int xx[8], int yy[8], int zz[8], char side)
{
	GLfloat mat[4] = {0, 0, 0, 0};
	Chunk *temploc;
	float res = 0;
	int InflLight;

	static BlockInChunk 
		xloclight, 
		yloclight, 
		zloclight;

	bool DiagonalblockInfluate = true;

	for(int i = 0; i < 4; i++)
	{
		InflLight = Light::InfluencingLight[side][i];
		temploc = wWorld.GetChunkByBlock(X + xx[InflLight], Z + zz[InflLight]);

		//if((ylight >= CHUNK_SIZE_Y)||(ylight < 0)) temploc = NULL;
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

			mat[i] = Light::LightTable[temploc->SkyLight[index]];

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
