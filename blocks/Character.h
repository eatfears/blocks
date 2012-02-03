#pragma once
#include "Blocks_Definitions.h"
#include <GLblocks\blocksglut.h>
//#include <gl\freeglut.h>

#include "World.h"

class Character
{
public:
	Character(World& ww);
	~Character();

	GLdouble dPositionX, dPositionY, dPositionZ;
//	LocInWorld lnwPositionX, lnwPositionZ;
	GLdouble dSpinY, dSpinX;
	GLdouble dVelocityX, dVelocityY, dVelocityZ;

	bool bFalling;
	void Control(GLdouble FrameInterval);
	void GetCenterCoords(GLsizei width, GLsizei height);
	bool  bKeyboardPress[256];					// Массив, используемый для операций с клавиатурой
	bool  bKeyboardHit[256];				// Массив, используемый для операций с клавиатурой

	GLdouble dDispCenterCoordX, dDispCenterCoordY, dDispCenterCoordZ;			// возвращаемые мировые координаты центра
	BlockInWorld sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ;	// возвращаемые координаты куба

	World& wWorld;

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

