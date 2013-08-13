#include "Statistics.h"
#include <stdio.h>
#include "Engine.h"
#include "Platform.h"


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

extern double constr;
extern double update;

void Statistics::PrintStat(void)
{
	static char cFPS[20] = "";
	static char pos[40] = "";
	static void *font = GLUT_BITMAP_HELVETICA_12;
	static int h, m, s;

	if (TimeCount > 100) {
		b_sprintf(cFPS, "FPS: %0.1f\n", 1000.0*FrameCount/TimeCount);
		TimeCount = 0.0;
		FrameCount = 0;
	}
	
	RenderString(engine.width - 100, engine.height - 30, font, cFPS);

	Character &player = engine.wWorld.player;

	b_sprintf(pos, "CX: %d\n", player.position.cx);
	RenderString(50, engine.height - 30, font, pos);
	b_sprintf(pos, "CZ: %d\n", player.position.cz);
	RenderString(50, engine.height - 50, font, pos);
	b_sprintf(pos, "X: %0.10f\n", player.position.bx);
	RenderString(50, engine.height - 70, font, pos);
	b_sprintf(pos, "Y: %0.10f\n", player.position.by);
	RenderString(50, engine.height - 90, font, pos);
	b_sprintf(pos, "Z: %0.10f\n", player.position.bz);
	RenderString(50, engine.height - 110, font, pos);


	b_sprintf(pos, "CX: %d\n", player.centerPos.cx);
	RenderString(200, engine.height - 30, font, pos);
	b_sprintf(pos, "CZ: %d\n", player.centerPos.cz);
	RenderString(200, engine.height - 50, font, pos);
	b_sprintf(pos, "X: %0.10f\n", player.centerPos.bx);
	RenderString(200, engine.height - 70, font, pos);
	b_sprintf(pos, "Y: %0.10f\n", player.centerPos.by);
	RenderString(200, engine.height - 90, font, pos);
	b_sprintf(pos, "Z: %0.10f\n", player.centerPos.bz);
	RenderString(200, engine.height - 110, font, pos);

	b_sprintf(pos, "Constr: %0.10f\n", constr);
	RenderString(200, engine.height - 130, font, pos);
	b_sprintf(pos, "Update: %0.10f\n", update);
	RenderString(200, engine.height - 150, font, pos);

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
