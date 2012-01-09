#include "Material.h"
#include "resource.h"
#include "Blocks_Definitions.h"


Material::Material(void)
{
	mMater = new stMater[MAT_NUMBER];
}

Material::~Material(void)
{
	glDeleteTextures(TexturesNum,textures);
	free(textures);

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


GLvoid Material::LoadGLTextures()
{
	HBITMAP hBMP;                   // ��������� �� �����������
	BITMAP  BMP;                    // ��������� �����������

	// ��� ID ��� �����������, ������� �� ����� ��������� �� ����� ��������
	byte  Texture[]={0, IDB_DIRT, IDB_GRASS_TOP, IDB_GRASS_SIDE, IDB_STONE, IDB_SAND};

	TexturesNum = sizeof(Texture);
	textures = (GLuint *)calloc(TexturesNum, sizeof(GLuint));
	
	TexPtr = new std::list<Tile*>::iterator *[6];
	for (int i = 0; i < 6; i++)
	{
		TexPtr[i] = new std::list<Tile*>::iterator [TexturesNum];
	}

	glGenTextures(TexturesNum, textures); // ������� 3 �������� (sizeof(Texture)=3 ID's)
	for (int i = 0; i < TexturesNum; i++)	// ���� �� ���� ID (�����������)
	{
		// �������� ��������
		hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(Texture[i]), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

		if (hBMP)				// ���������� �� �����������?
		{						// ���� �� �

			GetObject(hBMP,sizeof(BMP), &BMP);

			// ����� ���������� �������� (�������� �� �������� ����� / 4 Bytes)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
			glBindTexture(GL_TEXTURE_2D, textures[i]);  // ����������� � ����� ���������

			// �������� ����������
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

			// �������� ���������� Mipmap GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

			// ��������� Mipmapped �������� (3 �����, ������, ������ � ������ �� BMP)
			//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BMP.bmWidth, BMP.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

			DeleteObject(hBMP);              // �������� ������� �����������
		}
		else textures[i] = NULL;
	}
}
