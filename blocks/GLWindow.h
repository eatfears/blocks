#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <vector>

#include "Tiles.h"
#include "Character.h"

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
	void GlTile(int X, int Y, int Z);

	GLsizei width, height;

	HGLRC  hRC;						// ���������� �������� ����������
	HDC  hDC;						// ��������� �������� ���������� GDI
	HWND  hWnd;						// ����� ����� �������� ���������� ����
	HINSTANCE  hInstance;           // ����� ����� �������� ���������� ����������

	bool  keys[256];				// ������, ������������ ��� �������� � �����������
	bool  active;					// ���� ���������� ����, ������������� � true �� ���������

	Tile* FindTile(signed long x, signed long y, signed long z);
	int AddTile(signed long x, signed long y, signed long z, char mat);
	int RmTile(signed long x, signed long y, signed long z);
	int Hash(signed long x, signed long y, signed long z);
	Tiles *tTiles;

	void GetCenterCoords(GLdouble *wx, GLdouble *wy, GLdouble *wz);
	void Control();
	void GetFrameTime();

	Character player;
	bool bMousing;
	GLdouble g_FrameInterval;
};
