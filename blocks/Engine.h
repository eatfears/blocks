#pragma once
#include "Blocks_Definitions.h"
#include <gl\freeglut.h>
#include <gl\glaux.h>

#include "Character.h"
#include "Material.h"
#include "World.h"

class Engine
{
public:
	Engine();
	~Engine();

	int InitGL();
	//bool CreateGLWindow(LPCSTR title, GLsizei width, GLsizei height, int bits);
	//void KillGLWindow();
	void Reshape(GLsizei width, GLsizei height);
	void Display();
	void Keyboard(unsigned char button, int x, int y, bool KeyDown);
	void MouseMotion(int x, int y);
	void MouseButton(int button, int state, int x, int y);
	void Special(int button, int x, int y, bool KeyDown);
	

	int width, height;

	HGLRC  hRC;						// Постоянный контекст рендеринга
	HDC  hDC;						// Приватный контекст устройства GDI
	HWND  hWnd;						// Здесь будет хранится дескриптор окна
	HINSTANCE  hInstance;           // Здесь будет хранится дескриптор приложения

	bool	fullscreen;
	bool	bMousing;

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
