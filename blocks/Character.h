#pragma once
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

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
	bool  bKeyboard[256];					// ������, ������������ ��� �������� � �����������
	bool  bKeyboardDown[256];				// ������, ������������ ��� �������� � �����������

	GLdouble dDispCenterCoordX, dDispCenterCoordY, dDispCenterCoordZ;			// ������������ ������� ���������� ������
	BlockInWorld sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ;	// ������������ ���������� ����

	World& wWorld;

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);

};

