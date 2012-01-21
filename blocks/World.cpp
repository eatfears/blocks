#include "World.h"
#include "Blocks_Definitions.h"

World::World()
{
	skipbuild = false;


	parget = CreateEvent(NULL, false, false, NULL);
	mutex = CreateMutex(NULL, false, NULL);
}

World::~World()
{
	lLocations.clear();
}

Location* World::AddLocation(LocInWorld x, LocInWorld z)
{
	Location lLoc(x, z, &MaterialLib);

	auto locIterator = lLocations.begin();

	while(locIterator != lLocations.end())
	{
		if((locIterator->x == x)&&(locIterator->z == z)) return NULL;
		++locIterator;
	}

	locIterator = lLocations.insert(locIterator,lLoc);
	return &*locIterator;
}

void World::BuildWorld()
{
	MaterialLib.InitMaterials();

	/*
	for(int i = -10; i < 10; i++)
	for(int j = -10; j < 10; j++)
	{
		AddLocation(i,j);
	}
	//AddLocation(0,0);

	for(int j = 0; j < 20; j++)
	{
		for(int i = -60; i < 60; i++)
		{
			for(int k = -60; k < 60; k++)
			{
				AddTile(i, j, k, MAT_GRASS, true);
				//AddTile(i, j, k, rand()%4+1, true);
			}
		}
	}
	/**/
	/*
	for(int k = 20; k > 0; k--)
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
	for(int i = 0 ; i< 40 ; i++)
	{
		for(int k = 0 ; k < 1 ; k++)
		{
			AddTile(rand()%100-50,k,rand()%100-50,MAT_STONE);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_SAND);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_DIRT);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_GRASS);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_NO);
		}
	}
	/**/
	//for(int i = 0; i < 10; i++) AddTile(i,-1,0,rand()%4+1);

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

void World::DrawLoadedTiles(Location *loc)
{
	//if(skipbuild) {building = true; return;}
	//else building = false;

	//VisibleListAccessMutex.Acquire();
	//skipbuild = true;

	/*auto loc = lLocations.begin();

	while(loc != lLocations.end())
	{
		if((loc->x == x)&&(loc->z == z)) break;
		++loc;
	}*/
	int index = 0;

 	DWORD dwWaitResult; 
 	dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);

	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		TileInLoc x, y, z;
		loc->GetTilePositionByPointer(loc->tTile + index, &x, &y, &z);
		TileInWorld xx, yy, zz;

		xx = x + LOCATION_SIZE_XZ*loc->x;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*loc->z;

		if(loc->tTile[index].cMaterial != MAT_NO)
		{
			Location *tempLoc;
			int tempIndex;
			if(!FindTile(xx, yy + 1, zz, &tempLoc, &tempIndex)) ShowTile(&*loc, index, TOP);
			else HideTile(&*tempLoc, tempIndex, DOWN);
			if(!FindTile(xx, yy - 1, zz, &tempLoc, &tempIndex)) ShowTile(&*loc, index, DOWN);
			else HideTile(&*tempLoc, tempIndex, TOP);
			if(!FindTile(xx + 1, yy, zz, &tempLoc, &tempIndex)) ShowTile(&*loc, index, RIGHT);
			else HideTile(&*tempLoc, tempIndex, LEFT);
			if(!FindTile(xx - 1, yy, zz, &tempLoc, &tempIndex)) ShowTile(&*loc, index, LEFT);
			else HideTile(&*tempLoc, tempIndex, RIGHT);
			if(!FindTile(xx, yy, zz + 1, &tempLoc, &tempIndex)) ShowTile(&*loc, index, BACK);
			else HideTile(&*tempLoc, tempIndex, FRONT);
			if(!FindTile(xx, yy, zz - 1, &tempLoc, &tempIndex)) ShowTile(&*loc, index, FRONT);
			else HideTile(&*tempLoc, tempIndex, BACK);
		}

		index++;
	}

	ReleaseMutex(loc->mutex);

	//skipbuild = false;
	//VisibleListAccessMutex.Release();

	//if(building) DrawLoadedTiles();
}

void World::DrawUnLoadedTiles(Location *loc)
{
	int index = 0;

 	DWORD dwWaitResult; 
 	dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);

	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		TileInLoc x, y, z;
		loc->GetTilePositionByPointer(loc->tTile + index, &x, &y, &z);
		TileInWorld xx, yy, zz;

		xx = x + LOCATION_SIZE_XZ*loc->x;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*loc->z;

		if(loc->tTile[index].cMaterial == MAT_NO)
		{
			Location *tempLoc;
			int tempIndex;
			if(FindTile(xx, yy + 1, zz, &tempLoc, &tempIndex)) //HideTile(&*loc, index, TOP);
			ShowTile(&*tempLoc, tempIndex, DOWN);
			if(FindTile(xx, yy - 1, zz, &tempLoc, &tempIndex)) //HideTile(&*loc, index, DOWN);
			ShowTile(&*tempLoc, tempIndex, TOP);
			if(FindTile(xx + 1, yy, zz, &tempLoc, &tempIndex)) //HideTile(&*loc, index, RIGHT);
			ShowTile(&*tempLoc, tempIndex, LEFT);
			if(FindTile(xx - 1, yy, zz, &tempLoc, &tempIndex)) //HideTile(&*loc, index, LEFT);
			ShowTile(&*tempLoc, tempIndex, RIGHT);
			if(FindTile(xx, yy, zz + 1, &tempLoc, &tempIndex)) //HideTile(&*loc, index, BACK);
			ShowTile(&*tempLoc, tempIndex, FRONT);
			if(FindTile(xx, yy, zz - 1, &tempLoc, &tempIndex)) //HideTile(&*loc, index, FRONT);
			ShowTile(&*tempLoc, tempIndex, BACK);
		}

		index++;
	}

	ReleaseMutex(loc->mutex);
}

int World::AddTile(TileInWorld x, TileInWorld y, TileInWorld z, char mat, bool show)
{
	if((y < 0)||(y >= LOCATION_SIZE_Y)) return 0;

	if(FindTile(x, y, z)) return 0;

	Location *loc = GetLocByTile(x, z);
	int index; 

	if(loc == NULL) return 0;

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	
	if(show)
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		index = loc->AddTile(locx, locy, locz, mat);
		ReleaseMutex(loc->mutex);
	
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
	else 
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		loc->AddTile(locx, locy, locz, mat);
		ReleaseMutex(loc->mutex);
	}

	return 1;
}

int World::RemoveTile(TileInWorld x, TileInWorld y, TileInWorld z, bool show)
{
	if((y < 0)||(y >= LOCATION_SIZE_Y)) return 0;

	if(!FindTile(x, y, z)) return 0;

	Location *loc = GetLocByTile(x, z);
	int index; 

	if(loc == NULL) return 0;

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	index = loc->GetIndexByPosition(locx, locy, locz);

	if(show)
	{
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

		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		loc->RemoveTile(locx, locy, locz);
		ReleaseMutex(loc->mutex);
	}
	else
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		loc->RemoveTile(locx, locy, locz);
		ReleaseMutex(loc->mutex);
	}

	return 1;
}

void World::ShowTile(Location *loc, int index, char N)
{
	if(!loc->tTile[index].bVisible[N])
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);

		loc->ShowTile(loc->tTile + index, N);
		ReleaseMutex(loc->mutex);
	}
}

void World::HideTile(Location *loc, int index, char N)
{
	if(loc->tTile[index].bVisible[N])
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);

		loc->HideTile(loc->tTile + index, N);
		ReleaseMutex(loc->mutex);
	}
}
 
int World::FindTile(TileInWorld x, TileInWorld y, TileInWorld z, Location **loc, int *index)
{
	if((y < 0)||(y >= LOCATION_SIZE_Y)) { loc = NULL; *index = 0; return 0;}

	(*loc) = GetLocByTile(x, z);

	if((*loc) == NULL) { *index = 0; return 0;}

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	*index = locx*LOCATION_SIZE_XZ + locz + locy*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ;

	if((*loc)->GetTileMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}

int World::FindTile(TileInWorld x, TileInWorld y, TileInWorld z)
{
	if(y < 0) return 0;

	Location *loc = GetLocByTile(x, z);

	if(loc == NULL) return 0;

	TileInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	if(loc->GetTileMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}
