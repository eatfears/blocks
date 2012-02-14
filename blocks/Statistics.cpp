#include "Statistics.h"
#include <stdio.h>
#include "Engine.h"


Statistics::Statistics(Engine& eng)
	: engine(eng)
{
	TimeCount = 0;
	FrameCount = 0;
}

Statistics::~Statistics(void)
{
}

void Statistics::ComputeFPS( double FrameInterval )
{
	FrameCount++;
	TimeCount += FrameInterval;
}

void Statistics::PrintStat(void)
{
	static char cFPS[20] = "";

	if (TimeCount > 100)
	{
		sprintf_s(cFPS, "FPS = %0.1f\n", 1000.0*FrameCount/TimeCount);
		TimeCount = 0.0;
		FrameCount = 0;
	}

	RenderString(50, engine.height - 50, GLUT_BITMAP_HELVETICA_12, cFPS);
}

void Statistics::RenderString(int x, int y, void *font, const char string[])
{
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}