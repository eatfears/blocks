#include "Statistics.h"
#include <stdio.h>
#include "Engine.h"
#include "Platform.h"


Statistics::Statistics(Engine& eng)
    : m_Engine(eng)
{
    m_TimeCount = 0;
    m_FrameCount = 0;
    m_ReRenderedChunks = 0;
}

Statistics::~Statistics(void)
{
}

void Statistics::computeFPS( double FrameInterval )
{
    m_FrameCount++;
    m_TimeCount += FrameInterval;
}

extern double constr;
extern double update;

void Statistics::printStat(void)
{
	static char cFPS[20] = "";
	static char pos[40] = "";
	static void *font = GLUT_BITMAP_HELVETICA_12;
	static int h, m, s;

    if (m_TimeCount > 100) {
        b_sprintf(cFPS, "FPS: %0.1f\n", 1000.0*m_FrameCount/m_TimeCount);
        m_TimeCount = 0.0;
        m_FrameCount = 0;
	}
	
    renderString(m_Engine.width - 100, m_Engine.height - 30, font, cFPS);

    Character &player = m_Engine.m_World.player;

	b_sprintf(pos, "CX: %d\n", player.position.cx);
    renderString(50, m_Engine.height - 30, font, pos);
	b_sprintf(pos, "CZ: %d\n", player.position.cz);
    renderString(50, m_Engine.height - 50, font, pos);
	b_sprintf(pos, "X: %0.10f\n", player.position.bx);
    renderString(50, m_Engine.height - 70, font, pos);
	b_sprintf(pos, "Y: %0.10f\n", player.position.by);
    renderString(50, m_Engine.height - 90, font, pos);
	b_sprintf(pos, "Z: %0.10f\n", player.position.bz);
    renderString(50, m_Engine.height - 110, font, pos);


	b_sprintf(pos, "CX: %d\n", player.centerPos.cx);
    renderString(200, m_Engine.height - 30, font, pos);
	b_sprintf(pos, "CZ: %d\n", player.centerPos.cz);
    renderString(200, m_Engine.height - 50, font, pos);
	b_sprintf(pos, "X: %0.10f\n", player.centerPos.bx);
    renderString(200, m_Engine.height - 70, font, pos);
	b_sprintf(pos, "Y: %0.10f\n", player.centerPos.by);
    renderString(200, m_Engine.height - 90, font, pos);
	b_sprintf(pos, "Z: %0.10f\n", player.centerPos.bz);
    renderString(200, m_Engine.height - 110, font, pos);

	b_sprintf(pos, "Constr: %0.10f\n", constr);
    renderString(200, m_Engine.height - 130, font, pos);
	b_sprintf(pos, "Update: %0.10f\n", update);
    renderString(200, m_Engine.height - 150, font, pos);

    b_sprintf(pos, "Rendered: %d\n", m_ReRenderedChunks);
    renderString(50, m_Engine.height - 130, font, pos);

    h = ((int)m_Engine.m_TimeOfDay)/100;
    m = ((int)(m_Engine.m_TimeOfDay*0.6))%60;
    s = ((int)(m_Engine.m_TimeOfDay*36.0))%60;
	b_sprintf(pos, "Time: %d:%.2d:%.2d\n", h, m, s);
    renderString(50, m_Engine.height - 150, font, pos);

    h = ((int)m_Engine.m_World.player.LocalTimeOfDay)/100;
    m = ((int)(m_Engine.m_World.player.LocalTimeOfDay*0.6))%60;
    s = ((int)(m_Engine.m_World.player.LocalTimeOfDay*36.0))%60;
	b_sprintf(pos, "Local time: %d:%.2d:%.2d\n", h, m, s);
    renderString(50, m_Engine.height - 170, font, pos);
}

void Statistics::renderString(int x, int y, void *font, const char string[])
{
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}
