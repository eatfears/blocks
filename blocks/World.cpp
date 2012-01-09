#include "World.h"
#include "Blocks_Definitions.h"

World::World()
{
	tTiles = new Tiles[0x100000];
	visible = new std::list<Tile *>[6];
}

World::~World()
{
}

void World::Build()
{
	MaterialLib.InitMaterials();

	for (int i = 0; i < 6; i++)
		for (int j = 0; j < MaterialLib.TexturesNum; j++)
		{
			MaterialLib.TexPtr[i][j] = visible[i].end();
		}

	StartBuilding();
	/*
	AddTile(0,-1,0,MATERIAL_YES);
	AddTile(3,-1,0,MATERIAL_YES);
	AddTile(2,-1,0,MATERIAL_YES);
	AddTile(1,-1,0,MATERIAL_YES);
	AddTile(0,-1,1,MATERIAL_YES);
	AddTile(0,-1,2,MATERIAL_YES);
	AddTile(0,-1,3,MATERIAL_YES);

	/*
	AddTile(10,0,0,MATERIAL_YES);
	AddTile(0,1,0,MATERIAL_YES);
	AddTile(0,-3,0,MATERIAL_YES);
	AddTile(1,0,0,MATERIAL_YES);
	AddTile(-1,0,0,MATERIAL_YES);
	/**/
	
	for (int i = 0 ; i< 40 ; i++)
	{
		for (int k = 0 ; k < 1 ; k++)
		{
			AddTile(rand()%100-50,k,rand()%100-50,MAT_STONE);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_SAND);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_DIRT);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_GRASS);
			AddTile(rand()%100-50,k,rand()%100-50,MAT_NO);
		}
	}


	//100 500 500 1,5 GB
	for (int j = 1; j <= 5; j++)
	{
		for (int i = 0; i < 40; i++)
		{
			for (int k = 0; k < 40; k++)
			{				
				//AddTile(i-20,-j,k-20,rand()%4+1);
				AddTile(i-20,-j,k-20,MAT_GRASS);
			}
		}
	}

	StopBuilding();
}

void World::StartBuilding()
{
	building = true;
}

void World::StopBuilding()
{
	building = false;

	for (int i = 0; i < 0x100000; i++)
	{
		Tiles::iterator it = tTiles[i].begin();

		while(it != tTiles[i].end())
		{
			if(!FindTile(it->x, it->y + 1, it->z)) ShowTile(&*it,TOP);
			if(!FindTile(it->x, it->y - 1, it->z)) ShowTile(&*it,DOWN);
			if(!FindTile(it->x + 1, it->y, it->z)) ShowTile(&*it,RIGHT);
			if(!FindTile(it->x - 1, it->y, it->z)) ShowTile(&*it,LEFT);
			if(!FindTile(it->x, it->y, it->z + 1)) ShowTile(&*it,BACK);
			if(!FindTile(it->x, it->y, it->z - 1)) ShowTile(&*it,FRONT);
			it++;
		}
	}
}

int World::AddTile(signed short x, signed short y, signed short z, char mat)
{
	unsigned long bin = Hash(x, y, z);
	Tiles::iterator it = tTiles[bin].begin();

	while(it != tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it != tTiles[bin].end()) return 0;

	Tile t = {x, y, z, mat};
	tTiles[bin].push_front(t);

	if (!building)
	{
		if(!FindTile(x, y + 1, z)) ShowTile(&(*tTiles[bin].begin()),TOP);
		else HideTile(x, y + 1, z, DOWN);
		if(!FindTile(x, y - 1, z)) ShowTile(&(*tTiles[bin].begin()),DOWN);
		else HideTile(x, y - 1, z, TOP);
		if(!FindTile(x + 1, y, z)) ShowTile(&(*tTiles[bin].begin()),RIGHT);
		else HideTile(x + 1, y, z, LEFT);
		if(!FindTile(x - 1, y, z)) ShowTile(&(*tTiles[bin].begin()),LEFT);
		else HideTile(x - 1, y, z, RIGHT);
		if(!FindTile(x, y, z + 1)) ShowTile(&(*tTiles[bin].begin()),BACK);
		else HideTile(x, y, z + 1, FRONT);
		if(!FindTile(x, y, z - 1)) ShowTile(&(*tTiles[bin].begin()),FRONT);
		else HideTile(x, y, z - 1, BACK);
	}

	return 1;
}

int World::RmTile(signed short x, signed short y, signed short z)
{
	unsigned long bin = Hash(x, y, z);
	Tiles::iterator it = tTiles[bin].begin();

	while(it != tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it == tTiles[bin].end()) return 0;

	Tile *temp;
	temp = FindTile(x, y + 1, z); 
	if(!temp) HideTile(x, y, z, 0);
	else visible[1].push_back(temp);

	temp = FindTile(x, y - 1, z); 
	if(!temp) HideTile(x, y, z, 1);
	else visible[0].push_back(temp);
	temp = FindTile(x + 1, y, z); 
	if(!temp) HideTile(x, y, z, 2);
	else visible[3].push_back(temp);
	temp = FindTile(x - 1, y, z); 
	if(!temp) HideTile(x, y, z, 3);
	else visible[2].push_back(temp);
	temp = FindTile(x, y, z + 1); 
	if(!temp) HideTile(x, y, z, 4);
	else visible[5].push_back(temp);
	temp = FindTile(x, y, z - 1); 
	if(!temp) HideTile(x, y, z, 5);
	else visible[4].push_back(temp);
	/**/

	tTiles[bin].erase(it);

	return 1;
}

void World::ShowTile(Tile *tTile, char N)
{
	int iTex = MaterialLib.mMater[tTile->mat].iTex[N];

	auto it = MaterialLib.TexPtr[N][iTex];
	if(it == visible[N].end())
	{
		MaterialLib.TexPtr[N][iTex] = visible[N].insert(it, tTile);
	}
	else visible[N].insert(it, tTile);
//	visible[N].push_back(tTile);
}

void World::HideTile(signed short x, signed short y, signed short z, char N)
{
	auto it = begin(visible[N]);

	while(it != visible[N].end())
	{
		if (((*it)->z == z)&&((*it)->x == x)&&((*it)->y == y)) break;
		it++;
	}
	if (it == visible[N].end()) return;

	visible[N].erase(it);
	return;
}

Tile* World::FindTile(signed short x, signed short y, signed short z)
{
	unsigned long bin = Hash(x, y, z);
	Tiles::iterator it = tTiles[bin].begin();

	while(it != tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it == tTiles[bin].end()) return NULL;

	return &(*it);
}

unsigned long World::Hash(signed short x, signed short y, signed short z)
{
	return (x & 0xff) + ((z & 0xff)<<8) + ((y & 0x0f)<<16);
}
