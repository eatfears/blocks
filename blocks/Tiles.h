#pragma once

#include <list>

typedef struct tTile
{
	signed short sCoordX;
	signed short sCoordY;
	signed short sCoordZ;
	char cMaterial;
} Tile;

class Tiles : 
	public std::list<Tile>
{
public:
	Tiles(void);
	~Tiles(void);
	
	//int Number;
};

