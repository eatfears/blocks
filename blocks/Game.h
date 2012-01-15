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

	void DrawTileSide(signed short sXcoord, signed short sYcoord, signed short sZcoord, char N);
	//void DrawVisibleTileSide(Tile *tTile, char N);
	int DrawGLScene();
	void DrawInterface();
	bool Loop();

	GLWindow *glwWnd;
	World wWorld;
	Character player;

	void GetFrameTime();
	double FrameInterval;
};

