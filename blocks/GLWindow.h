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

	void AddTile(unsigned long x, unsigned long y, unsigned long z, char mat);
	int RmTile(unsigned long x, unsigned long y, unsigned long z);
	int Hash(unsigned long x, unsigned long y, unsigned long z);
	Tiles *tTiles;

	Character player;
	bool bMousing;

};
