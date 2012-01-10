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

	HGLRC  hRC;						// Постоянный контекст рендеринга
	HDC  hDC;						// Приватный контекст устройства GDI
	HWND  hWnd;						// Здесь будет хранится дескриптор окна
	HINSTANCE  hInstance;           // Здесь будет хранится дескриптор приложения

	bool	active;					// Флаг активности окна, установленный в true по умолчанию
	bool	fullscreen;
	bool	bMousing;
};
