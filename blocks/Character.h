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
	bool  keys[256];				// ������, ������������ ��� �������� � �����������

	GLdouble wx,wy,wz;			// ������������ ������� ���������� ������
	signed short xx, yy, zz;	// ������������ ���������� ����

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

