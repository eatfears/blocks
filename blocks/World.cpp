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
	/*
	AddTile(1,-1,0,MAT_DIRT);
	AddTile(2,-1,0,MAT_STONE);
	AddTile(3,-1,0,MAT_STONE);
	AddTile(4,-1,0,MAT_DIRT);
	/**/
}

void World::GetLocByBlock( BlockInWorld x, BlockInWorld z, LocInWorld *locx, LocInWorld *locz )
{
	*locx = (LocInWorld) floor((double)x/LOCATION_SIZE_XZ);
	*locz = (LocInWorld) floor((double)z/LOCATION_SIZE_XZ);
}

void World::GetPosInLocByWorld( BlockInWorld x, BlockInWorld y, BlockInWorld z, BlockInLoc *locx, BlockInLoc *locy, BlockInLoc *locz )
{
	while(x < 0) x += LOCATION_SIZE_XZ;
	while(z < 0) z += LOCATION_SIZE_XZ;
	*locx = x%LOCATION_SIZE_XZ;
	*locy = y;
	*locz = z%LOCATION_SIZE_XZ;
}

Location* World::GetLocByBlock( BlockInWorld x, BlockInWorld z )
{
	LocInWorld locx, locz;
	GetLocByBlock(x, z, &locx, &locz );

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

void World::DrawLoadedBlocks(Location *loc)
{
	int index = 0;

 	DWORD dwWaitResult; 
 	dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
	BlockInLoc x, y, z;
	BlockInWorld xx, yy, zz;

	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		loc->GetBlockPositionByPointer(loc->bBlocks + index, &x, &y, &z);

		xx = x + LOCATION_SIZE_XZ*loc->x;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*loc->z;

		if(loc->bBlocks[index].cMaterial != MAT_NO)
		{
			Location *tempLoc;
			int tempIndex;

			if(loc->bBlocks[index].cMaterial == MAT_WATER)
			{
				if(!FindBlock(xx, yy + 1, zz, &tempLoc, &tempIndex)) ShowTile(loc, index, TOP);
				//else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, DOWN);
				if(!FindBlock(xx, yy - 1, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, BOTTOM);}
				//else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, TOP);
				if(!FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, RIGHT);}
				//else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, LEFT);
				if(!FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, LEFT);}
				//else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, RIGHT);
				if(!FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, BACK);}
				//else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, FRONT);
				if(!FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)) {if(tempLoc) ShowTile(loc, index, FRONT);}
				//else if(tempLoc->tTile[tempIndex].cMaterial == MAT_WATER) HideTile(tempLoc, tempIndex, BACK);
			}
			else
			{
				if(!FindBlock(xx, yy + 1, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) 
					ShowTile(&*loc, index, TOP);
				//else HideTile(&*tempLoc, tempIndex, DOWN);
				if(!FindBlock(xx, yy - 1, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, BOTTOM);}
				//else HideTile(&*tempLoc, tempIndex, TOP);
				if(!FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, RIGHT);}
				//else HideTile(&*tempLoc, tempIndex, LEFT);
				if(!FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, LEFT);}
				//else HideTile(&*tempLoc, tempIndex, RIGHT);
				if(!FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, BACK);}
				//else HideTile(&*tempLoc, tempIndex, FRONT);
				if(!FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)||(tempLoc->bBlocks[tempIndex].cMaterial == MAT_WATER)) {
					if(tempLoc) ShowTile(&*loc, index, FRONT);}
				//else HideTile(&*tempLoc, tempIndex, BACK);
			}
		}
		index++;
	}

	ReleaseMutex(loc->mutex);
	
	index = 0;
	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		loc->GetBlockPositionByPointer(loc->bBlocks + index, &x, &y, &z);
		xx = x + LOCATION_SIZE_XZ*loc->x;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*loc->z;

		if((loc->bBlocks[index].cMaterial == MAT_NO)||(loc->bBlocks[index].cMaterial == MAT_WATER))
		{
			Location *tempLoc;
			int tempIndex;
			if(FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, LEFT);
			if(FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, RIGHT);
			if(FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, FRONT);
			if(FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)&&(tempLoc->bBlocks[tempIndex].cMaterial != MAT_WATER)) if(tempLoc != loc)
				ShowTile(&*tempLoc, tempIndex, BACK);
		}
		index++;
	}
}

void World::DrawUnLoadedBlocks(LocInWorld Lx, LocInWorld Lz)
{
	int index = 0;

	Location *temp = 0;

	BlockInLoc x, y, z;
	BlockInWorld xx, yy, zz;
	
	while(index < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y)
	{
		temp->GetBlockPositionByIndex(index, &x, &y, &z);

		xx = x + LOCATION_SIZE_XZ*Lx;
		yy = y;
		zz = z + LOCATION_SIZE_XZ*Lz;
		
		Location *tempLoc;
		int tempIndex;
		if(FindBlock(xx + 1, yy, zz, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, LEFT);
		if(FindBlock(xx - 1, yy, zz, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, RIGHT);
		if(FindBlock(xx, yy, zz + 1, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, FRONT);
		if(FindBlock(xx, yy, zz - 1, &tempLoc, &tempIndex)) HideTile(&*tempLoc, tempIndex, BACK);
		index++;
	}
}

int World::AddBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, char mat, bool show)
{
	if((y < 0)||(y >= LOCATION_SIZE_Y)) return 0;

	if(FindBlock(x, y, z)) return 0;

	Location *loc = GetLocByBlock(x, z);
	int index; 

	if(loc == NULL) return 0;

	BlockInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	
	if(show)
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		index = loc->AddBlock(locx, locy, locz, mat);
		ReleaseMutex(loc->mutex);

		Location *lTempLoc = 0;
		int iTempIndex;

		if(loc->bBlocks[index].cMaterial == MAT_WATER)
		{
			if(!FindBlock(x, y + 1, z, &lTempLoc, &iTempIndex)) ShowTile(loc, index, TOP);
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, BOTTOM);
			if(!FindBlock(x, y - 1, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, BOTTOM);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, TOP);
			if(!FindBlock(x + 1, y, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, RIGHT);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, LEFT);
			if(!FindBlock(x - 1, y, z, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, LEFT);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, RIGHT);
			if(!FindBlock(x, y, z + 1, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, BACK);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, FRONT);
			if(!FindBlock(x, y, z - 1, &lTempLoc, &iTempIndex)) {if(lTempLoc) ShowTile(loc, index, FRONT);}
			else if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(lTempLoc, iTempIndex, BACK);
		}
		else
		{
			if(!FindBlock(x, y + 1, z, &lTempLoc, &iTempIndex)) 
				ShowTile(loc, index, TOP);
			else {HideTile(lTempLoc, iTempIndex, BOTTOM); if (lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) ShowTile(loc, index, TOP);}
			if(!FindBlock(x, y - 1, z, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, BOTTOM);}
			else {HideTile(lTempLoc, iTempIndex, TOP); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, BOTTOM);}
			if(!FindBlock(x + 1, y, z, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, RIGHT);}
			else {HideTile(lTempLoc, iTempIndex, LEFT); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, RIGHT);}
			if(!FindBlock(x - 1, y, z, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, LEFT);}
			else {HideTile(lTempLoc, iTempIndex, RIGHT); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, LEFT);}
			if(!FindBlock(x, y, z + 1, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, BACK);}
			else {HideTile(lTempLoc, iTempIndex, FRONT); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, BACK);}
			if(!FindBlock(x, y, z - 1, &lTempLoc, &iTempIndex)) {
				if(lTempLoc) ShowTile(loc, index, FRONT);}
			else {HideTile(lTempLoc, iTempIndex, BACK); if ((lTempLoc)&&(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER)) ShowTile(loc, index, FRONT);}
		}
	}
	else 
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		loc->AddBlock(locx, locy, locz, mat);
		ReleaseMutex(loc->mutex);
	}

	return 1;
}

int World::RemoveBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, bool show)
{
	if((y < 0)||(y >= LOCATION_SIZE_Y)) return 0;

	if(!FindBlock(x, y, z)) return 0;

	Location *loc = GetLocByBlock(x, z);
	int index; 

	if(loc == NULL) return 0;

	BlockInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	index = loc->GetIndexByPosition(locx, locy, locz);

	if(show)
	{
		Location *lTempLoc = 0;
		int iTempIndex;

		if(!FindBlock(x, y + 1, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, TOP);
		else {ShowTile(lTempLoc, iTempIndex, BOTTOM); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, TOP);}
		if(!FindBlock(x, y - 1, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, BOTTOM);
		else {ShowTile(lTempLoc, iTempIndex, TOP); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, BOTTOM);}
		if(!FindBlock(x + 1, y, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, RIGHT);
		else {ShowTile(lTempLoc, iTempIndex, LEFT); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, RIGHT);}
		if(!FindBlock(x - 1, y, z, &lTempLoc, &iTempIndex)) HideTile(loc, index, LEFT);
		else {ShowTile(lTempLoc, iTempIndex, RIGHT); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, LEFT);}
		if(!FindBlock(x, y, z + 1, &lTempLoc, &iTempIndex)) HideTile(loc, index, BACK);
		else {ShowTile(lTempLoc, iTempIndex, FRONT); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, BACK);}
		if(!FindBlock(x, y, z - 1, &lTempLoc, &iTempIndex)) HideTile(loc, index, FRONT);
		else {ShowTile(lTempLoc, iTempIndex, BACK); if(lTempLoc->bBlocks[iTempIndex].cMaterial == MAT_WATER) HideTile(loc, index, FRONT);}

		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		loc->RemoveBlock(locx, locy, locz);
		ReleaseMutex(loc->mutex);
	}
	else
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);
		loc->RemoveBlock(locx, locy, locz);
		ReleaseMutex(loc->mutex);
	}

	return 1;
}

void World::ShowTile(Location *loc, int index, char N)
{
	if(!loc->bBlocks[index].bVisible[N])
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);

		loc->ShowTile(loc->bBlocks + index, N);
		ReleaseMutex(loc->mutex);
	}
}

void World::HideTile(Location *loc, int index, char N)
{
	if(loc->bBlocks[index].bVisible[N])
	{
		DWORD dwWaitResult; 
		dwWaitResult = WaitForSingleObject( loc->mutex, INFINITE);

		loc->HideTile(loc->bBlocks + index, N);
		ReleaseMutex(loc->mutex);
	}
}
 
int World::FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z, Location **loc, int *index)
{
	if((y < 0)||(y >= LOCATION_SIZE_Y)) { *loc = NULL; *index = 0; return 0;}

	(*loc) = GetLocByBlock(x, z);

	if((*loc) == NULL) { *index = 0; return 0;}

	BlockInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	*index = locx*LOCATION_SIZE_XZ + locz + locy*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ;

	if((*loc)->GetBlockMaterial(locx, locy, locz) == MAT_NO)
		return 0;

	return 1;
}

int World::FindBlock(BlockInWorld x, BlockInWorld y, BlockInWorld z)
{
	if(y < 0) return 0;

	Location *loc = GetLocByBlock(x, z);

	if(loc == NULL) return 0;

	BlockInLoc locx, locy, locz;

	GetPosInLocByWorld(x, y, z, &locx, &locy, &locz);

	if(loc->GetBlockMaterial(locx, locy, locz) == MAT_NO)
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

	
	wWorld.DrawLoadedBlocks(&*loc);




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

	wWorld.DrawUnLoadedBlocks(x, z);
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
