#pragma once

#include <list>

typedef struct tTile
{
	signed short x;
	signed short y;
	signed short z;

	char mat;

} Tile;

class Tiles : 
	public std::list<Tile>
{
public:
	Tiles(void);
	~Tiles(void);
	
	//int Number;
};

