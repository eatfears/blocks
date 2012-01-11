#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

class GLWindow
{
public:
	GLWindow();
	~GLWindow();

	int InitGL();
	bool CreateGLWindow(LPCSTR title, GLsizei width, GLsizei height, int bits);
	void KillGLWindow();
	void ReSizeGLScene(GLsizei width, GLsizei height);

	GLsizei width, height;

	HGLRC  hRC;						// ���������� �������� ����������
	HDC  hDC;						// ��������� �������� ���������� GDI
	HWND  hWnd;						// ����� ����� �������� ���������� ����
	HINSTANCE  hInstance;           // ����� ����� �������� ���������� ����������

	bool	active;					// ���� ���������� ����, ������������� � true �� ���������
	bool	fullscreen;
	bool	bMousing;
};
