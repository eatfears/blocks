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
	static char pos[40] = "";
	void *font = GLUT_BITMAP_HELVETICA_12;

	if (TimeCount > 100)
	{
		sprintf_s(cFPS, "FPS: %0.1f\n", 1000.0*FrameCount/TimeCount);
		TimeCount = 0.0;
		FrameCount = 0;
	}

	RenderString(50, engine.height - 50, font, cFPS);

	sprintf_s(pos, "X: %0.10f\n", engine.player.dPositionX/BLOCK_SIZE);
	RenderString(50, engine.height - 70, font, pos);
	sprintf_s(pos, "Y: %0.10f\n", engine.player.dPositionY/BLOCK_SIZE);
	RenderString(50, engine.height - 90, font, pos);
	sprintf_s(pos, "Z: %0.10f\n", engine.player.dPositionZ/BLOCK_SIZE);
	RenderString(50, engine.height - 110, font, pos);
}

void Statistics::RenderString(int x, int y, void *font, const char string[])
{
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}