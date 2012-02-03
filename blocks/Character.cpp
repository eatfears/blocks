#include "Character.h"
#include <math.h>
#include <process.h>

#include "PerlinNoise.h"

void LoadNGenerate(void* pParams)
{
	Param pParameters = *(Param*)pParams;
	World &wWorld = *pParameters.wWorld;
	ChunkInWorld x = pParameters.x;
	ChunkInWorld z = pParameters.z;
	SetEvent(wWorld.parget2);

	int size = 16;

	for(int i = x*size; i < (x+1)*size*1; i++)
		for(int j = z*size; j < (z+1)*size; j++)
	{
		wWorld.LoadChunk(i, j);
		//Sleep(30);
	}
	_endthread();
	return;
}

Character::Character(World& ww)
	: wWorld(ww)
{
	bFalling = true;
	for(int i = 0; i < 256; i++) 
	{
		bKeyboardPress[i] = false;
		bKeyboardHit[i] = true;
	}
}

Character::~Character()
{
}

double round(double x)
{
	if( x - floor(x) >= 0.5) return ceil(x);
	return floor(x);
}

void Character::GetPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr)
{
	*xerr = dDispCenterCoordX + BLOCK_SIZE/2;
	*yerr = dDispCenterCoordY;
	*zerr = dDispCenterCoordZ + BLOCK_SIZE/2;

	while(*yerr < -1) *yerr += BLOCK_SIZE;
	while(*yerr > BLOCK_SIZE + 1) *yerr -= BLOCK_SIZE;

	*yerr = abs(*yerr);
	if(*yerr > abs(*yerr - BLOCK_SIZE)) *yerr = abs(*yerr - BLOCK_SIZE); 

	while(*xerr < - 1) *xerr += BLOCK_SIZE;
	while(*xerr > BLOCK_SIZE + 1) *xerr -= BLOCK_SIZE;

	*xerr = abs(*xerr);
	if(*xerr > abs(*xerr - BLOCK_SIZE)) *xerr = abs(*xerr - BLOCK_SIZE); 

	while(*zerr < - 1) *zerr += BLOCK_SIZE;
	while(*zerr > BLOCK_SIZE + 1) *zerr -= BLOCK_SIZE;

	*zerr = abs(*zerr);
	if(*zerr > abs(*zerr - BLOCK_SIZE)) *zerr = abs(*zerr - BLOCK_SIZE); 
}

void Character::Control(GLdouble FrameInterval)
{
	GLdouble step = WALK_SPEED;
	if(bKeyboardPress[VK_SHIFT]) step *= SPRINT_KOEF;


	if(bKeyboardPress['W']) 
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
	if(bKeyboardPress['S']) 
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
	if(bKeyboardPress['D']) 
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
	if(bKeyboardPress['A']) 
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
	if(bKeyboardPress['R']) 
	{
		dVelocityY += FrameInterval*AIR_ACCEL*step;
	}
	if(bKeyboardPress['F']) 
	{
		dVelocityY -= FrameInterval*AIR_ACCEL*step;
	}

	GLdouble ko = dVelocityX*dVelocityX + dVelocityZ*dVelocityZ;
	if(ko > WALK_SPEED*WALK_SPEED*SPRINT_KOEF*SPRINT_KOEF)
	{
		ko = pow(ko, 0.5);
		dVelocityX = dVelocityX*WALK_SPEED*SPRINT_KOEF/ko;
		dVelocityZ = dVelocityZ*WALK_SPEED*SPRINT_KOEF/ko;
	}

	if(bKeyboardPress[VK_SPACE])
	{
		if(!bFalling)
		{
			dVelocityY = -JUMP_STR;
			bFalling = true;
			dPositionY += FrameInterval;
		}
	}

	if(bKeyboardPress['X'])
	{
		dVelocityX = 0;
		dVelocityY = 0;
		dVelocityZ = 0;
	}


	GLdouble yerr, xerr, zerr;
	GetPlane(&xerr, &yerr, &zerr);

	if((zerr < xerr)&&(zerr < yerr))
	{
		sCenterBlockCoordX = (BlockInWorld) floor(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		sCenterBlockCoordY = (BlockInWorld) floor(dDispCenterCoordY/BLOCK_SIZE);

		if(dPositionZ < dDispCenterCoordZ) sCenterBlockCoordZ = (BlockInWorld) round(dDispCenterCoordZ/BLOCK_SIZE + 0.5);
		if(dPositionZ > dDispCenterCoordZ) sCenterBlockCoordZ = (BlockInWorld) round(dDispCenterCoordZ/BLOCK_SIZE - 0.5);
	}
	if((xerr < zerr)&&(xerr < yerr))
	{
		sCenterBlockCoordZ = (BlockInWorld) floor(dDispCenterCoordZ/BLOCK_SIZE + 0.5);
		sCenterBlockCoordY = (BlockInWorld) floor(dDispCenterCoordY/BLOCK_SIZE);

		if(dPositionX < dDispCenterCoordX) sCenterBlockCoordX = (BlockInWorld) round(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		if(dPositionX > dDispCenterCoordX) sCenterBlockCoordX = (BlockInWorld) round(dDispCenterCoordX/BLOCK_SIZE - 0.5);
	}
	if((yerr < xerr)&&(yerr < zerr))
	{
		sCenterBlockCoordX = (BlockInWorld) floor(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		sCenterBlockCoordZ = (BlockInWorld) floor(dDispCenterCoordZ/BLOCK_SIZE + 0.5);

		if(dPositionY < dDispCenterCoordY) sCenterBlockCoordY = (BlockInWorld) round(dDispCenterCoordY/BLOCK_SIZE);
		if(dPositionY > dDispCenterCoordY) sCenterBlockCoordY = (BlockInWorld) round(dDispCenterCoordY/BLOCK_SIZE - 1.0);
	}

	int num = 100;
	int sq = 100;
	int sqb2 = sq/2;
	if(bKeyboardPress['1'])
	{
		int i = 0;
		
		while(i < num)
		{
			if(wWorld.AddBlock(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, rand()%14+1, true))
				i++;
		}
	}
	if(bKeyboardPress['2'])
	{
		int i = 0;

		while(i < num)
		{
			if(wWorld.RemoveBlock(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, true))
				i++;
		}
	}
 
	if(bKeyboardPress['3'])
	{
		for(int i = -sqb2; i <= sqb2; i++)
		for(int j = -sqb2; j <= sqb2; j++)
		for(int k = -sqb2; k <= sqb2; k++)
		{
			wWorld.RemoveBlock(i, j, k, true);	
		}
	}

	if(bKeyboardPress['4'])
	{
		if(bKeyboardHit['4'])
		{
			wWorld.LoadChunk(0, 0);	
			bKeyboardHit['4'] = false;
		}
	}
	if(bKeyboardPress['5'])
	{
		if(bKeyboardHit['5'])
		{
			wWorld.UnLoadChunk(0, 0);	
			bKeyboardHit['5'] = false;
		}
	}
	if(bKeyboardPress['6'])
	{
		if(bKeyboardHit['6'])
		{
			for(int i = 0; i < 8; i++)
				for(int j = 0; j < 8; j++)
					wWorld.LoadChunk(i, j);
			bKeyboardHit['6'] = false;
		}
	}
	if(bKeyboardPress['7'])
	{
		if(bKeyboardHit['7'])
		{
			for(int i = 0; i < 8; i++)
				for(int j = 0; j < 8; j++)
					wWorld.UnLoadChunk(i, j);
			bKeyboardHit['7'] = false;
		}
	}
	if(bKeyboardPress['0'])
	{
		if(bKeyboardHit['0'])
		{
			static Param par = {1, 0, &wWorld};

			 _beginthread(LoadNGenerate, 0, &par);

			WaitForSingleObject(wWorld.parget2, INFINITE);
			ResetEvent(wWorld.parget2);

			par.x++;

			bKeyboardHit['0'] = false;
		}
	}

	if(bKeyboardPress['E']) 
	{
		wWorld.RemoveBlock(sCenterBlockCoordX,sCenterBlockCoordY,sCenterBlockCoordZ, true);
	}

	if(bKeyboardPress['Q']) 
	{
		signed short ix = sCenterBlockCoordX, iy = sCenterBlockCoordY, iz = sCenterBlockCoordZ;
		if((zerr < xerr)&&(zerr < yerr))
		{
			if(dPositionZ < dDispCenterCoordZ) iz = sCenterBlockCoordZ - 1;
			if(dPositionZ > dDispCenterCoordZ) iz = sCenterBlockCoordZ + 1;
		}
		if((xerr < zerr)&&(xerr < yerr))
		{
			if(dPositionX < dDispCenterCoordX) ix = sCenterBlockCoordX - 1;
			if(dPositionX > dDispCenterCoordX) ix = sCenterBlockCoordX + 1;
		}
		if((yerr < xerr)&&(yerr < zerr))
		{
			if(dPositionY < dDispCenterCoordY) iy = sCenterBlockCoordY - 1;
			if(dPositionY > dDispCenterCoordY) iy = sCenterBlockCoordY + 1;
		}

		wWorld.AddBlock(ix,iy,iz,MAT_STONE, true);
	}

	dPositionX += FrameInterval*dVelocityX;
	dPositionZ += FrameInterval*dVelocityZ;
	dPositionY += FrameInterval*dVelocityY;

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

// 	if(dPositionX >= LOCATION_SIZE_XZ*TILE_SIZE) { dPositionX -= LOCATION_SIZE_XZ*TILE_SIZE; lnwPositionX++;}
// 	if(dPositionX < 0) { dPositionX += LOCATION_SIZE_XZ*TILE_SIZE; lnwPositionX--;}
// 	if(dPositionZ >= LOCATION_SIZE_XZ*TILE_SIZE) { dPositionZ -= LOCATION_SIZE_XZ*TILE_SIZE; lnwPositionZ++;}
// 	if(dPositionZ < 0) { dPositionZ += LOCATION_SIZE_XZ*TILE_SIZE; lnwPositionZ--;}

}

void Character::GetCenterCoords(GLsizei width, GLsizei height)
{
	GLint    viewport[4];		// параметры viewport-a.
	GLdouble projection[16];	// матрица проекции.
	GLdouble modelview[16];		// видовая матрица.
	GLfloat vz;					// координаты курсора мыши в системе координат viewport-a.

	glGetIntegerv(GL_VIEWPORT,viewport);           // узнаём параметры viewport-a.
	glGetDoublev(GL_PROJECTION_MATRIX,projection); // узнаём матрицу проекции.
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);   // узнаём видовую матрицу.

	glReadPixels(width/2, height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
	gluUnProject((double) width/2,(double) height/2,(double) vz, modelview, projection, viewport, &dDispCenterCoordX, &dDispCenterCoordY, &dDispCenterCoordZ);
	
// 	dDispCenterCoordX -= lnwPositionX*LOCATION_SIZE_XZ*TILE_SIZE;
// 	dDispCenterCoordZ -= lnwPositionZ*LOCATION_SIZE_XZ*TILE_SIZE;
}
