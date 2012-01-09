#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "World.h"

class Character
{
public:
	Character(void);
	~Character(void);

	GLdouble gfPosX, gfPosY, gfPosZ;
	GLdouble gfSpinY, gfSpinX;
	GLdouble gfVelX, gfVelY, gfVelZ;

	bool falling;
	void Control(GLdouble FrameInterval, World &wWorld);
	bool  keys[256];				// Массив, используемый для операций с клавиатурой

	GLdouble wx,wy,wz;			// возвращаемые мировые координаты центра
	signed short xx, yy, zz;	// возвращаемые координаты куба

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

