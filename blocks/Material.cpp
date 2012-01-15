#include "Material.h"
#include "resource.h"
#include "Blocks_Definitions.h"


MaterialLibrary::MaterialLibrary()
{
	mMaterial = new stMater[MAT_NUMBER];
}

MaterialLibrary::~MaterialLibrary()
{
	glDeleteTextures(iNumberOfTextures,texture);
	free(texture);

	delete[] mMaterial;
}

void MaterialLibrary::InitMaterials()
{
	for (int i = 0; i < MAT_NUMBER; i++)
	{
		switch(i)
		{
		case MAT_NO: 
			{
				for (int j = 0; j < 6; j++)
					mMaterial[MAT_NO].iTexture[j] = TEX_NO; 
			}
			break;		
		case MAT_DIRT: 
			{
				mMaterial[MAT_DIRT].iTexture[TOP] = TEX_DIRT_TOP; 
				mMaterial[MAT_DIRT].iTexture[DOWN] = TEX_DIRT_DOWN; 
				for (int j = 2; j < 6; j++)
					mMaterial[MAT_DIRT].iTexture[j] = TEX_DIRT_SIDE; 
			}
			break;		
		case MAT_GRASS: 
			{
				mMaterial[MAT_GRASS].iTexture[TOP] = TEX_GRASS_TOP; 
				mMaterial[MAT_GRASS].iTexture[DOWN] = TEX_GRASS_DOWN; 
				for (int j = 2; j < 6; j++)
					mMaterial[MAT_GRASS].iTexture[j] = TEX_GRASS_SIDE; 
			}
			break;		
		case MAT_STONE: 
			{
				mMaterial[MAT_STONE].iTexture[TOP] = TEX_STONE_TOP; 
				mMaterial[MAT_STONE].iTexture[DOWN] = TEX_STONE_DOWN; 
				for (int j = 2; j < 6; j++)
					mMaterial[MAT_STONE].iTexture[j] = TEX_STONE_SIDE; 
			}
			break;		
		case MAT_SAND: 
			{
				mMaterial[MAT_SAND].iTexture[TOP] = TEX_SAND_TOP; 
				mMaterial[MAT_SAND].iTexture[DOWN] = TEX_SAND_DOWN; 
				for (int j = 2; j < 6; j++)
					mMaterial[MAT_SAND].iTexture[j] = TEX_SAND_SIDE; 
			}
			break;
		default:
			{
				for (int j = 0; j < 6; j++)
					mMaterial[i].iTexture[j] = TEX_NO; 
			}
			break;
		}
	}
}

void MaterialLibrary::AllocGLTextures()
{

	// Три ID для изображений, которые мы хотим загрузить из файла ресурсов
	byte  Texture[]={0, IDB_DIRT, IDB_GRASS_TOP, IDB_GRASS_SIDE, IDB_STONE, IDB_SAND};

	iNumberOfTextures = sizeof(Texture);
	texture = (GLuint *)calloc(iNumberOfTextures, sizeof(GLuint));
	
	glGenTextures(iNumberOfTextures, texture); // создаем 3 текстуры (sizeof(Texture)=3 ID's)

}

void MaterialLibrary::LoadGLTextures()
{
	HBITMAP hBMP;                   // Указатель на изображение
	BITMAP  BMP;                    // структура изображения
	byte  Texture[]={0, IDB_DIRT, IDB_GRASS_TOP, IDB_GRASS_SIDE, IDB_STONE, IDB_SAND};

	for (int i = 0; i < iNumberOfTextures; i++)	// цикл по всем ID (изображений)
	{
		// Создание текстуры
		hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(Texture[i]), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

		if (hBMP)				// существует ли изображение?
		{						// если да …

			GetObject(hBMP,sizeof(BMP), &BMP);

			// режим сохранения пикселей (равнение по двойному слову / 4 Bytes)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
			glBindTexture(GL_TEXTURE_2D, texture[i]);  // связываемся с нашей текстурой

			// линейная фильтрация
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

			// линейная фильтрация Mipmap GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

			// генерация Mipmapped текстуры (3 байта, ширина, высота и данные из BMP)
			//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BMP.bmWidth, BMP.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

			DeleteObject(hBMP);              // удаление объекта изображения
		}
		else texture[i] = NULL;
	}
}
