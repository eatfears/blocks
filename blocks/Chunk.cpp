#include "Chunk.h"
#include "World.h"

float LightTable[16] = {
	0.000f, 0.044f, 0.055f, 0.069f, 
	0.086f, 0.107f, 0.134f, 0.168f, 
	0.210f, 0.262f, 0.328f, 0.410f, 
	0.512f, 0.640f, 0.800f, 1.000f};

Chunk::Chunk(ChunkInWorld x, ChunkInWorld z, World& wrld)
	: wWorld(wrld)
{
	Chunk::x = x; Chunk::z = z;

	bBlocks = new Block[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];
	SkyLight = new char[CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y];

	DisplayedTiles = new std::list<Block *>[6];
	DisplayedWaterTiles = new std::list<Block *>[6];
	
	for(int i = 0; i < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; i++)
	{	
		SkyLight[i] = 0;
		bBlocks[i].cMaterial = MAT_NO;
		for(int N = 0; N < 6; N++)
			bBlocks[i].bVisible &= ~(1 << N);
	}
	NeedToRender[0] = RENDER_NEED;
	NeedToRender[1] = RENDER_NEED;

	listgen = false;
	LightToUpdate = true;
	
	mutex = CreateMutex(NULL, false, NULL);
}

Chunk::~Chunk()
{
	delete[] bBlocks;
	delete[] SkyLight;

	delete[] DisplayedTiles;
	delete[] DisplayedWaterTiles;

	glDeleteLists(RenderList, 2);
}

int Chunk::AddBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z, char mat)
{
	x = x%CHUNK_SIZE_XZ;
	y = y%CHUNK_SIZE_Y;
	z = z%CHUNK_SIZE_XZ;
	if((GetBlockMaterial(x, y, z) != MAT_NO)||(GetBlockMaterial(x, y, z) == -1)) return -1;

	BlockInChunk index = SetBlockMaterial(x, y, z, mat);
	bBlocks[index].bVisible = 0;

	LightToUpdate = true;

	return index;
}

int Chunk::RemoveBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z)
{
	if((GetBlockMaterial(x, y, z) == MAT_NO)||(GetBlockMaterial(x, y, z) == -1)) return -1;

	BlockInChunk index = SetBlockMaterial(x, y, z, MAT_NO);
	bBlocks[index].bVisible = 0;

	LightToUpdate = true;

	return index;
}

void Chunk::ShowTile(Block *bBlock, char N)
{
	if(!bBlock) return;
	if(bBlock->cMaterial == MAT_NO) return;
	if(bBlock->bVisible & (1 << N)) return;
	

	std::list<Block *> *Tiles;
	if(bBlock->cMaterial == MAT_WATER)
		Tiles = &DisplayedWaterTiles[N];
	else
		Tiles = &DisplayedTiles[N];

	Tiles->push_back(bBlock);
		
	bBlock->bVisible |= (1 << N);

	NeedToRender[1] = RENDER_NEED;
	NeedToRender[0] = RENDER_NEED;
}

void Chunk::HideTile(Block *bBlock, char N)
{
	if(!bBlock) return;
	if(bBlock->cMaterial == MAT_NO) return;
	if(!(bBlock->bVisible & (1 << N))) return;

	std::list<Block *> *Tiles;
	if(bBlock->cMaterial == MAT_WATER)
		Tiles = &DisplayedWaterTiles[N];
	else
		Tiles = &DisplayedTiles[N];

	auto it = Tiles->begin();

	while(it != Tiles->end())
	{
		if(*it == bBlock) break;
		++it;
	}
	if(it == Tiles->end()) return;

	(*it)->bVisible &= ~(1 << N);
	Tiles->erase(it);

	NeedToRender[1] = RENDER_NEED;
	NeedToRender[0] = RENDER_NEED;
}

char Chunk::GetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z)
{
	if((x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
	{
		return -1;
	}
	return bBlocks[x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ].cMaterial;
}

int Chunk::SetBlockMaterial(BlockInChunk x, BlockInChunk y, BlockInChunk z, char cMat)
{
	if((x >= CHUNK_SIZE_XZ)||(z >= CHUNK_SIZE_XZ)||(y >= CHUNK_SIZE_Y))
		return -1;

	int index = GetIndexByPosition(x, y, z);
	bBlocks[index].cMaterial = cMat;
	return index;
}

int Chunk::GetBlockPositionByPointer(Block *tCurrentBlock, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z) 
	const
{
	int t = tCurrentBlock - bBlocks;

	if(GetBlockPositionByIndex(t, x, y, z) == -1) 
		return -1;

	return 0;
}

inline int Chunk::GetBlockPositionByIndex(int index, BlockInChunk *x, BlockInChunk *y, BlockInChunk *z)
{
	if((index < 0)||(index >= CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y))
		return -1;

	*z  = index%CHUNK_SIZE_XZ;
	index/=CHUNK_SIZE_XZ;
	*x = index%CHUNK_SIZE_XZ;
	index/=CHUNK_SIZE_XZ;
	*y = index;

	return 0;
}

int Chunk::GetIndexByPosition(BlockInChunk x, BlockInChunk y, BlockInChunk z)
{
	return x*CHUNK_SIZE_XZ + z + y*CHUNK_SIZE_XZ*CHUNK_SIZE_XZ;
}

void Chunk::DrawLoadedBlocks()
{
	int index = 0;
	BlockInChunk xx, yy, zz;

	while(index < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y)
	{
		GetBlockPositionByPointer(bBlocks + index, &xx, &yy, &zz);

		if(bBlocks[index].cMaterial != MAT_NO)
		{
			if(bBlocks[index].cMaterial == MAT_WATER)
			{
				if((GetBlockMaterial(xx, yy + 1, zz) == MAT_NO)||(yy == CHUNK_SIZE_Y - 1)) ShowTile(bBlocks + index, TOP);
				if(GetBlockMaterial(xx, yy - 1, zz) == MAT_NO) ShowTile(bBlocks + index, BOTTOM);
				if(GetBlockMaterial(xx + 1, yy, zz) == MAT_NO) ShowTile(bBlocks + index, RIGHT);
				if(GetBlockMaterial(xx - 1, yy, zz) == MAT_NO) ShowTile(bBlocks + index, LEFT);
				if(GetBlockMaterial(xx, yy, zz + 1) == MAT_NO) ShowTile(bBlocks + index, BACK);
				if(GetBlockMaterial(xx, yy, zz - 1) == MAT_NO) ShowTile(bBlocks + index, FRONT);
			}
			else
			{
				if((GetBlockMaterial(xx, yy + 1, zz) == MAT_NO)||(GetBlockMaterial(xx, yy + 1, zz) == MAT_WATER)||(yy == CHUNK_SIZE_Y - 1))
					ShowTile(bBlocks + index, TOP);
				if((GetBlockMaterial(xx, yy - 1, zz) == MAT_NO)||(GetBlockMaterial(xx, yy - 1, zz) == MAT_WATER))
					ShowTile(bBlocks + index, BOTTOM);
				if((GetBlockMaterial(xx + 1, yy, zz) == MAT_NO)||(GetBlockMaterial(xx + 1, yy, zz) == MAT_WATER))
					ShowTile(bBlocks + index, RIGHT);
				if((GetBlockMaterial(xx - 1, yy, zz) == MAT_NO)||(GetBlockMaterial(xx - 1, yy, zz) == MAT_WATER))
					ShowTile(bBlocks + index, LEFT);
				if((GetBlockMaterial(xx, yy, zz + 1) == MAT_NO)||(GetBlockMaterial(xx, yy, zz + 1) == MAT_WATER))
					ShowTile(bBlocks + index, BACK);
				if((GetBlockMaterial(xx, yy, zz - 1) == MAT_NO)||(GetBlockMaterial(xx, yy, zz - 1) == MAT_WATER))
					ShowTile(bBlocks + index, FRONT);
			}
		}
		index++;
	}
}

void Chunk::Generate()
{
	wWorld.lLandscape.Generate(*this);
	//wWorld.lLandscape.Fill(*this, 0, 0.999, 64);
	//wWorld.lLandscape.Fill(*this, MAT_GRASS, 1, 64);
	//ChunkPosition pos = {x, z};
	//wWorld.lLandscape.Load(pos);
}

void Chunk::FillSkyLight(char bright)
{
	BlockInChunk y;
	int index;

	for(int i = 0; i < CHUNK_SIZE_XZ*CHUNK_SIZE_XZ*CHUNK_SIZE_Y; i++)
	{
		SkyLight[i] = 0;
	}

	for(BlockInChunk x = 0; x < CHUNK_SIZE_XZ; x++)
	{
		for(BlockInChunk z = 0; z < CHUNK_SIZE_XZ; z++)
		{
			y = CHUNK_SIZE_Y - 1;
			while (y > 0)
			{
				index = GetIndexByPosition(x, y, z);
				if(bBlocks[index].cMaterial != MAT_NO)
					break;

				SkyLight[index] = bright;
				y--;
			}
		}
	}
}

void Chunk::Render(GLenum mode, char mat)
{
	if(!listgen)
	{
		// 1 - solid tiles
		// 2 - water tiles
		RenderList = glGenLists(2);
		listgen = true;
	}

	int pointertorender = 0;
	if(mat == MAT_WATER)
		pointertorender = 1;

	if(NeedToRender[pointertorender] == RENDER_NEED)
	{
		mode = GL_COMPILE;
		NeedToRender[pointertorender] = RENDER_TRY;
	}

	if((mode == GL_COMPILE)||(mode == GL_COMPILE_AND_EXECUTE))
	{
		glNewList(RenderList + pointertorender, mode);

		std::list<Block *> *Tiles;
		static GLfloat br;
		static BlockInChunk cx, cy, cz;
		static BlockInWorld xx, yy, zz;
		static BlockInWorld blckwx, blckwz;
		static BlockInWorld xlight, ylight, zlight;
		static BlockInChunk 
			xloclight, 
			yloclight, 
			zloclight;
		static Chunk *temploc;

		glBegin(GL_QUADS);

		for(int i = 0; i < 6; i++)
		{
			blckwx = x*CHUNK_SIZE_XZ;
			blckwz = z*CHUNK_SIZE_XZ;


			if(mat == MAT_WATER)
				Tiles = &DisplayedWaterTiles[i];
			else
				Tiles = &DisplayedTiles[i];

			auto it = Tiles->begin();

			while(it != Tiles->end())
			{
				GetBlockPositionByPointer(*it, &cx, &cy, &cz);

				xx = cx + blckwx;
				yy = cy;
				zz = cz + blckwz;
// 
// 				switch(i)
// 				{
// 				case TOP:
// 					xlight = cx;
// 					ylight = cy + 1;
// 					zlight = cz;
// 					break;
// 				case BOTTOM:
// 					xlight = cx;
// 					ylight = cy - 1;
// 					zlight = cz;
// 					break;
// 				case RIGHT:
// 					xlight = cx + 1;
// 					ylight = cy;
// 					zlight = cz;
// 					break;
// 				case LEFT:
// 					xlight = cx - 1;
// 					ylight = cy;
// 					zlight = cz;
// 					break;
// 				case FRONT:
// 					xlight = cx;
// 					ylight = cy;
// 					zlight = cz - 1;
// 					break;
// 				case BACK:
// 					xlight = cx;
// 					ylight = cy;
// 					zlight = cz + 1;
// 					break;
// 				}
// 			
// 				if((xlight >= CHUNK_SIZE_XZ)||(xlight < 0)||(zlight >= CHUNK_SIZE_XZ)||(zlight < 0))
// 					temploc = wWorld.GetChunkByBlock(xlight + blckwx, zlight + blckwz);
// 				else temploc = this;
// 				if((ylight >= CHUNK_SIZE_Y)||(ylight < 0)) temploc = NULL;
// 				if (temploc)
// 				{
// 					wWorld.GetPosInChunkByWorld(xlight, ylight, zlight, &xloclight, &yloclight, &zloclight);
// 					int index = temploc->GetIndexByPosition(xloclight, yloclight, zloclight);
// 					//wWorld.lLocations.begin()->GetIndexByPosition(sXcoord, sXcoord, sXcoord);
// 
// 					br = LightTable[temploc->SkyLight[index]];
// 				}else br = 0.0f;
// 				glColor3f(br, br, br);

	// 			if(	(abs(xx*BLOCK_SIZE - player.dPositionX) < MAX_VIEV_DIST + 10*BLOCK_SIZE) && 
	// 				(abs(yy*BLOCK_SIZE - player.dPositionY) < MAX_VIEV_DIST + 10*BLOCK_SIZE) && 
	// 				(abs(zz*BLOCK_SIZE - player.dPositionZ) < MAX_VIEV_DIST + 10*BLOCK_SIZE))
					DrawTile(xx, yy, zz, *it, i);

				++it;
			}
		}
		glEnd();
		glEndList();


		if(NeedToRender[pointertorender] == RENDER_TRY)
			NeedToRender[pointertorender] = RENDER_NO_NEED;
	}

	if(mode != GL_COMPILE_AND_EXECUTE)
	{
		glCallList(RenderList + pointertorender);
	}
}

void Chunk::DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, Block* block, char N)
{
	GLdouble 
		// 		dXcoord = (sXcoord-player.lnwPositionX*LOCATION_SIZE_XZ)*TILE_SIZE, 
		// 		dYcoord = sYcoord*TILE_SIZE, 
		// 		dZcoord = (sZcoord-player.lnwPositionZ*LOCATION_SIZE_XZ)*TILE_SIZE;
		dXcoord = sXcoord*BLOCK_SIZE, 
		dYcoord = sYcoord*BLOCK_SIZE, 
		dZcoord = sZcoord*BLOCK_SIZE;

	dXcoord -= BLOCK_SIZE/2;
	dZcoord -= BLOCK_SIZE/2;

	static double space = 0.0005;
	static double offsetx = 0;
	static double offsety = 0;

	char covered = block->bVisible;
	char material = block->cMaterial;

	wWorld.MaterialLib.GetTextureOffsets(offsetx, offsety, material, covered, N);
	
	switch(N)
	{
	case TOP:
		{
			//Верхняя грань
			GetBrightVertex(sXcoord, sYcoord, sZcoord, 0);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 1);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 2);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 3);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BOTTOM:
		{
			//Нижняя грань
			GetBrightVertex(sXcoord, sYcoord, sZcoord, 4);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 7);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 6);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 5);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case RIGHT:
		{
			//Правая грань
			GetBrightVertex(sXcoord, sYcoord, sZcoord, 7);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 3);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 2);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 6);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case LEFT:
		{
			//Левая грань
			GetBrightVertex(sXcoord, sYcoord, sZcoord, 4);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 5);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 1);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 0);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BACK:
		{
			//Задняя грань
			GetBrightVertex(sXcoord, sYcoord, sZcoord, 5);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 6);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 2);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 1);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
		}break;
	case FRONT:
		{
			//Передняя грань
			GetBrightVertex(sXcoord, sYcoord, sZcoord, 4);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 0);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 3);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);

			GetBrightVertex(sXcoord, sYcoord, sZcoord, 7);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
		}break;
	}
}

void Chunk::GetBrightVertex( BlockInWorld X, BlockInWorld Y, BlockInWorld Z, int vertex)
{
	float res = 0;

	if (vertex == 0)
	{
		int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
		int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
		int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}
	else if (vertex == 1)
	{
		int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
		int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
		int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}
	else if (vertex == 2)
	{
		int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
		int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
		int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}
	else if (vertex == 3)
	{
		int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
		int yy[8] = {0, 0, 0, 0, 1, 1, 1, 1};
		int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}
	else if (vertex == 4)
	{
		int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
		int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
		int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}
	else if (vertex == 5)
	{
		int xx[8] = {0, 0,-1,-1, 0, 0,-1,-1};
		int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
		int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}
	else if (vertex == 6)
	{
		int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
		int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
		int zz[8] = {0, 1, 0, 1, 0, 1, 0, 1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}
	else if (vertex == 7)
	{
		int xx[8] = {0, 0, 1, 1, 0, 0, 1, 1};
		int yy[8] = {0, 0, 0, 0,-1,-1,-1,-1};
		int zz[8] = {0,-1, 0,-1, 0,-1, 0,-1};
		res = GetBrightAverage(X, Y, Z, xx, yy, zz);
	}

	glColor3f(res, res, res);
}

float Chunk::GetBrightAverage(BlockInWorld X, BlockInWorld Y, BlockInWorld Z, int xx[8], int yy[8], int zz[8])
{
	float mat[8];
	Chunk *temploc;
	float res = 0;

	static BlockInChunk 
		xloclight, 
		yloclight, 
		zloclight;

	for(int i = 0; i < 8; i++)
	{
		temploc = wWorld.GetChunkByBlock(X + xx[i], Z + zz[i]);

		//if((ylight >= CHUNK_SIZE_Y)||(ylight < 0)) temploc = NULL;
		if (temploc)
		{
			wWorld.GetPosInChunkByWorld(X + xx[i], Y + yy[i], Z + zz[i], &xloclight, &yloclight, &zloclight);
			int index = temploc->GetIndexByPosition(xloclight, yloclight, zloclight);

			mat[i] = LightTable[temploc->SkyLight[index]];
		}else mat[i] = 0.0f;
	}

	for(int i = 0; i < 8; i++)
		res += mat[i];

	return res /= 4;
}
