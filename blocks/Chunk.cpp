#include "Chunk.h"
#include "World.h"

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
			bBlocks[i].bVisible[N] = false;
	}
	NeedToRender = false;
	listgen = false;

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
	
	return index;
}

int Chunk::RemoveBlock(BlockInChunk x, BlockInChunk y, BlockInChunk z)
{
	if((GetBlockMaterial(x, y, z) == MAT_NO)||(GetBlockMaterial(x, y, z) == -1)) return -1;

	BlockInChunk index = SetBlockMaterial(x, y, z, MAT_NO);

	return index;
}

void Chunk::ShowTile(Block *bBlock, char N)
{
	if(!bBlock) return;
	if(bBlock->cMaterial == MAT_NO) return;
	if(bBlock->bVisible[N]) return;
	

	std::list<Block *> *Tiles;
	if(bBlock->cMaterial == MAT_WATER)
		Tiles = &DisplayedWaterTiles[N];
	else
		Tiles = &DisplayedTiles[N];

	Tiles->push_back(bBlock);
		
	bBlock->bVisible[N] = true;
	
	NeedToRender = true;
}

void Chunk::HideTile(Block *bBlock, char N)
{
	if(!bBlock) return;
	if(bBlock->cMaterial == MAT_NO) return;
	if(!bBlock->bVisible[N]) return;

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

	(*it)->bVisible[N] = false;
	Tiles->erase(it);

	NeedToRender = true;
	return;
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
				if(GetBlockMaterial(xx, yy + 1, zz) == MAT_NO) ShowTile(bBlocks + index, TOP);
				if(GetBlockMaterial(xx, yy - 1, zz) == MAT_NO) ShowTile(bBlocks + index, BOTTOM);
				if(GetBlockMaterial(xx + 1, yy, zz) == MAT_NO) ShowTile(bBlocks + index, RIGHT);
				if(GetBlockMaterial(xx - 1, yy, zz) == MAT_NO) ShowTile(bBlocks + index, LEFT);
				if(GetBlockMaterial(xx, yy, zz + 1) == MAT_NO) ShowTile(bBlocks + index, BACK);
				if(GetBlockMaterial(xx, yy, zz - 1) == MAT_NO) ShowTile(bBlocks + index, FRONT);
			}
			else
			{
				if((GetBlockMaterial(xx, yy + 1, zz) == MAT_NO)||(GetBlockMaterial(xx, yy + 1, zz) == MAT_WATER))
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
}

void Chunk::FillSkyLight(char bright)
{
	BlockInChunk y;
	int index;

	for(BlockInChunk x = 0; x < CHUNK_SIZE_XZ; x++)
	{
		for(BlockInChunk z = 0; z < CHUNK_SIZE_XZ; z++)
		{
			y = CHUNK_SIZE_Y - 1;
			while (y >= 0)
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

	if(NeedToRender)
	{
		mode = GL_COMPILE;

		if(mat == MAT_WATER)
			NeedToRender = false;
	}

	if((mode == GL_COMPILE)||(mode == GL_COMPILE_AND_EXECUTE))
	{
		if(mat == MAT_WATER)
			glNewList(RenderList + 1, mode);
		else
			glNewList(RenderList, mode);

		std::list<Block *> *Tiles;
		static GLfloat br;
		static BlockInChunk cx, cy, cz;
		static BlockInWorld xx, yy, zz;
		static BlockInWorld locx, locz;
		static BlockInWorld xlight, ylight, zlight;
		static BlockInChunk 
			xloclight, 
			yloclight, 
			zloclight;
		static Chunk *temploc;

		glBegin(GL_QUADS);

		for(int i = 0; i < 6; i++)
		{
			locx = x*CHUNK_SIZE_XZ;
			locz = z*CHUNK_SIZE_XZ;


			if(mat == MAT_WATER)
				Tiles = &DisplayedWaterTiles[i];
			else
				Tiles = &DisplayedTiles[i];

			auto it = Tiles->begin();

			while(it != Tiles->end())
			{
				GetBlockPositionByPointer(*it, &cx, &cy, &cz);

				xx = cx + locx;
				yy = cy;
				zz = cz + locz;

				switch(i)
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
					temploc = wWorld.GetChunkByBlock(xlight + locx, zlight + locz);
				else temploc = this;
				if(temploc)
				{
					wWorld.GetPosInChunkByWorld(xlight, ylight, zlight, &xloclight, &yloclight, &zloclight);
					int index = temploc->GetIndexByPosition(xloclight, yloclight, zloclight);
					//wWorld.lLocations.begin()->GetIndexByPosition(sXcoord, sXcoord, sXcoord);

					br = 0.2f + temploc->SkyLight[index];
					glColor3f(br, br, br);
				}else glColor3f(0.0f, 0.0f, 0.0f);
			
	// 			if(	(abs(xx*BLOCK_SIZE - player.dPositionX) < MAX_VIEV_DIST + 10*BLOCK_SIZE) && 
	// 				(abs(yy*BLOCK_SIZE - player.dPositionY) < MAX_VIEV_DIST + 10*BLOCK_SIZE) && 
	// 				(abs(zz*BLOCK_SIZE - player.dPositionZ) < MAX_VIEV_DIST + 10*BLOCK_SIZE))
					DrawTile(xx, yy, zz, (*it)->cMaterial, i);

				++it;
			}
		}
		glEnd();

		glEndList();
	}

	if(mode != GL_COMPILE_AND_EXECUTE)
	{
		if(mat == MAT_WATER)
			glCallList(RenderList + 1);
		else
			glCallList(RenderList);
	}
}

void Chunk::DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char N)
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

	wWorld.MaterialLib.GetTextureOffsets(offsetx, offsety, material, N);

	switch(N)
	{
	case TOP:
		{
			//Верхняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BOTTOM:
		{
			//Нижняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case RIGHT:
		{
			//Правая грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case LEFT:
		{
			//Левая грань
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BACK:
		{
			//Задняя грань
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
		}break;
	case FRONT:
		{
			//Передняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
		}break;
	}
}