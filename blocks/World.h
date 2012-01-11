#pragma once
#include <list>

#include "Tiles.h"
#include "Material.h"

class World
{
public:
	World();
	~World();

	MaterialLibrary MaterialLib;
	Tiles *tTiles;
	std::list<Tile *> *DisplayedTiles;

	void BuildWorld();
	Tile* FindTile(signed short x, signed short y, signed short z);
	int AddTile(signed short x, signed short y, signed short z, char mat, bool show);
	int RemoveTile(signed short x, signed short y, signed short z);

//private:
	void StartBuilding();
	void StopBuilding();
	void ShowTile(Tile *tTile, char N);
	void HideTile(signed short x, signed short y, signed short z, char N);
	unsigned long ComputeBin(signed short x, signed short y, signed short z);

	bool building;
};

