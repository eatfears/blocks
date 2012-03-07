#include "Statistics.h"
#include <stdio.h>
#include "Engine.h"
#include "PlatformDefinitions.h"


Statistics::Statistics(Engine& eng)
	: engine(eng)
{
	TimeCount = 0;
	FrameCount = 0;
	reRenderedChunks = 0;
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
	static void *font = GLUT_BITMAP_HELVETICA_12;
	static int h, m, s;

	if (TimeCount > 100)
	{
		b_sprintf(cFPS, "FPS: %0.1f\n", 1000.0*FrameCount/TimeCount);
		TimeCount = 0.0;
		FrameCount = 0;
	}

	RenderString(50, engine.height - 50, font, cFPS);

	Character &player = engine.wWorld.player;
	b_sprintf(pos, "X: %0.10f\n", player.dPositionX/BLOCK_SIZE);
	RenderString(50, engine.height - 70, font, pos);
	b_sprintf(pos, "Y: %0.10f\n", player.dPositionY/BLOCK_SIZE);
	RenderString(50, engine.height - 90, font, pos);
	b_sprintf(pos, "Z: %0.10f\n", player.dPositionZ/BLOCK_SIZE);
	RenderString(50, engine.height - 110, font, pos);

	b_sprintf(pos, "Rendered: %d\n", reRenderedChunks);
	RenderString(50, engine.height - 130, font, pos);

	h = ((int)engine.TimeOfDay)/100;
	m = ((int)(engine.TimeOfDay*0.6))%60;
	s = ((int)(engine.TimeOfDay*36.0))%60;
	b_sprintf(pos, "Time: %d:%.2d:%.2d\n", h, m, s);
	RenderString(50, engine.height - 150, font, pos);

	h = ((int)engine.wWorld.player.LocalTimeOfDay)/100;
	m = ((int)(engine.wWorld.player.LocalTimeOfDay*0.6))%60;
	s = ((int)(engine.wWorld.player.LocalTimeOfDay*36.0))%60;
	b_sprintf(pos, "Local time: %d:%.2d:%.2d\n", h, m, s);
	RenderString(50, engine.height - 170, font, pos);
}

void Statistics::RenderString(int x, int y, void *font, const char string[])
{
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}
