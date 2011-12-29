#pragma once

#include <deque>
#include <list>
#include "Tile.h"

class Tiles : 
	public std::deque<Tile>
{
public:
	Tiles(void);
	~Tiles(void);
	
	int Number;
};

