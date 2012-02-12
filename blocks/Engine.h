#pragma once
#include "Blocks_Definitions.h"
#include <GLblocks\blocksglut.h>

#include "World.h"
#include "Character.h"

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

	int width, height;

	bool	fullscreen;
	bool	bMousing;

	void InitGame();
	void DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char N);
	void DrawInterface();
	void DrawSelectedItem();
	void Loop();

	void DrawSunMoon();
	void DrawClouds();

	void OpenGL2d();
	void OpenGL3d();

	World wWorld;
	Character player;

	void GetFrameTime();
	double FrameInterval;
	double TimeOfDay;
};
