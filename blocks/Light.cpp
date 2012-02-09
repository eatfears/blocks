#include "Light.h"

#include "Material.h"

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
		if((TempChunk->bBlocks[tempindex].cMaterial == MAT_NO)||(TempChunk->bBlocks[tempindex].cMaterial == MAT_WATER))
			if(TempChunk->SkyLight[tempindex] < templight - 1)
				TempChunk->SkyLight[tempindex] = templight - 1;
	}
}
