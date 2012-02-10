#include "Character.h"
#include <math.h>
#include <process.h>
#include "Threads.h"
#include "World.h"
#include "Primes.h"

Character::Character(World& ww)
	: wWorld(ww)
{
	bFalling = true;
	for(int i = 0; i < 256; i++) 
	{
		bKeyboard[i] = false;
		bSpecial[i] = false;
	}
	dPositionX = 0;
	dPositionY = 0;
	dPositionZ = 0;

	dVelocityX = 0;
	dVelocityY = 0;
	dVelocityZ = 0;

	dSpinX = 0;
	dSpinY = 0;

	sCenterBlockCoordX = 0;
	sCenterBlockCoordY = 0;
	sCenterBlockCoordZ = 0;

	UnderWater = false;
}

Character::~Character()
{
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
	if(bSpecial[GLUT_KEY_SHIFT_L]) 
		step *= SPRINT_KOEF;

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
	if(bKeyboard['R']) 
	{
		dVelocityY += FrameInterval*AIR_ACCEL*step;
	}
	if(bKeyboard['F']) 
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

	if(bKeyboard[VK_SPACE])
	{
		if(!bFalling)
		{
			dVelocityY = -JUMP_STR;
			bFalling = true;
			dPositionY += FrameInterval;
		}
	}

	if(bKeyboard['X'])
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

		if(dPositionZ < dDispCenterCoordZ) sCenterBlockCoordZ = (BlockInWorld) Primes::Round(dDispCenterCoordZ/BLOCK_SIZE + 0.5);
		if(dPositionZ > dDispCenterCoordZ) sCenterBlockCoordZ = (BlockInWorld) Primes::Round(dDispCenterCoordZ/BLOCK_SIZE - 0.5);
	}
	if((xerr < zerr)&&(xerr < yerr))
	{
		sCenterBlockCoordZ = (BlockInWorld) floor(dDispCenterCoordZ/BLOCK_SIZE + 0.5);
		sCenterBlockCoordY = (BlockInWorld) floor(dDispCenterCoordY/BLOCK_SIZE);

		if(dPositionX < dDispCenterCoordX) sCenterBlockCoordX = (BlockInWorld) Primes::Round(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		if(dPositionX > dDispCenterCoordX) sCenterBlockCoordX = (BlockInWorld) Primes::Round(dDispCenterCoordX/BLOCK_SIZE - 0.5);
	}
	if((yerr < xerr)&&(yerr < zerr))
	{
		sCenterBlockCoordX = (BlockInWorld) floor(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		sCenterBlockCoordZ = (BlockInWorld) floor(dDispCenterCoordZ/BLOCK_SIZE + 0.5);

		if(dPositionY < dDispCenterCoordY) sCenterBlockCoordY = (BlockInWorld) Primes::Round(dDispCenterCoordY/BLOCK_SIZE);
		if(dPositionY > dDispCenterCoordY) sCenterBlockCoordY = (BlockInWorld) Primes::Round(dDispCenterCoordY/BLOCK_SIZE - 1.0);
	}

	int num = 100;
	int sq = 100;
	int sqb2 = sq/2;

	if(bKeyboard['1'])
	{
		int i = 0;
		
		while(i < num)
		{
			if(wWorld.AddBlock(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, rand()%14+1, true))
				i++;
		}
	}
	if(bKeyboard['2'])
	{
		int i = 0;

		while(i < num)
		{
			if(wWorld.RemoveBlock(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, true))
				i++;
		}
	}
 
	if(bKeyboard['3'])
	{
		for(int i = -sqb2; i <= sqb2; i++)
		for(int j = -sqb2; j <= sqb2; j++)
		for(int k = -sqb2; k <= sqb2; k++)
		{
			wWorld.RemoveBlock(i, j, k, true);	
		}
	}

	if(bKeyboard['4'])
	{
		wWorld.LoadChunk(0, 0);	
		bKeyboard['4'] = false;
	}
	if(bKeyboard['5'])
	{
		wWorld.UnLoadChunk(0, 0);	
		bKeyboard['5'] = false;
	}
	if(bKeyboard['6'])
	{
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 8; j++)
				wWorld.LoadChunk(i, j);
		bKeyboard['6'] = false;
	}
	if(bKeyboard['7'])
	{
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 8; j++)
				wWorld.UnLoadChunk(i, j);
		bKeyboard['7'] = false;
	}
	if(bKeyboard['0'])
	{
		static Param par = {1, 0, &wWorld};

			_beginthread(LoadNGenerate, 0, &par);

		WaitForSingleObject(wWorld.parget2, INFINITE);
		ResetEvent(wWorld.parget2);

		par.x++;

		bKeyboard['0'] = false;
	}
	if(bKeyboard['C']) 
	{
		Chunk *chunk;
		int index;
		wWorld.FindBlock(sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ, &chunk, &index);
		if ((chunk)&&(chunk->bBlocks[index].cMaterial == MAT_DIRT))
		{
			chunk->NeedToRender[0] = true;
			chunk->bBlocks[index].bVisible ^= (1 << SNOWCOVERED);
		}
	}
	if(bKeyboard['V']) 
	{
		Chunk *chunk;
		int index;
		wWorld.FindBlock(sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ, &chunk, &index);
		if ((chunk)&&(chunk->bBlocks[index].cMaterial == MAT_DIRT))
		{
			chunk->NeedToRender[0] = true;
			chunk->bBlocks[index].bVisible ^= (1 << GRASSCOVERED);
		}
	}
	if(bKeyboard['E']) 
	{
		wWorld.RemoveBlock(sCenterBlockCoordX,sCenterBlockCoordY,sCenterBlockCoordZ, true);
	}

	if(bKeyboard['Q']) 
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

void Character::GetMyPosition()
{
	BlockInWorld x, y, z;
	x = (BlockInWorld) Primes::Round(dPositionX/BLOCK_SIZE);
	y = (BlockInWorld) Primes::Round(dPositionY/BLOCK_SIZE - 0.5);
	z = (BlockInWorld) Primes::Round(dPositionZ/BLOCK_SIZE);

	wWorld.FindBlock(x, y, z, &chunk, &index);
	if((chunk)&&(chunk->bBlocks[index].cMaterial == MAT_WATER))
		UnderWater = true;
	else
		UnderWater = false;
}
