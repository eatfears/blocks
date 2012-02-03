#pragma once
#define FREEGLUT_STATIC
#include <gl\glut.h>
#include <gl\glaux.h>
#include "gsl/gsl_randist.h"


class Engine
{
public:
	Engine();
	~Engine();

	int Init();
	//bool CreateGLWindow(LPCSTR title, GLsizei width, GLsizei height, int bits);
	//void KillGLWindow();
	void Reshape(GLsizei width, GLsizei height);
	void Display();
	void Keyboard(unsigned char button, int x, int y, bool KeyUp);
	void MouseMotion(int x, int y);


	int width, height;

	HGLRC  hRC;						// ���������� �������� ����������
	HDC  hDC;						// ��������� �������� ���������� GDI
	HWND  hWnd;						// ����� ����� �������� ���������� ����
	HINSTANCE  hInstance;           // ����� ����� �������� ���������� ����������

	bool	active;					// ���� ���������� ����, ������������� � true �� ���������
	bool	fullscreen;
	bool	bMousing;
	gsl_rng *randNumGen;

	double dPositionX;
	double dPositionY;
	double dPositionZ;
	double dSpinX;
	double dSpinY;
};
