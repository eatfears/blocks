#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

class Character
{
public:
	Character(void);
	~Character(void);

	GLdouble gfPosX, gfPosY, gfPosZ;
	GLdouble gfSpinY, gfSpinX;
};

