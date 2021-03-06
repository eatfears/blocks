#pragma once


class Engine;

class Statistics
{
public:
    Statistics(const Engine &engine);

    void computeFPS(double frame_interval);
    void printStat();

    int m_ReRenderedChunks;

private:
    const Engine &m_Engine;
    void renderString(int x, int y, void *font, const char string[]) const;

    double m_TimeCount;
    int m_FrameCount;
};
