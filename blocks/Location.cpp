#include "Location.h"

#define LOCATION_SIZE_XZ	16
#define LOCATION_SIZE_Y		128

Location::Location(void)
{
	tTile.reserve(LOCATION_SIZE_XZ*LOCATION_SIZE_XZ*LOCATION_SIZE_Y);
}

Location::~Location(void)
{

}

char Location::GetTileMaterial(signed short x, signed short y, signed short z)
{
	if ((x >= LOCATION_SIZE_XZ)||(z >= LOCATION_SIZE_XZ)||(y >= LOCATION_SIZE_Y))
	{
		return 0;
	}
	return tTile[x*16+z].cMaterial;
}

void Location::SetTileMaterial(signed short x, signed short y, signed short z, char cMat)
{

}

void Location::GetTilePositionByPointer(Tile *tCurrentTile, signed short *x, signed short *y, signed short *z)
{
	
}
