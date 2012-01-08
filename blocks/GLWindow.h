#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <vector>
#include <list>

#include "Tiles.h"
#include "Character.h"
#include "Material.h"

class GLWindow
{
public:
	GLWindow(void);
	~GLWindow(void);

	void KillGLWindow(void);
	int InitGL(void);
	void ReSizeGLScene( GLsizei width, GLsizei height );
	bool CreateGLWindow( LPCSTR title, GLsizei width, GLsizei height, int bits);
	int DrawGLScene();
	void GlTile(signed short X, signed short Y, signed short Z, char N);
	void DrawInterface();

	GLsizei width, height;

	HGLRC  hRC;						// Постоянный контекст рендеринга
	HDC  hDC;						// Приватный контекст устройства GDI
	HWND  hWnd;						// Здесь будет хранится дескриптор окна
	HINSTANCE  hInstance;           // Здесь будет хранится дескриптор приложения

	//bool  keys[256];				// Массив, используемый для операций с клавиатурой
	bool  active;					// Флаг активности окна, установленный в true по умолчанию

	Tile* FindTile(signed short x, signed short y, signed short z);
	void FindTileN(signed short x, signed short y, signed short z, char N);
	int AddTile(signed short x, signed short y, signed short z, char mat);
	int RmTile(signed short x, signed short y, signed short z);
	unsigned long Hash(signed short x, signed short y, signed short z);
	Tiles *tTiles;
	Material MaterialLib;
	std::deque<Tile *> *visible;

	void GetCenterCoords(GLdouble *wx, GLdouble *wy, GLdouble *wz);
	void Control();
	void GetFrameTime();

	Character player;
	bool bMousing;
	GLdouble g_FrameInterval;

	bool building;
};
