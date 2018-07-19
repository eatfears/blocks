#pragma once

#include "definitions.h"
#include <GL/freeglut.h>

class Engine;

class Statistics
{
public:
	Statistics(Engine& eng);
	~Statistics(void);

    void computeFPS( double FrameInterval );
    void printStat(void);

    int m_ReRenderedChunks;

private:
    Engine& m_Engine;
    void renderString(int x, int y, void *font, const char string[]);

    double m_TimeCount;
    int m_FrameCount;
};
