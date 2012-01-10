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
	bool  bKeyboard[256];				// ������, ������������ ��� �������� � �����������

	GLdouble dDispCenterCoordX, dDispCenterCoordY, dDispCenterCoordZ;	// ������������ ������� ���������� ������
	signed short sCenterCubeCoordX, sCenterCubeCoordY, sCenterCubeCoordZ;	// ������������ ���������� ����

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

