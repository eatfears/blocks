#pragma once

#include "Blocks_Definitions.h"
#include <GLblocks\blocksglut.h>

class Statistic
{
public:
	Statistic(void);
	~Statistic(void);

	void ComputeFPS( double FrameInterval );
	void PrintStat(int width, int height);

private:
	void RenderString(int x, int y, void *font, const char string[], GLfloat r, GLfloat g, GLfloat b, GLfloat a);

	double TimeCount;	
	int FrameCount;
};

