#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "World.h"

class Character
{
public:
	Character();
	~Character();

	GLdouble dPositionX, dPositionY, dPositionZ;
	GLdouble dSpinY, dSpinX;
	GLdouble dVelocityX, dVelocityY, dVelocityZ;

	bool bFalling;
	void Control(GLdouble FrameInterval, World &wWorld);
	bool  bKeyboard[256];				// Массив, используемый для операций с клавиатурой

	GLdouble dDispCenterCoordX, dDispCenterCoordY, dDispCenterCoordZ;	// возвращаемые мировые координаты центра
	signed short sCenterCubeCoordX, sCenterCubeCoordY, sCenterCubeCoordZ;	// возвращаемые координаты куба

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

