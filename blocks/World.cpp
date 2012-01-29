#include <process.h>
#include <fstream>

#include "World.h"
#include "Blocks_Definitions.h"
#include "Landscape.h"


World::World()
{
	skipbuild = false;

	parget = CreateEvent(NULL, false, false, NULL);
	parget2 = CreateEvent(NULL, false, false, NULL);
	mutex = CreateMutex(NULL, false, NULL);
	semaphore = CreateSemaphore(NULL, 15, 15, NULL);

//	loading_mutex = CreateMutex(NULL, false, NULL);
}

World::~World()
{
	lLocations.clear();
}

void LoadNGenerate (void*);
void World::BuildWorld()
{
	
	Param par = {0, 0, this};

	_beginthread(LoadNGenerate, 0, &par);

	WaitForSingleObject(parget2, INFINITE);
	ResetEvent(parget2);
	/**/
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
	int index = 0;

 	DWORD dwWaitResult; 
 	dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
	TileInLoc x, y, z;
	TileInWorld xx, yy, zz;

	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		loc->GetTilePositionByPointer(loc->tTile + index, &x, &y, &z);

		xx = x + LOCATION_SIZE_XZ*loc->x;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*loc->z;

		if(loc->tTile[index].cMaterial != MAT_NO)
		{
			Location *tempLoc;
			int tempIndex;

			if(loc->tTile[index].cMaterial == MAT_WATER)
			{
				if(!FindTile(xx, yy + 1, zz, &tempLoc, &tempIndex)) ShowTile(loc, index, TOP);
				else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, DOWN);
				if(!FindTile(xx, yy - 1, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, DOWN);}
				else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, TOP);
				if(!FindTile(xx + 1, yy, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, RIGHT);}
				else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, LEFT);
				if(!FindTile(xx - 1, yy, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, LEFT);}
				else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, RIGHT);
				if(!FindTile(xx, yy, zz + 1, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, BACK);}
				else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, FRONT);
				if(!FindTile(xx, yy, zz - 1, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, FRONT);}
				else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, BACK);
			}
			else
			{
				if(!FindTile(xx, yy + 1, zz, &tempLoc, &tempIndex)||(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER)) 
					ShowTile(&*loc, index, TOP);
				else HideTile(&*tempLoc, tempIndex, DOWN);
				if(!FindTile(xx, yy - 1, zz, &tempLoc, &tempIndex)||(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, DOWN);}
				else HideTile(&*tempLoc, tempIndex, TOP);
				if(!FindTile(xx + 1, yy, zz, &tempLoc, &tempIndex)||(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, RIGHT);}
				else HideTile(&*tempLoc, tempIndex, LEFT);
				if(!FindTile(xx - 1, yy, zz, &tempLoc, &tempIndex)||(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, LEFT);}
				else HideTile(&*tempLoc, tempIndex, RIGHT);
				if(!FindTile(xx, yy, zz + 1, &tempLoc, &tempIndex)||(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, BACK);}
				else HideTile(&*tempLoc, tempIndex, FRONT);
				if(!FindTile(xx, yy, zz - 1, &tempLoc, &tempIndex)||(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, FRONT);}
				else HideTile(&*tempLoc, tempIndex, BACK);
			}
		}
		index++;
	}

	ReleaseMutex(loc->mutex);
	
	index = 0;
	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		loc->GetTilePositionByPointer(loc->tTile + index, &x, &y, &z);
		xx = x + LOCATION_SIZE_XZ*loc->x;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*loc->z;

		if((loc->tTile[index].cMaterial == MAT_NO)||(loc->tTile[index].cMaterial == MAT_WATER))
		{
			Location *tempLoc;
			int tempIndex;
			if(FindTile(xx + 1, yy, zz, &tempLoc, &tempIndex)&&(tempLoc->tTile[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, LEFT);
			if(FindTile(xx - 1, yy, zz, &tempLoc, &tempIndex)&&(tempLoc->tTile[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, RIGHT);
			if(FindTile(xx, yy, zz + 1, &tempLoc, &tempIndex)&&(tempLoc->tTile[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, FRONT);
			if(FindTile(xx, yy, zz - 1, &tempLoc, &tempIndex)&&(tempLoc->tTile[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, BACK);
		}
		index++;
	}
}

void World::DrawUnLoadedTiles(LocInWorld Lx, LocInWorld Lz)
{
	int index = 0;

 	DWORD dwWaitResult; 
	Location *temp = 0;

	TileInLoc x, y, z;
	TileInWorld xx, yy, zz;
	
	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		temp->GetTilePositionByIndex(index, &x, &y, &z);

		xx = x + LOCATION_SIZE_XZ*Lx;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*Lz;
		
		Location *tempLoc;
		int tempIndex;
		if(FindTile(xx + 1, yy, zz, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, LEFT);
		if(FindTile(xx - 1, yy, zz, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, RIGHT);
		if(FindTile(xx, yy, zz + 1, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, FRONT);
		if(FindTile(xx, yy, zz - 1, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, BACK);
		index++;
	}
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

		if(loc->tTile[index].cMaterial == MAT_WATER)
		{
			if(!FindTile(x, y + 1, z, &lTempLoc, &iTempIndex)) ShowTile(loc, index, TOP);
			else if(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, DOWN);
			if(!FindTile(x, y - 1, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, DOWN);}
			else if(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, TOP);
			if(!FindTile(x + 1, y, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, RIGHT);}
			else if(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, LEFT);
			if(!FindTile(x - 1, y, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, LEFT);}
			else if(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, RIGHT);
			if(!FindTile(x, y, z + 1, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, BACK);}
			else if(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, FRONT);
			if(!FindTile(x, y, z - 1, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, FRONT);}
			else if(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, BACK);
		}
		else
		{
			if(!FindTile(x, y + 1, z, &lTempLoc, &iTempIndex)||(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER)) 
				ShowTile(loc, index, TOP);
			else HideTile(lTempLoc, iTempIndex, DOWN);
			if(!FindTile(x, y - 1, z, &lTempLoc, &iTempIndex)||(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER)) {
				if(lTempLoc) ShowTile(loc, index, DOWN);}
			else HideTile(lTempLoc, iTempIndex, TOP);
			if(!FindTile(x + 1, y, z, &lTempLoc, &iTempIndex)||(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER)) {
				if(lTempLoc) ShowTile(loc, index, RIGHT);}
			else HideTile(lTempLoc, iTempIndex, LEFT);
			if(!FindTile(x - 1, y, z, &lTempLoc, &iTempIndex)||(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER)) {
				if(lTempLoc) ShowTile(loc, index, LEFT);}
			else HideTile(lTempLoc, iTempIndex, RIGHT);
			if(!FindTile(x, y, z + 1, &lTempLoc, &iTempIndex)||(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER)) {
				if(lTempLoc) ShowTile(loc, index, BACK);}
			else HideTile(lTempLoc, iTempIndex, FRONT);
			if(!FindTile(x, y, z - 1, &lTempLoc, &iTempIndex)||(lTempLoc->tTile[iTempIndex].cMaterial == MAT_WATER)) {
				if(lTempLoc) ShowTile(loc, index, FRONT);}
			else HideTile(lTempLoc, iTempIndex, BACK);
		}
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
	if((y < 0)||(y >= LOCATION_SIZE_Y)) { *loc = NULL; *index = 0; return 0;}

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

void LoadLocationThread(void* pParams)
{

	Param pParameters = *(Param*)pParams;
	LocInWorld x = pParameters.x;
	LocInWorld z = pParameters.z;
	World &wWorld = *pParameters.wWorld;

	SetEvent(wWorld.parget);
	WaitForSingleObject(wWorld.semaphore, INFINITE);


	HANDLE threadHandle = GetCurrentThread();
	SetThreadPriority(threadHandle, THREAD_PRIORITY_BELOW_NORMAL);

	DWORD dwWaitResult; 
	//auto loc = wWorld.AddLocation(x,z);

	Location *loc = new Location(x, z, wWorld.MaterialLib, wWorld.lLandscape);

	loc->Generate();
	//loc->lLandscape.Fill(*loc, 0, 0.999, 64);
	//loc->lLandscape.Load(loc->x, loc->z);


	dwWaitResult = WaitForSingleObject(wWorld.mutex, INFINITE);
	wWorld.lLocations.push_front(*loc);
	ReleaseMutex(wWorld.mutex);

	
	wWorld.DrawLoadedTiles(&*loc);




// 	dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
// 	auto locc = wWorld.LoadedLocations.begin();
// 	while (locc != wWorld.LoadedLocations.end())
// 	{
// 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
// 		++loc;
// 	}
// 	if (locc == wWorld.LoadedLocations.end()) 
// 	{
// 		ReleaseMutex(wWorld.loading_mutex);
// 		_endthread();
// 		return;
// 	}
// 	wWorld.LoadedLocations.erase(locc);
// 	ReleaseMutex(wWorld.loading_mutex);

	ReleaseSemaphore(wWorld.semaphore, 1, NULL);

	_endthread();
}

void UnLoadLocationThread(void* pParams)
{
	Param pParameters = *(Param*)pParams;
	LocInWorld x = pParameters.x;
	LocInWorld z = pParameters.z;
	World &wWorld = *pParameters.wWorld;

	SetEvent(wWorld.parget);
	WaitForSingleObject(wWorld.semaphore, INFINITE);

	HANDLE threadHandle = GetCurrentThread();
	SetThreadPriority(threadHandle, THREAD_PRIORITY_BELOW_NORMAL);

	DWORD dwWaitResult; 

	dwWaitResult = WaitForSingleObject(wWorld.mutex, INFINITE);

	auto loc= wWorld.lLocations.begin();

	while(loc != wWorld.lLocations.end())
	{
		if((loc->x == x)&&(loc->z == z)) break;
		++loc;
	}
	if(loc == wWorld.lLocations.end()) 
	{	
		ReleaseMutex(wWorld.mutex);
		ReleaseSemaphore(wWorld.semaphore, 1, NULL);

// 		dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
// 		LocationPosiion lp = {x, z};
// 		auto locc = wWorld.LoadedLocations.begin();
// 		while (locc != wWorld.LoadedLocations.end())
// 		{
// 			if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
// 			++loc;
// 		}
// 		if (locc == wWorld.LoadedLocations.end())
// 		{
// 			ReleaseMutex(wWorld.loading_mutex);
// 			_endthread(); 
// 			return;
// 		}
// 		wWorld.LoadedLocations.erase(locc);
// 		ReleaseMutex(wWorld.loading_mutex);
		_endthread(); 
		return;
	}

	dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);
	wWorld.lLocations.erase(loc);

	wWorld.DrawUnLoadedTiles(x, z);
	ReleaseMutex(wWorld.mutex);

// 
// 	dwWaitResult = WaitForSingleObject(wWorld.loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
// 	auto locc = wWorld.LoadedLocations.begin();
// 	while (locc != wWorld.LoadedLocations.end())
// 	{
// 		if (((*locc).x == lp.x)&&((*locc).z == lp.z)) break;
// 		++loc;
// 	}
// 	if (locc == wWorld.LoadedLocations.end())
// 	{
// 		ReleaseMutex(wWorld.loading_mutex);
// 		_endthread(); 
// 		return;
// 	}
// 	wWorld.LoadedLocations.erase(locc);
// 	ReleaseMutex(wWorld.loading_mutex);
	ReleaseSemaphore(wWorld.semaphore, 1, NULL);
	_endthread();
}

void World::LoadLocation(LocInWorld x, LocInWorld z)
{
// 	WaitForSingleObject(loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
// 
// 	auto loc = LoadedLocations.begin();
// 
// 	while (loc != LoadedLocations.end())
// 	{
// 		if (((*loc).x == lp.x)&&((*loc).z == lp.z)) 
// 		{
// 			ReleaseMutex(loading_mutex);
// 			return;
// 		}		
// 		++loc;
// 	}
// 	LoadedLocations.push_back(lp);
// 	ReleaseMutex(loading_mutex);


	Param par = {x, z, this};

	HANDLE hThread;
	hThread = (HANDLE) _beginthread( LoadLocationThread, 0, &par);//&Param(par) );

	//WaitForSingleObject(hThread, 30);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
	/*
	par.x++;
	if(par.x == 10)
	{
		par.x = 0;
		par.z++;
	}
	/**/
}

void World::UnLoadLocation(LocInWorld x, LocInWorld z)
{
// 	WaitForSingleObject(loading_mutex, INFINITE);
// 	LocationPosiion lp = {x, z};
// 
// 	auto loc = LoadedLocations.begin();
// 
// 	while (loc != LoadedLocations.end())
// 	{
// 		if (((*loc).x == lp.x)&&((*loc).z == lp.z))
// 		{
// 			ReleaseMutex(loading_mutex);
// 			return;
// 		}
// 		++loc;
// 	}
// 	LoadedLocations.push_back(lp);
// 	ReleaseMutex(loading_mutex);

	Param par = {x, z, this};

	HANDLE hThread;

	hThread = (HANDLE) _beginthread( UnLoadLocationThread, 0, &par);//&Param(par) );

	//WaitForSingleObject(hThread, 30);
	WaitForSingleObject(parget, INFINITE);
	ResetEvent(parget);
	/*
	par.x++;
	if(par.x == 10)
	{
		par.x = 0;
		par.z++;
	}
	/**/
}
