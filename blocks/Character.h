#pragma once
#include "Definitions.h"
#include <GL/freeglut.h>
#include "Chunk.h"

class World;

class Character
{
public:
	Character(World& ww);
	~Character();

//	GLdouble dPositionX, dPositionY, dPositionZ;
	PosInWorld position;
//	LocInWorld lnwPositionX, lnwPositionZ;
	GLdouble dSpinY, dSpinX;
	GLdouble dVelocityX, dVelocityY, dVelocityZ;
	GLdouble Longitude;
	GLdouble Longitude2;

	void Control(GLdouble FrameInterval);
	void GetCenterCoords(GLsizei width, GLsizei height);
	bool  bKeyboard[256];					// ������, ������������ ��� �������� � �����������
	bool  bSpecial[256];					// ������, ������������ ��� �������� � �����������

	GLdouble dDispCenterCoordX, dDispCenterCoordY, dDispCenterCoordZ;			// ������������ ������� ���������� ������
	BlockInWorld sCenterBlockCoord;	// ������������ ���������� ����

	World& wWorld;
	bool underWater;
	Chunk *chunk;
	int index;
	void GetMyPosition();
	double LocalTimeOfWinal;
	double LocalTimeOfDay;
	void GetLocalTime(double TimeOfDay, double TimeOfWinal);

private:
	void GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr);
};

