#pragma once
#include "Definitions.h"
#include <GL/freeglut.h>

#include "World.h"
#include "Character.h"
#include "Statistics.h"

class Engine
{
public:
	Engine();
	~Engine();

	int InitGL();
	void Reshape(GLsizei width, GLsizei height);
	void Display();
	void Keyboard(unsigned char button, int x, int y, bool KeyDown);
	void MouseMotion(int x, int y);
	void MouseButton(int button, int state, int x, int y);
	void Special(int button, int x, int y, bool KeyDown);
	void Loop();

	void InitGame();

private:
	int width, height;

	bool	fullscreen;
	bool	bMousing;

	void DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char side);
	void DrawInterface();
	void DrawSelectedItem();

	void DrawBottomBorder();
	void DrawSunMoon();
	void DrawClouds();

	void OpenGL2d();
	void OpenGL3d();

	World wWorld;

	double TimeOfDay; 
	double TimeOfWinal;
	void GetFogColor();
	GLfloat FogColor[4];

	void GetFrameTime();
	double FrameInterval;
	friend class Statistics;
	Statistics stat;
};
