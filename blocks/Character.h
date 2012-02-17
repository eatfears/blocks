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

	bool bFalling;
	void Control(GLdouble FrameInterval);
	void GetCenterCoords(GLsizei width, GLsizei height);
	bool  bKeyboard[256];					// ������, ������������ ��� �������� � �����������
	bool  bSpecial[256];					// ������, ������������ ��� �������� � �����������

	GLdouble dDispCenterCoordX, dDispCenterCoordY, dDispCenterCoordZ;			// ������������ ������� ���������� ������
	BlockInWorld sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ;	// ������������ ���������� ����

	World& wWorld;
	bool UnderWater;
	Chunk *chunk;
	int index;
	void GetMyPosition();

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

