#include "World.h"
#include "Blocks_Definitions.h"
#include "Mutex.h"

extern Mutex VisibleListAccessMutex;

World::World()
{
//	tTiles = new Tiles[0x100000];
//	DisplayedTiles = new std::list<Tile *>[6];
//	skipbuild = false;
}

World::~World()
{
}

void World::AddLocation(LocInWorld x, LocInWorld z)
{
	Location loc(x, z);
	lLocations.push_front(loc);
}

void World::BuildWorld()
{
	MaterialLib.InitMaterials();

	AddLocation(0,0);
	AddLocation(0,-1);

	//auto it = lLocations.begin();
	/*it->AddTile(0,0,0, MAT_GRASS, true);
	it->AddTile(1,0,0, MAT_GRASS, true);
	it->AddTile(1,0,1, MAT_GRASS, true);
	it->AddTile(1,1,1, MAT_GRASS, true);
	*/
	
	for (int j = 0; j < 129; j++)
	{
		for (int i = 0; i < 160; i++)
		{
			for (int k = 0; k < 160; k++)
			{
				//AddTile(i, j, k, MAT_GRASS, true);
				AddTile(i, j, k, rand()%4+1, true);
			}
		}
	}
	/**/
// 
// 	for (int i = 0; i < 6; i++)
// 		for (int j = 0; j < MaterialLib.iNumberOfTextures; j++)
// 		{
// 			MaterialLib.TexurePointerInVisible[i][j] = DisplayedTiles[i].end();
// 		}

	//StartBuilding();
	/*
	AddTile(0,-1,0,MAT_STONE);
	AddTile(3,-1,0,MAT_STONE);
	AddTile(2,-1,0,MAT_STONE);
	AddTile(1,-1,0,MAT_STONE);
	AddTile(0,-1,1,MAT_STONE);
	AddTile(0,-1,2,MAT_STONE);
	AddTile(0,-1,3,MAT_STONE);
	/**/

	/*
	AddTile(10,0,0,MAT_DIRT);
	AddTile(0,1,0,MAT_DIRT);
	AddTile(0,-3,0,MAT_STONE);
	AddTile(1,0,0,MAT_STONE);
	AddTile(-1,0,0,MAT_STONE);
	/**/
	/*
	for (int i = 0 ; i< 40 ; i++)
	{
		for (int k = 0 ; k < 1 ; k++)
		{
			AddTile(rand()%100-50,k,rand()%100-50,MAT_STONE);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_SAND);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_DIRT);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_GRASS);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_NO);
		}
	}
	/**/
	//for (int i = 0; i < 10; i++) AddTile(i,-1,0,rand()%4+1);

	/*
	AddTile(1,-1,0,MAT_DIRT);
	AddTile(2,-1,0,MAT_STONE);
	AddTile(3,-1,0,MAT_STONE);
	AddTile(4,-1,0,MAT_DIRT);
	/**/

	//DrawLoadedTiles();
}

void World::GetLocByTile( TileInWorld x, TileInWorld z, LocInWorld *locx, LocInWorld *locz )
{
	*locx = floor((double)x/LOCATION_SIZE_XZ);
	*locz = floor((double)z/LOCATION_SIZE_XZ);
}

void World::GetPosInLocByWorld( TileInWorld x, TileInWorld z, TileInLoc *locx, TileInLoc *locz )
{
	while(x < 0) x += LOCATION_SIZE_XZ;
	while(z < 0) z += LOCATION_SIZE_XZ;
	*locx = x%LOCATION_SIZE_XZ;
	*locz = z%LOCATION_SIZE_XZ;
}

Location* World::GetLocByTile( TileInWorld x, TileInWorld z )
{
	LocInWorld locx, locz;
	GetLocByTile(x, z, &locx, &locz );

	auto loc = lLocations.begin();

	while(loc != lLocations.end())
	{
		if((loc->x == locx)&&(loc->z == locz)) break;
		++loc;
	}
	if(loc == lLocations.end())
		return NULL;

	return &*loc;
}

// void World::DrawLoadedTiles()
// {
// 	if(skipbuild) {building = true; return;}
// 	else building = false;
// 
// 	VisibleListAccessMutex.Acquire();
// 	skipbuild = true;
// 	signed short x, y, z;
// 	for (int i = 0; i < 0x100000; i++)
// 	{
// 		Tiles::iterator it = tTiles[i].begin();
// 
// 		while(it != tTiles[i].end())
// 		{
// 			x = it->sCoordX;
// 			y = it->sCoordY;
// 			z = it->sCoordZ;
// 
// 			if(!FindTile(x, y + 1, z)) ShowTile(&*it,TOP);
// 			else HideTile(x, y + 1, z, DOWN);
// 			if(!FindTile(x, y - 1, z)) ShowTile(&*it,DOWN);
// 			else HideTile(x, y - 1, z, TOP);
// 			if(!FindTile(x + 1, y, z)) ShowTile(&*it,RIGHT);
// 			else HideTile(x + 1, y, z, LEFT);
// 			if(!FindTile(x - 1, y, z)) ShowTile(&*it,LEFT);
// 			else HideTile(x - 1, y, z, RIGHT);
// 			if(!FindTile(x, y, z + 1)) ShowTile(&*it,BACK);
// 			else HideTile(x, y, z + 1, FRONT);
// 			if(!FindTile(x, y, z - 1)) ShowTile(&*it,FRONT);
// 			else HideTile(x, y, z - 1, BACK);
// 			it++;
// 		}
// 	}
// 	skipbuild = false;
// 
// 	VisibleListAccessMutex.Release();
// 
// 	if (building)
// 		DrawLoadedTiles();
// }

int World::AddTile(TileInWorld x, TileInWorld y, TileInWorld z, char mat, bool show)
{
	if ((y < 0)||(y >= LOCATION_SIZE_Y)) return 0;

	if(FindTile(x, y, z)) return 0;

	Location *loc = GetLocByTile(x, z);
	int index; 

	if (loc == NULL) return 0;

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, z, &locx, &locz);
	locy = y;


	index = loc->AddTile(locx, locy, locz, mat);
	
	if (show)
	{
		Location *lTempLoc = 0;
		int iTempIndex;
		if(!FindTile(x, y + 1, z, &lTempLoc, &iTempIndex)) ShowTile(loc, index, TOP);
		else HideTile(lTempLoc, iTempIndex, DOWN);
		if(!FindTile(x, y - 1, z, &lTempLoc, &iTempIndex)) ShowTile(loc, index, DOWN);
		else HideTile(lTempLoc, iTempIndex, TOP);
		if(!FindTile(x + 1, y, z, &lTempLoc, &iTempIndex)) ShowTile(loc, index, RIGHT);
		else HideTile(lTempLoc, iTempIndex, LEFT);
		if(!FindTile(x - 1, y, z, &lTempLoc, &iTempIndex)) ShowTile(loc, index, LEFT);
		else HideTile(lTempLoc, iTempIndex, RIGHT);
		if(!FindTile(x, y, z + 1, &lTempLoc, &iTempIndex)) ShowTile(loc, index, BACK);
		else HideTile(lTempLoc, iTempIndex, FRONT);
		if(!FindTile(x, y, z - 1, &lTempLoc, &iTempIndex)) ShowTile(loc, index, FRONT);
		else HideTile(lTempLoc, iTempIndex, BACK);
	}
	
	/*
	if (show)
	{
		//VisibleListAccessMutex.Acquire();
		tTiles[bin].push_front(t);

		if(!FindTile(x, y + 1, z)) ShowTile(&(*tTiles[bin].begin()),TOP);
		else HideTile(x, y + 1, z, DOWN);
		if(!FindTile(x, y - 1, z)) ShowTile(&(*tTiles[bin].begin()),DOWN);
		else HideTile(x, y - 1, z, TOP);
		if(!FindTile(x + 1, y, z)) ShowTile(&(*tTiles[bin].begin()),RIGHT);
		else HideTile(x + 1, y, z, LEFT);
		if(!FindTile(x - 1, y, z)) ShowTile(&(*tTiles[bin].begin()),LEFT);
		else HideTile(x - 1, y, z, RIGHT);
		if(!FindTile(x, y, z + 1)) ShowTile(&(*tTiles[bin].begin()),BACK);
		else HideTile(x, y, z + 1, FRONT);
		if(!FindTile(x, y, z - 1)) ShowTile(&(*tTiles[bin].begin()),FRONT);
		else HideTile(x, y, z - 1, BACK);
		//VisibleListAccessMutex.Release();
	}
	else tTiles[bin].push_front(t);
	/**/
	return 1;
}

// int World::RemoveTile(signed short x, signed short y, signed short z, bool show)
// {
// 	unsigned long bin = ComputeBin(x, y, z);
// 	Tiles::iterator it = tTiles[bin].begin();
// 
// 	while(it != tTiles[bin].end())
// 	{
// 		if ((it->sCoordZ == z)&&(it->sCoordX == x)&&(it->sCoordY == y)) break;
// 		it++;
// 	}
// 	if (it == tTiles[bin].end()) return 0;
// 	
// 	if (show)
// 	{
// 		//VisibleListAccessMutex.Acquire();
// 		Tile *temp;
// 		temp = FindTile(x, y + 1, z); 
// 		if(!temp) HideTile(x, y, z, 0);
// 		else ShowTile(temp, 1);
// 		temp = FindTile(x, y - 1, z); 
// 		if(!temp) HideTile(x, y, z, 1);
// 		else ShowTile(temp, 0);
// 		temp = FindTile(x + 1, y, z); 
// 		if(!temp) HideTile(x, y, z, 2);
// 		else ShowTile(temp, 3);
// 		temp = FindTile(x - 1, y, z); 
// 		if(!temp) HideTile(x, y, z, 3);
// 		else ShowTile(temp, 2);
// 		temp = FindTile(x, y, z + 1); 
// 		if(!temp) HideTile(x, y, z, 4);
// 		else ShowTile(temp, 5);
// 		temp = FindTile(x, y, z - 1); 
// 		if(!temp) HideTile(x, y, z, 5);
// 		else ShowTile(temp, 4);
// 		
// 		tTiles[bin].erase(it);
// 		//VisibleListAccessMutex.Release();
// 	}
// 	else 
// 	{
// 		VisibleListAccessMutex.Acquire();
// 
// 		for(int N = 0; N < 6; N++)
// 			if(it->bVisible[N]) HideTile(x, y, z, N);
// 		tTiles[bin].erase(it);
// 
// 		VisibleListAccessMutex.Release();
// 	}
// 
// 	return 1;
// }

void World::ShowTile(Location *loc, int index, char N)
{
	if(!loc->tTile[index].bVisible[N])
	{

		//int iTex = MaterialLib.mMaterial[loc->tTile[index].cMaterial].iTexture[N];
		//auto it = MaterialLib.TexurePointerInVisible[N][iTex];

		//MaterialLib.TexurePointerInVisible[N][iTex] = DisplayedTiles[N].insert(it, tTile);
		
		loc->ShowTile(loc->tTile + index, N);
		//loc->tTile[index].bVisible[N] = true;
	}
	//else for(;;);
}

void World::HideTile(Location *loc, int index, char N)
{
	if(loc->tTile[index].bVisible[N])
	{
		loc->HideTile(loc->tTile + index, N);
	}


// 	Tile *tTile = FindTile(x, y, z);
// 	if (!tTile) return;
// 	if (tTile->bVisible[N] == false) return;
// 
// 	int iTex = MaterialLib.mMaterial[tTile->cMaterial].iTexture[N];
// 	auto it = begin(DisplayedTiles[N]);
// 	auto it = MaterialLib.TexurePointerInVisible[N][iTex];

// 	while(it != DisplayedTiles[N].end())
// 	{
// 		if(*it == tTile) break;
// 		//if (((*it)->sCoordZ == z)&&((*it)->sCoordX == x)&&((*it)->sCoordY == y)) break;
// 		it++;
// 	}
// 	if (it == DisplayedTiles[N].end()) return;
// 
// 	(*it)->bVisible[N] = false;
// 
// 	//int iTex = MaterialLib.mMaterial[(*it)->cMaterial].iTexture[N];
// 
// 	auto it2 = MaterialLib.TexurePointerInVisible[N][iTex];
// 	Tile *t1 = (*it), *t2 = (*it2);
// 	
// 	if(t1 == t2)
// 	{
// 		++MaterialLib.TexurePointerInVisible[N][iTex];
// 		
// 		if(MaterialLib.TexurePointerInVisible[N][iTex] != DisplayedTiles[N].end())
// 		if((*MaterialLib.TexurePointerInVisible[N][iTex])->cMaterial != t1->cMaterial)
// 			MaterialLib.TexurePointerInVisible[N][iTex] = DisplayedTiles[N].end();
// 	}
// 
// 	DisplayedTiles[N].erase(it);
	return;
}
 
int World::FindTile(TileInWorld x, TileInWorld y, TileInWorld z, Location **loc, int *index)
{
	if ((y < 0)||(y >= LOCATION_SIZE_Y)) { loc = NULL; *index = 0; return 0;}

	(*loc) = GetLocByTile(x, z);

	if ((*loc) == NULL) { *index = 0; return 0;}

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, z, &locx, &locz);
	locy = y;

	*index = locx*LOCATION_SIZE_XZ + locz + locy*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ;

	if ((*loc)->GetTileMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}

int World::FindTile(TileInWorld x, TileInWorld y, TileInWorld z)
{
	if (y < 0) return 0;

	Location *loc = GetLocByTile(x, z);

	if (loc == NULL) return 0;

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, z, &locx, &locz);
	locy = y;

	if (loc->GetTileMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}
// 
// unsigned long World::ComputeBin(signed short x, signed short y, signed short z)
// {
// 	return (x & 0xff) + ((z & 0xff)<<8) + ((y & 0x0f)<<16);
// }
