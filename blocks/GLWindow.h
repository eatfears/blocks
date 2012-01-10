#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "Tiles.h"
#include "Character.h"
#include "Material.h"
#include "World.h"

class GLWindow
{
public:
	GLWindow(void);
	~GLWindow(void);

	int InitGL(void);
	bool CreateGLWindow( LPCSTR title, GLsizei width, GLsizei height, int bits);
	void KillGLWindow(void);
	void ReSizeGLScene( GLsizei width, GLsizei height );
	int DrawGLScene();
	void DrawVisibleTileSide(Tile *tTile, char N);
	void DrawInterface();

	GLsizei width, height;

	HGLRC  hRC;						// ���������� �������� ����������
	HDC  hDC;						// ��������� �������� ���������� GDI
	HWND  hWnd;						// ����� ����� �������� ���������� ����
	HINSTANCE  hInstance;           // ����� ����� �������� ���������� ����������

	bool	active;					// ���� ���������� ����, ������������� � true �� ���������
	bool	fullscreen;

	World wWorld;

	bool Loop();
	void GetCenterCoords(GLdouble *wx, GLdouble *wy, GLdouble *wz);
	void GetFrameTime();

	Character player;
	bool bMousing;
	GLdouble FrameInterval;
};
