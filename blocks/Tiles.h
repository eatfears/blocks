#pragma once

#include <deque>
#include <vector>
#include <list>
#include "Tile.h"

class Tiles : 
	public std::list<Tile>
{
public:
	Tiles(void);
	~Tiles(void);
	
	//int Number;
};

