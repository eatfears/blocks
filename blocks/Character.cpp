#include <math.h>

#include "Character.h"
#include "Blocks_Definitions.h"

Character::Character(void)
{
	falling = true;
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

void Character::Control(GLdouble FrameInterval, World &wWorld)
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
			gfVelX -= FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
			gfVelZ -= FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
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
			gfVelX += FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
			gfVelZ += FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
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
			gfVelX += FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
			gfVelZ -= FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
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
			gfVelX -= FrameInterval*AIR_ACCEL*step*cos(TORAD(gfSpinY));
			gfVelZ += FrameInterval*AIR_ACCEL*step*sin(TORAD(gfSpinY));
		}
	}

	GLdouble ko = gfVelX*gfVelX + gfVelZ*gfVelZ;
	if(ko > WALK_SPEED*WALK_SPEED*SPRINT_KOEF*SPRINT_KOEF)
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
			gfPosY += FrameInterval;
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

	int num = 100;
	int sq = 100;
	int sqb2 = sq/2;
	if(keys['1'])
	{
		int i = 0;
		
		while(i < num)
		{
			if(wWorld.AddTile(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, rand()%4+1))
				i++;
		}
	}
	if(keys['2'])
	{
		int i = 0;

		while(i < num)
		{
			if(wWorld.RmTile(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2))
				i++;
		}
	}

	if(keys['3'])
	{
		for (int i = -sqb2; i <= sqb2; i++)
		for (int j = -sqb2; j <= sqb2; j++)
		for (int k = -sqb2; k <= sqb2; k++)
		{
			wWorld.RmTile(i, j, k);	
		}
	}

	if(keys['4'])
	{
		sq = 42;
		sqb2 = sq/2;
		for (int k = -2; k <= 0; k++)
		for (int i = -sqb2; i <= sqb2; i++)
			for (int j = -sqb2; j <= sqb2; j++)
				{
					wWorld.RmTile(i, k, j);	
				}
	}

	if(keys['E']) 
	{
		wWorld.RmTile(xx,yy,zz);
	}

	if(keys['Q']) 
	{
		signed short ix = xx, iy = yy, iz = zz;
		if((zerr < xerr)&&(zerr < yerr))
		{
			if(gfPosZ < wz) iz = zz - 1;
			if(gfPosZ > wz) iz = zz + 1;
		}
		if((xerr < zerr)&&(xerr < yerr))
		{
			if(gfPosX < wx) ix = xx - 1;
			if(gfPosX > wx) ix = xx + 1;
		}
		if((yerr < xerr)&&(yerr < zerr))
		{
			if(gfPosY < wy) iy = yy - 1;
			if(gfPosY > wy) iy = yy + 1;
		}

		wWorld.AddTile(ix,iy,iz,MAT_DIRT);
	}

	gfPosX += FrameInterval*gfVelX;
	gfPosZ += FrameInterval*gfVelZ;

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