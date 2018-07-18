#pragma once
#include "Definitions.h"
#include <GL/freeglut.h>
#include "Chunk.h"

class World;

class Character
{
public:
    Character(World& ww);

	PosInWorld position;

	GLdouble dSpinY, dSpinX;
	GLdouble dVelocityX, dVelocityY, dVelocityZ;
	GLdouble Longitude;
	GLdouble Longitude2;

	void Control(GLdouble FrameInterval);
	void GetCenterCoords(GLsizei width, GLsizei height);
	bool  bKeyboard[256];					// Массив, используемый для операций с клавиатурой
	bool  bSpecial[256];					// Массив, используемый для операций с клавиатурой

	BlockInWorld aimedBlock;	// возвращаемые координаты куба
	BlockInWorld freeBlock;
	PosInWorld centerPos;

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

