#pragma once

#include "GLWindow.h"
#include "Character.h"
#include "Material.h"
#include "World.h"

class Game
{
public:
	Game();
	~Game();
	
	void InitGame(GLWindow *glwWnd);

	void DrawTile(signed short sXcoord, signed short sYcoord, signed short sZcoord, int material, char N);
	int DrawGLScene();
	void DrawInterface();
	void DrawSelectedItem();
	bool Loop();

	GLWindow *glwWnd;
	World wWorld;
	Character player;

	void GetFrameTime();
	double FrameInterval;
};

