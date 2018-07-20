#include "statistics.h"

#include <stdio.h>

#include "engine.h"
#include "platform.h"


Statistics::Statistics(const Engine &eng)
    : m_Engine(eng)
{
    m_TimeCount = 0;
    m_FrameCount = 0;
    m_ReRenderedChunks = 0;
}

void Statistics::computeFPS(double FrameInterval)
{
    m_FrameCount++;
    m_TimeCount += FrameInterval;
}

extern double constr;
extern double update;

void Statistics::printStat()
{
	static char cFPS[20] = "";
	static char pos[40] = "";
	static void *font = GLUT_BITMAP_HELVETICA_12;
	static int h, m, s;

    if (m_TimeCount > 100)
    {
        b_sprintf(cFPS, "FPS: %0.1f\n", 1000.0*m_FrameCount/m_TimeCount);
        m_TimeCount = 0.0;
        m_FrameCount = 0;
	}
	
    renderString(m_Engine.m_Width - 100, m_Engine.m_Height - 30, font, cFPS);

    const Character &player = m_Engine.m_World.m_Player;

	b_sprintf(pos, "CX: %d\n", player.m_Position.cx);
    renderString(50, m_Engine.m_Height - 30, font, pos);
	b_sprintf(pos, "CZ: %d\n", player.m_Position.cz);
    renderString(50, m_Engine.m_Height - 50, font, pos);
	b_sprintf(pos, "X: %0.10f\n", player.m_Position.bx);
    renderString(50, m_Engine.m_Height - 70, font, pos);
	b_sprintf(pos, "Y: %0.10f\n", player.m_Position.by);
    renderString(50, m_Engine.m_Height - 90, font, pos);
	b_sprintf(pos, "Z: %0.10f\n", player.m_Position.bz);
    renderString(50, m_Engine.m_Height - 110, font, pos);


	b_sprintf(pos, "CX: %d\n", player.m_CenterPos.cx);
    renderString(200, m_Engine.m_Height - 30, font, pos);
	b_sprintf(pos, "CZ: %d\n", player.m_CenterPos.cz);
    renderString(200, m_Engine.m_Height - 50, font, pos);
	b_sprintf(pos, "X: %0.10f\n", player.m_CenterPos.bx);
    renderString(200, m_Engine.m_Height - 70, font, pos);
	b_sprintf(pos, "Y: %0.10f\n", player.m_CenterPos.by);
    renderString(200, m_Engine.m_Height - 90, font, pos);
	b_sprintf(pos, "Z: %0.10f\n", player.m_CenterPos.bz);
    renderString(200, m_Engine.m_Height - 110, font, pos);

	b_sprintf(pos, "Constr: %0.10f\n", constr);
    renderString(200, m_Engine.m_Height - 130, font, pos);
	b_sprintf(pos, "Update: %0.10f\n", update);
    renderString(200, m_Engine.m_Height - 150, font, pos);

    b_sprintf(pos, "Rendered: %d\n", m_ReRenderedChunks);
    renderString(50, m_Engine.m_Height - 130, font, pos);

    h = ((int)m_Engine.m_TimeOfDay)/100;
    m = ((int)(m_Engine.m_TimeOfDay*0.6))%60;
    s = ((int)(m_Engine.m_TimeOfDay*36.0))%60;
	b_sprintf(pos, "Time: %d:%.2d:%.2d\n", h, m, s);
    renderString(50, m_Engine.m_Height - 150, font, pos);

    h = ((int)m_Engine.m_World.m_Player.m_LocalTimeOfDay)/100;
    m = ((int)(m_Engine.m_World.m_Player.m_LocalTimeOfDay*0.6))%60;
    s = ((int)(m_Engine.m_World.m_Player.m_LocalTimeOfDay*36.0))%60;
	b_sprintf(pos, "Local time: %d:%.2d:%.2d\n", h, m, s);
    renderString(50, m_Engine.m_Height - 170, font, pos);
}

void Statistics::renderString(int x, int y, void *font, const char string[]) const
{
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}
