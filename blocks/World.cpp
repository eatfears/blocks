#include "World.h"
#include "Blocks_Definitions.h"
#include "Mutex.h"

extern Mutex VisibleListAccessMutex;

World::World()
{

}

World::~World()
{

}

void World::AddLocation(LocInWorld x, LocInWorld z)
{
	Location loc(x, z, &MaterialLib);
	lLocations.push_front(loc);
}

void World::BuildWorld()
{
	MaterialLib.InitMaterials();

	AddLocation(0,0);
	AddLocation(0,-1);
	AddLocation(0,1);
	AddLocation(-1,0);
	AddLocation(1,0);

	for (int j = 0; j < 12; j++)
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
	/*
	for (int k = 20; k > 0; k--)
	{
		//AddTile(i, j, k, MAT_GRASS, true);
		//if(rand()%2)AddTile(0, k, 0, rand()%4+1, true);
	}
	//StartBuilding();

	//AddTile(0,3,0,MAT_GRASS, true);
	//AddTile(0,1,0,MAT_DIRT, true);
	//AddTile(0,0,0,MAT_STONE, true);
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

void World::GetPosInLocByWorld( TileInWorld x, TileInWorld y, TileInWorld z, TileInLoc *locx, TileInLoc *locy, TileInLoc *locz )
{
	while(x < 0) x += LOCATION_SIZE_XZ;
	while(z < 0) z += LOCATION_SIZE_XZ;
	*locx = x%LOCATION_SIZE_XZ;
	*locy = y;
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

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	
	if (show)
	{
		//VisibleListAccessMutex.Acquire();
		index = loc->AddTile(locx, locy, locz, mat);

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
		//VisibleListAccessMutex.Release();
	}
	else loc->AddTile(locx, locy, locz, mat);

	return 1;
}

int World::RemoveTile(TileInWorld x, TileInWorld y, TileInWorld z, bool show)
{
	if ((y < 0)||(y >= LOCATION_SIZE_Y)) return 0;

	if(!FindTile(x, y, z)) return 0;

	Location *loc = GetLocByTile(x, z);
	int index; 

	if (loc == NULL) return 0;

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	index = loc->GetIndexByPosition(locx, locy, locz);

	if (show)
	{
		//VisibleListAccessMutex.Acquire();

		Location *lTempLoc = 0;
		int iTempIndex;
		if(!FindTile(x, y + 1, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, TOP);
		else ShowTile(lTempLoc, iTempIndex, DOWN);
		if(!FindTile(x, y - 1, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, DOWN);
		else ShowTile(lTempLoc, iTempIndex, TOP);
		if(!FindTile(x + 1, y, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, RIGHT);
		else ShowTile(lTempLoc, iTempIndex, LEFT);
		if(!FindTile(x - 1, y, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, LEFT);
		else ShowTile(lTempLoc, iTempIndex, RIGHT);
		if(!FindTile(x, y, z + 1, &lTempLoc, &iTempIndex)) HideTile(loc, index, BACK);
		else ShowTile(lTempLoc, iTempIndex, FRONT);
		if(!FindTile(x, y, z - 1, &lTempLoc, &iTempIndex)) HideTile(loc, index, FRONT);
		else ShowTile(lTempLoc, iTempIndex, BACK);

		loc->RemoveTile(locx, locy, locz);
		//VisibleListAccessMutex.Release();
	}
	else loc->RemoveTile(locx, locy, locz);

	return 1;
}

void World::ShowTile(Location *loc, int index, char N)
{
	if(!loc->tTile[index].bVisible[N])
	{
		loc->ShowTile(loc->tTile + index, N);
	}
}

void World::HideTile(Location *loc, int index, char N)
{
	if(loc->tTile[index].bVisible[N])
	{
		loc->HideTile(loc->tTile + index, N);
	}
}
 
int World::FindTile(TileInWorld x, TileInWorld y, TileInWorld z, Location **loc, int *index)
{
	if ((y < 0)||(y >= LOCATION_SIZE_Y)) { loc = NULL; *index = 0; return 0;}

	(*loc) = GetLocByTile(x, z);

	if ((*loc) == NULL) { *index = 0; return 0;}

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

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

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	if (loc->GetTileMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}
