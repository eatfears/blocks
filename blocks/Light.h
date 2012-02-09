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

class Light
{
public:
	Light(Chunk *ChnkArr[5][5]);
	~Light(void);

	void UpdateLight(void);

private:
	Chunk *ChunkArray[5][5];

	void DiffuseLight(int i, int j, BlockInChunk tempx, BlockInChunk tempy, BlockInChunk tempz, int templight);
	void FindFillChunk(int i, int j, BlockInWorld tempWx, BlockInWorld tempWy, BlockInWorld tempWz, int templight);
};

