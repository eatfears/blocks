#include <math.h>

#include "Character.h"
#include "Blocks_Definitions.h"

Character::Character(void)
{
	bFalling = true;
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
	*xerr = dDispCenterCoordX + TILE_SIZE/2;
	*yerr = dDispCenterCoordY;
	*zerr = dDispCenterCoordZ + TILE_SIZE/2;

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
	if(bKeyboard[VK_SHIFT]) step *= SPRINT_KOEF;


	if(bKeyboard['W']) 
	{
		if(!bFalling)
		{
			dVelocityX -= step*sin(TORAD(dSpinY));
			dVelocityZ -= step*cos(TORAD(dSpinY));
		}
		else
		{
			dVelocityX -= FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
			dVelocityZ -= FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
		}
	}
	if(bKeyboard['S']) 
	{
		if(!bFalling)
		{
			dVelocityX += step*sin(TORAD(dSpinY));
			dVelocityZ += step*cos(TORAD(dSpinY));
		}
		else
		{
			dVelocityX += FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
			dVelocityZ += FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
		}
	}
	if(bKeyboard['D']) 
	{
		if(!bFalling)
		{
			dVelocityX += step*cos(TORAD(dSpinY));
			dVelocityZ -= step*sin(TORAD(dSpinY));
		}
		else
		{
			dVelocityX += FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
			dVelocityZ -= FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
		}
	}
	if(bKeyboard['A']) 
	{
		if(!bFalling)
		{
			dVelocityX -= step*cos(TORAD(dSpinY));
			dVelocityZ += step*sin(TORAD(dSpinY));
		}
		else
		{
			dVelocityX -= FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
			dVelocityZ += FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
		}
	}

	GLdouble ko = dVelocityX*dVelocityX + dVelocityZ*dVelocityZ;
	if(ko > WALK_SPEED*WALK_SPEED*SPRINT_KOEF*SPRINT_KOEF)
	{
		ko = pow(ko, 0.5);
		dVelocityX = dVelocityX*step/ko;
		dVelocityZ = dVelocityZ*step/ko;
	}

	if(bKeyboard[VK_SPACE])
	{
		if(!bFalling)
		{
			dVelocityY = -JUMP_STR;
			bFalling = true;
			dPositionY += FrameInterval;
		}
	}

	GLdouble yerr, xerr, zerr;
	GetPlane(&xerr, &yerr, &zerr);

	if((zerr < xerr)&&(zerr < yerr))
	{
		sCenterCubeCoordX = floor(dDispCenterCoordX/TILE_SIZE + 0.5);
		sCenterCubeCoordY = floor(dDispCenterCoordY/TILE_SIZE);

		if(dPositionZ < dDispCenterCoordZ) sCenterCubeCoordZ = round(dDispCenterCoordZ/TILE_SIZE + 0.5);
		if(dPositionZ > dDispCenterCoordZ) sCenterCubeCoordZ = round(dDispCenterCoordZ/TILE_SIZE - 0.5);
	}
	if((xerr < zerr)&&(xerr < yerr))
	{
		sCenterCubeCoordZ = floor(dDispCenterCoordZ/TILE_SIZE + 0.5);
		sCenterCubeCoordY = floor(dDispCenterCoordY/TILE_SIZE);

		if(dPositionX < dDispCenterCoordX) sCenterCubeCoordX = round(dDispCenterCoordX/TILE_SIZE + 0.5);
		if(dPositionX > dDispCenterCoordX) sCenterCubeCoordX = round(dDispCenterCoordX/TILE_SIZE - 0.5);
	}
	if((yerr < xerr)&&(yerr < zerr))
	{
		sCenterCubeCoordX = floor(dDispCenterCoordX/TILE_SIZE + 0.5);
		sCenterCubeCoordZ = floor(dDispCenterCoordZ/TILE_SIZE + 0.5);

		if(dPositionY < dDispCenterCoordY) sCenterCubeCoordY = round(dDispCenterCoordY/TILE_SIZE);
		if(dPositionY > dDispCenterCoordY) sCenterCubeCoordY = round(dDispCenterCoordY/TILE_SIZE - 1.0);
	}

	int num = 100;
	int sq = 100;
	int sqb2 = sq/2;
	if(bKeyboard['1'])
	{
		int i = 0;
		
		while(i < num)
		{
			if(wWorld.AddTile(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, rand()%4+1))
				i++;
		}
	}
	if(bKeyboard['2'])
	{
		int i = 0;

		while(i < num)
		{
			if(wWorld.RemoveTile(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2))
				i++;
		}
	}

	if(bKeyboard['3'])
	{
		for (int i = -sqb2; i <= sqb2; i++)
		for (int j = -sqb2; j <= sqb2; j++)
		for (int k = -sqb2; k <= sqb2; k++)
		{
			wWorld.RemoveTile(i, j, k);	
		}
	}

	if(bKeyboard['4'])
	{
		sq = 42;
		sqb2 = sq/2;
		for (int k = -2; k <= 0; k++)
		for (int i = -sqb2; i <= sqb2; i++)
			for (int j = -sqb2; j <= sqb2; j++)
				{
					wWorld.RemoveTile(i, k, j);	
				}
	}

	if(bKeyboard['E']) 
	{
		wWorld.RemoveTile(sCenterCubeCoordX,sCenterCubeCoordY,sCenterCubeCoordZ);
	}

	if(bKeyboard['Q']) 
	{
		signed short ix = sCenterCubeCoordX, iy = sCenterCubeCoordY, iz = sCenterCubeCoordZ;
		if((zerr < xerr)&&(zerr < yerr))
		{
			if(dPositionZ < dDispCenterCoordZ) iz = sCenterCubeCoordZ - 1;
			if(dPositionZ > dDispCenterCoordZ) iz = sCenterCubeCoordZ + 1;
		}
		if((xerr < zerr)&&(xerr < yerr))
		{
			if(dPositionX < dDispCenterCoordX) ix = sCenterCubeCoordX - 1;
			if(dPositionX > dDispCenterCoordX) ix = sCenterCubeCoordX + 1;
		}
		if((yerr < xerr)&&(yerr < zerr))
		{
			if(dPositionY < dDispCenterCoordY) iy = sCenterCubeCoordY - 1;
			if(dPositionY > dDispCenterCoordY) iy = sCenterCubeCoordY + 1;
		}

		wWorld.AddTile(ix,iy,iz,MAT_DIRT);
	}

	dPositionX += FrameInterval*dVelocityX;
	dPositionZ += FrameInterval*dVelocityZ;

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