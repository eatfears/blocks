#include "Location.h"

Location::Location(LocInWorld x, LocInWorld z, MaterialLibrary& MLib, Landscape& lLand)
	: MaterialLib(MLib), lLandscape(lLand)
{
	Location::x = x; Location::z = z;

	tTile = new Tile[LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y];

	DisplayedTiles = new std::list<Tile *>[6];
	
	for(int i = 0; i < LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y; i++)
	{	tTile[i].cMaterial = MAT_NO;
		for(int N = 0; N < 6; N++)
			tTile[i].bVisible[N] = false;
	}
	bVisible = false;

	mutex = CreateMutex(NULL, false, NULL);
}

Location::~Location(void)
{
	delete[] tTile;

	delete[] DisplayedTiles;
}

int Location::AddTile(TileInLoc x, TileInLoc y, TileInLoc z, char mat)
{
	x = x%LOCATION_SIZE_XZ;
	y = y%LOCATION_SIZE_Y;
	z = z%LOCATION_SIZE_XZ;
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
	
	DisplayedTiles[N].push_back(tTile);
		
	tTile->bVisible[N] = true;
}

void Location::HideTile(Tile *tTile, char N)
{
	if(!tTile) return;
	if(tTile->cMaterial == MAT_NO) return;
	if(!tTile->bVisible[N]) return;

	auto it = DisplayedTiles[N].begin();

	while(it != DisplayedTiles[N].end())
	{
		if(*it == tTile) break;
		++it;
	}
	if(it == DisplayedTiles[N].end()) return;

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

	if(GetTilePositionByIndex(t, x, y, z) == -1) 
		return -1;

	return 0;
}

int Location::GetTilePositionByIndex(int index, TileInLoc *x, TileInLoc *y, TileInLoc *z)
{
	if((index < 0)||(index >= LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y))
		return -1;

	*z  = index%LOCATION_SIZE_XZ;
	index/=LOCATION_SIZE_XZ;
	*x = index%LOCATION_SIZE_XZ;
	index/=LOCATION_SIZE_XZ;
	*y = index;

	return 0;
}

int Location::GetIndexByPosition( TileInLoc x, TileInLoc y, TileInLoc z )
{
	return x*LOCATION_SIZE_XZ + z + y*LOCATION_SIZE_XZ*LOCATION_SIZE_XZ;
}

void Location::Generate()
{
	lLandscape.Generate(*this);
}
