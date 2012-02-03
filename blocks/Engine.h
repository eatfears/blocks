#pragma once
#include "Blocks_Definitions.h"
#include <gl\glut.h>
#include <gl\glaux.h>
#include "gsl/gsl_randist.h"

#include "Character.h"
#include "Material.h"
#include "World.h"

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
	void Keyboard(unsigned char button, int x, int y, bool KeyDown);
	void MouseMotion(int x, int y);
	void MouseButton( int button, int state, int x, int y );
	

	int width, height;

	HGLRC  hRC;						// Постоянный контекст рендеринга
	HDC  hDC;						// Приватный контекст устройства GDI
	HWND  hWnd;						// Здесь будет хранится дескриптор окна
	HINSTANCE  hInstance;           // Здесь будет хранится дескриптор приложения

	bool	active;					// Флаг активности окна, установленный в true по умолчанию
	bool	fullscreen;
	bool	bMousing;
	gsl_rng *randNumGen;


	void InitGame();//GLWindow *glwWnd);

	void DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char N);
	void DrawInterface();
	void DrawSelectedItem();
	void Loop();

	//GLWindow *glwWnd;
	World wWorld;
	Character player;

	void GetFrameTime();
	double FrameInterval;
};
