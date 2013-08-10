#pragma once

#include "Definitions.h"
#include <GL/freeglut.h>

class Engine;

class Statistics
{
public:
	Statistics(Engine& eng);
	~Statistics(void);

	void ComputeFPS( double FrameInterval );
	void PrintStat(void);

	int reRenderedChunks;

private:
	Engine& engine;
	void RenderString(int x, int y, void *font, const char string[]);

	double TimeCount;
	int FrameCount;
};
