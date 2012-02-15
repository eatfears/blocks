#pragma once

#include "Blocks_Definitions.h"
#include <GL/freeglut.h>

class Engine;

class Statistics
{
public:
	Statistics(Engine& eng);
	~Statistics(void);

	void ComputeFPS( double FrameInterval );
	void PrintStat(void);

private:
	Engine& engine;
	void RenderString(int x, int y, void *font, const char string[]);

	double TimeCount;
	int FrameCount;
};
