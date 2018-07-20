#pragma once

#define FREEGLUT_STATIC
#include <GL/freeglut.h>

#include "chunk.h"


class World;

class Character
{
public:
    Character(World &ww);

    PointInWorld position;

    GLdouble dSpinY, dSpinX;
    GLdouble dVelocityX, dVelocityY, dVelocityZ;
    GLdouble Longitude;

    void control(GLdouble FrameInterval);
    void getCenterCoords(GLsizei width, GLsizei height);
    bool  bKeyboard[256];					// Массив, используемый для операций с клавиатурой
    bool  bSpecial[256];					// Массив, используемый для операций с клавиатурой

    BlockInWorld aimedBlock;	// возвращаемые координаты куба
    BlockInWorld freeBlock;
    PointInWorld centerPos;

    World& wWorld;
    bool underWater;
    Chunk *chunk;
    int index;
    double LocalTimeOfDay;

    void getMyPosition();
    void getLocalTime(double TimeOfDay);

private:
    void getPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

