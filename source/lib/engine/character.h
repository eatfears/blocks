#pragma once

#include "chunk.h"


class World;

class Character
{
public:
    Character(World &world);

    PointInWorld m_Position;

    GLdouble m_SpinY, m_SpinX;
    GLdouble m_VelocityX, m_VelocityY, m_VelocityZ;
    GLdouble m_Longitude;

    void control(GLdouble frame_interval);
    void computeCenterCoords(GLsizei width, GLsizei height);
    bool  m_Keyboard[256];					// Массив, используемый для операций с клавиатурой
    bool  m_SpecialKeys[256];				// Массив, используемый для операций с клавиатурой

    BlockInWorld m_AimedBlock;          	// возвращаемые координаты куба
    BlockInWorld m_FreeBlock;
    PointInWorld m_CenterPos;

    World &m_World;
    bool m_UnderWater;
    Chunk *m_pChunk;
    unsigned int m_Index;
    double m_LocalTimeOfDay;

    void computeMyPosition();
    void computeLocalTime(double time_of_day);

private:
    void getPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr) const;
};

