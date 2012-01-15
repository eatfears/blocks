#include "Location.h"


Location::Location(LocInWorld x, LocInWorld z)
{
	Location::x = x; Location::z = z;

	tTile = new Tile[LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y];

	DisplayedTiles = new std::list<Tile *>[6];
	TexurePointerInVisible = new std::list<Tile*>::iterator *[6];
	for (int i = 0; i < 6; i++)
	{
		TexurePointerInVisible[i] = new std::list<Tile*>::iterator [100];//iNumberOfTextures];
	}

	for (int i = 0; i < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y; i++)
	{	tTile[i].cMaterial = MAT_NO;
		for (int N = 0; N < 6; N++)
			tTile[i].bVisible[N] = false;
	}
	bVisible = false;
}

Location::~Location(void)
{
// 	delete[] tTile;
// 
// 	for (int i = 0; i < 6; i++)
// 	{
// 		delete[] TexurePointerInVisible[i];
// 	}
// 	delete[] TexurePointerInVisible;
}

int Location::AddTile(TileInLoc x, TileInLoc y, TileInLoc z, char mat)
{
	if ((GetTileMaterial(x, y, z) != MAT_NO)||(GetTileMaterial(x, y, z) == -1)) return -1;

	TileInLoc index = SetTileMaterial(x, y, z, mat);
/*
	if (show)
	{
		if ((GetTileMaterial(x, y + 1, z) == MAT_NO)||(GetTileMaterial(x, y + 1, z) == -1)) ShowTile(tTile + index, TOP);
		else HideTile(x, y + 1, z, DOWN);
		if ((GetTileMaterial(x, y - 1, z) == MAT_NO)||(GetTileMaterial(x, y - 1, z) == -1)) ShowTile(tTile + index, DOWN);
		else HideTile(x, y - 1, z, TOP);
		if ((GetTileMaterial(x + 1, y, z) == MAT_NO)||(GetTileMaterial(x + 1, y, z) == -1)) ShowTile(tTile + index, RIGHT);
		else HideTile(x + 1, y, z, LEFT);
		if ((GetTileMaterial(x - 1, y, z) == MAT_NO)||(GetTileMaterial(x - 1, y, z) == -1)) ShowTile(tTile + index, LEFT);
		else HideTile(x - 1, y, z, RIGHT);
		if ((GetTileMaterial(x, y, z + 1) == MAT_NO)||(GetTileMaterial(x, y, z + 1) == -1)) ShowTile(tTile + index, BACK);
		else HideTile(x, y, z + 1, FRONT);
		if ((GetTileMaterial(x, y, z - 1) == MAT_NO)||(GetTileMaterial(x, y, z - 1) == -1)) ShowTile(tTile + index, FRONT);
		else HideTile(x, y, z - 1, BACK);

		//VisibleListAccessMutex.Acquire();
//  		tTiles[bin].push_front(t);
//  
//  		if(!FindTile(x, y + 1, z)) ShowTile(&(*tTiles[bin].begin()),TOP);
//  		else HideTile(x, y + 1, z, DOWN);
//  		if(!FindTile(x, y - 1, z)) ShowTile(&(*tTiles[bin].begin()),DOWN);
//  		else HideTile(x, y - 1, z, TOP);
//  		if(!FindTile(x + 1, y, z)) ShowTile(&(*tTiles[bin].begin()),RIGHT);
//  		else HideTile(x + 1, y, z, LEFT);
//  		if(!FindTile(x - 1, y, z)) ShowTile(&(*tTiles[bin].begin()),LEFT);
//  		else HideTile(x - 1, y, z, RIGHT);
//  		if(!FindTile(x, y, z + 1)) ShowTile(&(*tTiles[bin].begin()),BACK);
//  		else HideTile(x, y, z + 1, FRONT);
//  		if(!FindTile(x, y, z - 1)) ShowTile(&(*tTiles[bin].begin()),FRONT);
//  		else HideTile(x, y, z - 1, BACK);
 		//VisibleListAccessMutex.Release();
	}
//	else tTiles[bin].push_front(t);
*/
	return index;
}

int Location::RemoveTile(TileInLoc x, TileInLoc y, TileInLoc z)
{
	if (GetTileMaterial(x, y, z) == MAT_NO) return 0;

	SetTileMaterial(x, y, z, MAT_NO);

// 	if (show)
// 	{
// 		//VisibleListAccessMutex.Acquire();
//		Tile *temp;
//		temp = FindTile(x, y + 1, z); 
//		if(!temp) HideTile(x, y, z, 0);
//		else ShowTile(temp, 1);
//		temp = FindTile(x, y - 1, z); 
//		if(!temp) HideTile(x, y, z, 1);
//		else ShowTile(temp, 0);
//		temp = FindTile(x + 1, y, z); 
//		if(!temp) HideTile(x, y, z, 2);
//		else ShowTile(temp, 3);
//		temp = FindTile(x - 1, y, z); 
//		if(!temp) HideTile(x, y, z, 3);
//		else ShowTile(temp, 2);
//		temp = FindTile(x, y, z + 1); 
//		if(!temp) HideTile(x, y, z, 4);
//		else ShowTile(temp, 5);
//		temp = FindTile(x, y, z - 1); 
//		if(!temp) HideTile(x, y, z, 5);
//		else ShowTile(temp, 4);
// 		
//		tTiles[bin].erase(it);
//		//VisibleListAccessMutex.Release();
//	}
// 	else 
// 	{
//  		VisibleListAccessMutex.Acquire();
//  
//  		for(int N = 0; N < 6; N++)
//  			if(it->bVisible[N]) HideTile(x, y, z, N);
//  		tTiles[bin].erase(it);
//  
//  		VisibleListAccessMutex.Release();
// 	}
 
	return 1;
}

void Location::ShowTile(Tile *tTile, char N)
{
	if(!tTile->bVisible[N])
	{
		//int iTex = MaterialLib.mMaterial[tTile->cMaterial].iTexture[N];
		//auto it = MaterialLib.TexurePointerInVisible[N][iTex];

		//MaterialLib.TexurePointerInVisible[N][iTex] = DisplayedTiles[N].insert(it, tTile);
		
		tTile->bVisible[N] = true;
		DisplayedTiles[N].push_back(tTile);
	}
	//else for(;;);
}


void Location::HideTile(Tile *tTile, char N)
{

	//Tile *tTile = FindTile(x, y, z);
	
	if (!tTile) return;
	if (tTile->bVisible[N] == false) return;

	//int iTex = MaterialLib.mMaterial[tTile->cMaterial].iTexture[N];
	auto it = begin(DisplayedTiles[N]);
	//auto it = MaterialLib.TexurePointerInVisible[N][iTex];

	while(it != DisplayedTiles[N].end())
	{
		if(*it == tTile) break;
		//if (((*it)->sCoordZ == z)&&((*it)->sCoordX == x)&&((*it)->sCoordY == y)) break;
		it++;
	}
	if (it == DisplayedTiles[N].end()) return;

	(*it)->bVisible[N] = false;

	//int iTex = MaterialLib.mMaterial[(*it)->cMaterial].iTexture[N];

	//auto it2 = MaterialLib.TexurePointerInVisible[N][iTex];
	//Tile *t1 = (*it), *t2 = (*it2);
	
	//if(t1 == t2)
	//{
	//	++MaterialLib.TexurePointerInVisible[N][iTex];
		
	//	if(MaterialLib.TexurePointerInVisible[N][iTex] != DisplayedTiles[N].end())
	//	if((*MaterialLib.TexurePointerInVisible[N][iTex])->cMaterial != t1->cMaterial)
	//		MaterialLib.TexurePointerInVisible[N][iTex] = DisplayedTiles[N].end();
	//}

	DisplayedTiles[N].erase(it);
	return;
}

char Location::GetTileMaterial(TileInLoc x, TileInLoc y, TileInLoc z)
{
	if ((x >= LOCATION_SIZE_XZ)||(z >= LOCATION_SIZE_XZ)||(y >= LOCATION_SIZE_Y))
	{
		return -1;
	}
	return tTile[x*LOCATION_SIZE_XZ + z + y*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ].cMaterial;
}

TileInLoc Location::SetTileMaterial(TileInLoc x, TileInLoc y, TileInLoc z, char cMat)
{
	if ((x >= LOCATION_SIZE_XZ)||(z >= LOCATION_SIZE_XZ)||(y >= LOCATION_SIZE_Y))
		return -1;

	TileInLoc index = x*LOCATION_SIZE_XZ + z + y*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ;
	tTile[index].cMaterial = cMat;
	return index;
}

int Location::GetTilePositionByPointer(Tile *tCurrentTile, TileInLoc *x, TileInLoc *y, TileInLoc *z)
{
	int t = tCurrentTile - tTile;

	if ((t < 0)||(t >= LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y))
		return -1;

	*z  = t%LOCATION_SIZE_XZ;
	t/=LOCATION_SIZE_XZ;
	*x = t%LOCATION_SIZE_XZ;
	t/=LOCATION_SIZE_XZ;
	*y = t;

	return 0;
}
