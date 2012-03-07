#pragma once
#include "Blocks_Definitions.h"
#include <GL/freeglut.h>
#include "Chunk.h"

class World;

class Character
{
public:
	Character(World& ww);
	~Character();

	GLdouble dPositionX, dPositionY, dPositionZ;
//	LocInWorld lnwPositionX, lnwPositionZ;
	GLdouble dSpinY, dSpinX;
	GLdouble dVelocityX, dVelocityY, dVelocityZ;
	GLdouble Longitude;
	GLdouble Longitude2;

	bool bFalling;
	void Control(GLdouble FrameInterval);
	void GetCenterCoords(GLsizei width, GLsizei height);
	bool  bKeyboard[256];					// Массив, используемый для операций с клавиатурой
	bool  bSpecial[256];					// Массив, используемый для операций с клавиатурой

	GLdouble dDispCenterCoordX, dDispCenterCoordY, dDispCenterCoordZ;			// возвращаемые мировые координаты центра
	BlockInWorld sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ;	// возвращаемые координаты куба

	World& wWorld;
	bool UnderWater;
	Chunk *chunk;
	int index;
	void GetMyPosition();
	double LocalTimeOfWinal;
	double LocalTimeOfDay;
	void GetLocalTime(double TimeOfDay, double TimeOfWinal);

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

