#include "Material.h"
#include "Blocks_Definitions.h"


Material::Material(void)
{
	mMater = new stMater[MAT_NUMBER];
}

Material::~Material(void)
{
	delete[] mMater;
}


void Material::InitMaterials()
{
	for (int i = 0; i < MAT_NUMBER; i++)
	{
		switch(i)
		{
		case MAT_NO: 
			{
				for (int j = 0; j < 6; j++)
					mMater[MAT_NO].iTex[j] = TEX_NO; 
			}
			break;		
		case MAT_DIRT: 
			{
				mMater[MAT_DIRT].iTex[TOP] = TEX_DIRT_TOP; 
				mMater[MAT_DIRT].iTex[DOWN] = TEX_DIRT_DOWN; 
				for (int j = 2; j < 6; j++)
					mMater[MAT_DIRT].iTex[j] = TEX_DIRT_SIDE; 
			}
			break;		
		case MAT_GRASS: 
			{
				mMater[MAT_GRASS].iTex[TOP] = TEX_GRASS_TOP; 
				mMater[MAT_GRASS].iTex[DOWN] = TEX_GRASS_DOWN; 
				for (int j = 2; j < 6; j++)
					mMater[MAT_GRASS].iTex[j] = TEX_GRASS_SIDE; 
			}
			break;		
		case MAT_STONE: 
			{
				mMater[MAT_STONE].iTex[TOP] = TEX_STONE_TOP; 
				mMater[MAT_STONE].iTex[DOWN] = TEX_STONE_DOWN; 
				for (int j = 2; j < 6; j++)
					mMater[MAT_STONE].iTex[j] = TEX_STONE_SIDE; 
			}
			break;		
		case MAT_SAND: 
			{
				mMater[MAT_SAND].iTex[TOP] = TEX_SAND_TOP; 
				mMater[MAT_SAND].iTex[DOWN] = TEX_SAND_DOWN; 
				for (int j = 2; j < 6; j++)
					mMater[MAT_SAND].iTex[j] = TEX_SAND_SIDE; 
			}
			break;
		default:
			{
				for (int j = 0; j < 6; j++)
					mMater[i].iTex[j] = TEX_NO; 
			}
			break;
		}
	}
}
