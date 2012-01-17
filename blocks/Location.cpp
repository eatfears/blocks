#include "Location.h"

Location::Location(LocInWorld x, LocInWorld z, MaterialLibrary *MaterialLib)
{
	Location::x = x; Location::z = z;
	Location::MaterialLib = MaterialLib;

	tTile = new Tile[LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y];

	DisplayedTiles = new std::list<Tile *>[6];
	TexurePointerInVisible = new std::list<Tile*>::iterator *[6];
	
	for(int i = 0; i < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y; i++)
	{	tTile[i].cMaterial = MAT_NO;
		for(int N = 0; N < 6; N++)
			tTile[i].bVisible[N] = false;
	}
	bVisible = false;

	for(int i = 0; i < 6; i++)
	{
		TexurePointerInVisible[i] = new std::list<Tile*>::iterator [MaterialLib->iNumberOfTextures];
	}

	for(int i = 0; i < 6; i++)
	{
		for(int j = 1; j < MaterialLib->iNumberOfTextures; j++)
		{
			TexurePointerInVisible[i][j] = DisplayedTiles[i].end();
		}
	}
	mutex = CreateMutex(NULL, false, NULL);
}

Location::~Location(void)
{
// 	delete[] tTile;
// 
// 	for(int i = 0; i < 6; i++)
// 	{
// 		delete[] TexurePointerInVisible[i];
// 	}
// 	delete[] TexurePointerInVisible;
}

int Location::AddTile(TileInLoc x, TileInLoc y, TileInLoc z, char mat)
{
	if((GetTileMaterial(x, y, z) != MAT_NO)||(GetTileMaterial(x, y, z) == -1)) return -1;

	TileInLoc index = SetTileMaterial(x, y, z, mat);
	
	return index;
}

int Location::RemoveTile(TileInLoc x, TileInLoc y, TileInLoc z)
{
	if((GetTileMaterial(x, y, z) == MAT_NO)||(GetTileMaterial(x, y, z) == -1)) return -1;

	TileInLoc index = SetTileMaterial(x, y, z, MAT_NO);

	return index;
}

void Location::ShowTile(Tile *tTile, char N)
{
	if(!tTile) return;
	if(tTile->cMaterial == MAT_NO) return;
	if(tTile->bVisible[N]) return;
	
	int iTex = MaterialLib->mMaterial[tTile->cMaterial].iTexture[N];
	auto it = TexurePointerInVisible[N][iTex];

	TexurePointerInVisible[N][iTex] = DisplayedTiles[N].insert(it, tTile);
		
	tTile->bVisible[N] = true;
}


void Location::HideTile(Tile *tTile, char N)
{
	if(!tTile) return;
	if(tTile->cMaterial == MAT_NO) return;
	if(!tTile->bVisible[N]) return;

	int iTex = MaterialLib->mMaterial[tTile->cMaterial].iTexture[N];
	auto it = TexurePointerInVisible[N][iTex];
	auto it2 = it;

	while(it != DisplayedTiles[N].end())
	{
		if(*it == tTile) break;
		++it;
	}
	if(it == DisplayedTiles[N].end()) return;

	Tile *t1 = (*it), *t2 = (*it2);
	
	if(t1 == t2)
	{
		++TexurePointerInVisible[N][iTex];
		

		if(TexurePointerInVisible[N][iTex] != DisplayedTiles[N].end())
		{
			int iTex2 = MaterialLib->mMaterial[(*TexurePointerInVisible[N][iTex])->cMaterial].iTexture[N];
			if(iTex != iTex2)
				TexurePointerInVisible[N][iTex] = DisplayedTiles[N].end();
		}
	}

	(*it)->bVisible[N] = false;
	DisplayedTiles[N].erase(it);
	return;
}

char Location::GetTileMaterial(TileInLoc x, TileInLoc y, TileInLoc z)
{
	if((x >= LOCATION_SIZE_XZ)||(z >= LOCATION_SIZE_XZ)||(y >= LOCATION_SIZE_Y))
	{
		return -1;
	}
	return tTile[x*LOCATION_SIZE_XZ + z + y*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ].cMaterial;
}

int Location::SetTileMaterial(TileInLoc x, TileInLoc y, TileInLoc z, char cMat)
{
	if((x >= LOCATION_SIZE_XZ)||(z >= LOCATION_SIZE_XZ)||(y >= LOCATION_SIZE_Y))
		return -1;

	int index = GetIndexByPosition(x, y, z);
	tTile[index].cMaterial = cMat;
	return index;
}

int Location::GetTilePositionByPointer(Tile *tCurrentTile, TileInLoc *x, TileInLoc *y, TileInLoc *z)
{
	int t = tCurrentTile - tTile;

	if((t < 0)||(t >= LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y))
		return -1;

	*z  = t%LOCATION_SIZE_XZ;
	t/=LOCATION_SIZE_XZ;
	*x = t%LOCATION_SIZE_XZ;
	t/=LOCATION_SIZE_XZ;
	*y = t;

	return 0;
}

int Location::GetIndexByPosition( TileInLoc x, TileInLoc y, TileInLoc z )
{
	return x*LOCATION_SIZE_XZ + z + y*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ;
}
