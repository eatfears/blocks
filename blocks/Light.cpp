#include "Light.h"

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
