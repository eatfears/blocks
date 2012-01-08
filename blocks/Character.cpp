#include "Character.h"

#include <math.h>

#define TORAD(X) X*0.01745329251994329576923690768489
#define TODEG(X) X*57.295779513082320876798154814105

#define TILE_SIZE 10.0

#define STEP_DOWNSTEP	0.6
#define MAX_DOWNSTEP	7.0
#define PLAYER_HEIGHT	15.0
#define AIR_ACCEL		0.12
#define JUMP_STR		4.0
#define WALK_SPEED		1.5

#define SPRINT_KOEF		2.0

#define FOG_COLOR		0.5f,0.5f,0.8f,1.0f
#define FOG_DENSITY		10.0
#define FOG_START		TILE_SIZE*10
#define MAX_VIEV_DIST	TILE_SIZE*80

Character::Character(void)
{
}


Character::~Character(void)
{
}

double round(double x)
{
	if ( x - floor(x) >= 0.5) return ceil(x);
	return floor(x);
}

void Character::GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr)
{
	*xerr = wx + TILE_SIZE/2;
	*yerr = wy;
	*zerr = wz + TILE_SIZE/2;

	while (*yerr < -1) *yerr += TILE_SIZE;
	while (*yerr > TILE_SIZE + 1) *yerr -= TILE_SIZE;

	*yerr = abs(*yerr);
	if(*yerr > abs(*yerr - TILE_SIZE)) *yerr = abs(*yerr - TILE_SIZE); 

	while (*xerr < - 1) *xerr += TILE_SIZE;
	while (*xerr > TILE_SIZE + 1) *xerr -= TILE_SIZE;

	*xerr = abs(*xerr);
	if(*xerr > abs(*xerr - TILE_SIZE)) *xerr = abs(*xerr - TILE_SIZE); 

	while (*zerr < - 1) *zerr += TILE_SIZE;
	while (*zerr > TILE_SIZE + 1) *zerr -= TILE_SIZE;

	*zerr = abs(*zerr);
	if(*zerr > abs(*zerr - TILE_SIZE)) *zerr = abs(*zerr - TILE_SIZE); 
}

void Character::Control(GLdouble g_FrameInterval)
{
	GLdouble step = WALK_SPEED;
	if(keys[VK_SHIFT]) step *= SPRINT_KOEF;


	if(keys['W']) 
	{
		if(!falling)
		{
			gfVelX -= step*sin(TORAD(gfSpinY));
			gfVelZ -= step*cos(TORAD(gfSpinY));
		}
		else
		{
			gfVelX -= g_FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
			gfVelZ -= g_FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
		}
	}
	if(keys['S']) 
	{
		if(!falling)
		{
			gfVelX += step*sin(TORAD(gfSpinY));
			gfVelZ += step*cos(TORAD(gfSpinY));
		}
		else
		{
			gfVelX += g_FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
			gfVelZ += g_FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
		}
	}
	if(keys['D']) 
	{
		if(!falling)
		{
			gfVelX += step*cos(TORAD(gfSpinY));
			gfVelZ -= step*sin(TORAD(gfSpinY));
		}
		else
		{
			gfVelX += g_FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
			gfVelZ -= g_FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
		}
	}
	if(keys['A']) 
	{
		if(!falling)
		{
			gfVelX -= step*cos(TORAD(gfSpinY));
			gfVelZ += step*sin(TORAD(gfSpinY));
		}
		else
		{
			gfVelX -= g_FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
			gfVelZ += g_FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
		}
	}

	GLdouble ko = gfVelX*gfVelX + gfVelZ*gfVelZ;
	if(ko > step*step)
	{
		ko = pow(ko, 0.5);
		gfVelX = gfVelX*step/ko;
		gfVelZ = gfVelZ*step/ko;
	}

	if(keys[VK_SPACE])
	{
		if(!falling)
		{
			gfVelY = -JUMP_STR;
			falling = true;
			gfPosY += g_FrameInterval;
		}
	}

	GLdouble yerr, xerr, zerr;
	GetPlane(&xerr, &yerr, &zerr);

	if((zerr < xerr)&&(zerr < yerr))
	{
		xx = floor(wx/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if(gfPosZ < wz) zz = round(wz/TILE_SIZE + 0.5);
		if(gfPosZ > wz) zz = round(wz/TILE_SIZE - 0.5);
	}
	if((xerr < zerr)&&(xerr < yerr))
	{
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if(gfPosX < wx) xx = round(wx/TILE_SIZE + 0.5);
		if(gfPosX > wx) xx = round(wx/TILE_SIZE - 0.5);
	}
	if((yerr < xerr)&&(yerr < zerr))
	{
		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);

		if(gfPosY < wy) yy = round(wy/TILE_SIZE);
		if(gfPosY > wy) yy = round(wy/TILE_SIZE - 1.0);
	}

	if(keys['E']) 
	{
		//RmTile(xx,yy,zz);
	}

	if(keys['Q']) 
	{
		if((zerr < xerr)&&(zerr < yerr))
		{
			if(gfPosZ < wz) zz -= 1;
			if(gfPosZ > wz) zz += 1;
		}
		if((xerr < zerr)&&(xerr < yerr))
		{
			if(gfPosX < wx) xx -= 1;
			if(gfPosX > wx) xx += 1;
		}
		if((yerr < xerr)&&(yerr < zerr))
		{
			if(gfPosY < wy) yy -= 1;
			if(gfPosY > wy) yy += 1;
		}

		//AddTile(xx,yy,zz,1);
	}

	/*{
		signed short xx, yy, zz;
		GLdouble wx = gfPosX + gfVelX;
		GLdouble wy = gfPosY - PLAYER_HEIGHT + 0.1;
		GLdouble wz = gfPosZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if((FindTile(xx, yy, zz) == NULL)&&(FindTile(xx, yy + 1, zz) == NULL))
			gfPosX += g_FrameInterval*gfVelX;
		else gfVelX = 0;
	}
	{
		signed short xx, yy, zz;
		GLdouble wx = gfPosX;
		GLdouble wy = gfPosY - PLAYER_HEIGHT + 0.1;
		GLdouble wz = gfPosZ + gfVelZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if((FindTile(xx, yy, zz) == NULL)&&(FindTile(xx, yy + 1, zz) == NULL))
			gfPosZ += g_FrameInterval*gfVelZ;
		else gfVelZ = 0;
	}
	*/
	/*if(falling)
	{
		gfPosY -= g_FrameInterval*gfVelY;
		if(gfVelY < MAX_DOWNSTEP)
			gfVelY += g_FrameInterval*STEP_DOWNSTEP;
	}*/
	/*
	{
		signed short xx, yy, zz;
		GLdouble wx = gfPosX;
		GLdouble wy = gfPosY - PLAYER_HEIGHT;
		GLdouble wz = gfPosZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if(FindTile(xx, yy, zz) == NULL) falling = true;
		else
		{
			gfVelY = 0;
			if(falling)
			{	
				falling = false;
				gfPosY = (yy + 1)*TILE_SIZE + PLAYER_HEIGHT - 0.001;
			}
		}

	}

	if(!falling)
	{
		gfVelX = 0;
		gfVelZ = 0;
	}*/
	//falling = 1;

}