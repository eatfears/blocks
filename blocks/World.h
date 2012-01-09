#pragma once
#include <deque>

#include "Tiles.h"
#include "Material.h"

class World
{
public:
	World();
	~World();

	Tiles *tTiles;
	std::deque<Tile *> *visible;

	void Build();
	Tile* FindTile(signed short x, signed short y, signed short z);
	int AddTile(signed short x, signed short y, signed short z, char mat);
	int RmTile(signed short x, signed short y, signed short z);

private:
	void StartBuilding();
	void StopBuilding();
	void ShowTile(Tile *tTile, char N);
	void HideTile(signed short x, signed short y, signed short z, char N);
	unsigned long Hash(signed short x, signed short y, signed short z);

	bool building;
};

