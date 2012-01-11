#pragma once

#include <list>

typedef struct tTile
{
	signed short sCoordX;
	signed short sCoordY;
	signed short sCoordZ;
	char cMaterial;
	bool bVisible[6];
} Tile;

class Tiles : 
	public std::list<Tile>
{
public:
	Tiles();
	~Tiles();
	
	//int Number;
};
