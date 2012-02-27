#pragma once

#include "Blocks_Definitions.h"
#include "Chunk.h"

/*
Updating light in chunk will cause updating light in neighbor chunks
but clearing light in neighbor chunk clears light from its neighbor.. -_-
so i need to get light from neighbor neighbor chunk too

'OOO'
O***O
O*X*O
O***O
'OOO'

X - current chunk to update
* - also we should update this chunks
O - also need this chunks to fill * chunks
' - dont need this chunks
*/
class World;

class Light
{
public:
	Light(Chunk *ChnkArr[5][5], bool skylight);
	Light() {};
	~Light(void);

	static float LightTable[16];

	void UpdateLight(void);
	static void BlockLight(World& wWorld, Chunk& chunk, int side, BlockInChunk cx, BlockInChunk cy, BlockInChunk cz);
	static void SoftLight(World& wWorld, BlockInWorld X, BlockInWorld Y, BlockInWorld Z, char side, int vertex);
	bool skylight;

	static void GetLight(Chunk& chunk, int index, GLfloat& br);
private:
	static char InfluencingLight[6][4];

	Chunk *ChunkArray[5][5];

	void SetVal( BlockInWorld i, BlockInWorld j, BlockInWorld k, int val );
	int GetVal( BlockInWorld i, BlockInWorld j, BlockInWorld k, bool *water_flag, bool *wall_flag  );
	static float GetBrightAverage(World& wWorld, BlockInWorld X, BlockInWorld Y, BlockInWorld Z, int xx[8], int yy[8], int zz[8], char side);
	void rec_diffuse( BlockInWorld i, BlockInWorld j, BlockInWorld k, int val, bool initial );
	void FillLight(Chunk& chunk, char bright, bool skylight);
};
