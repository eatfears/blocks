#include "Statistic.h"
#include <stdio.h>

Statistic::Statistic(void)
{
	TimeCount = 0;
	FrameCount = 0;
}

Statistic::~Statistic(void)
{
}

void Statistic::ComputeFPS( double FrameInterval )
{
	FrameCount++;
	TimeCount += FrameInterval;
}

void Statistic::PrintStat(int width, int height)
{
	static char cFPS[20] = "";

	if (TimeCount > 100)
	{
		sprintf_s(cFPS, "FPS = %0.1f\n", 1000.0*FrameCount/TimeCount);
		TimeCount = 0.0;
		FrameCount = 0;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	RenderString(50, height - 50, GLUT_BITMAP_HELVETICA_12, cFPS, 1.0f, 1.0f, 1.0f, 1.0f);
}

void Statistic::RenderString(int x, int y, void *font, const char string[], GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glColor4f(r, g, b, a);
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}