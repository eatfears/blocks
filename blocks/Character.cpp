#include <math.h>
#include <process.h>

#include "Character.h"
#include "Blocks_Definitions.h"


#include "World.h"
#include "Mutex.h"
Mutex VisibleListAccessMutex;;

typedef struct params
{
	signed short x;
	signed short z;
	World *wWorld;
} Param;

Param par2 = {0, 0, 0};

void Thread( void* pParams )
{
	Param pParameters = *(Param*)pParams;
	signed short x = pParameters.x;
	signed short z = pParameters.z;
	World &wWorld = *pParameters.wWorld;

	SetEvent(wWorld.parget);

	HANDLE threadHandle = GetCurrentThread();
	SetThreadPriority(threadHandle, THREAD_PRIORITY_BELOW_NORMAL);

	DWORD dwWaitResult; 
	dwWaitResult = WaitForSingleObject(wWorld.mutex, INFINITE);
	auto loc = wWorld.AddLocation(x,z);
	if(!loc) {_endthread(); return;}
	ReleaseMutex(wWorld.mutex);

// 
// 	auto loc = wWorld.lLocations.begin();
// 	while (loc != wWorld.lLocations.end())
// 	{
// 		if(loc->x == x && loc->z == z)
// 			break;
// 		++loc;
// 	}
	//VisibleListAccessMutex.Acquire();

	dwWaitResult = WaitForSingleObject(loc->mutex, INFINITE);

	for(int j = 0; j < 100; j++)
	{
		for(int i = 0; i < 16; i++)
		{
			for(int k = 0; k < 16; k++)
			{
				//if(rand()%100) wWorld.AddTile(i-8 + 16*x, -j, k-8 + 16*z, MAT_GRASS, false);
				//wWorld.AddTile(i + 16*x, j, k + 16*z, rand()%4+1, false);
				
				//if(rand()%100) wWorld.AddTile(i + 16*x, j, k + 16*z, MAT_GRASS, false);
				if(rand()%500) wWorld.AddTile(i + 16*x, j, k + 16*z, rand()%4+1, false);
				//if(rand()%500) wWorld.AddTile(i + 16*x, j, k + 16*z, MAT_STONE, false);
			}
		}
	}
	wWorld.DrawLoadedTiles(&*loc);
	
	//VisibleListAccessMutex.Release();
	ReleaseMutex(loc->mutex);

	_endthread();
}
// 
// void Thread2( void* pParams )
// {
// 	m2.Acquire();
// 	Param pParameters = *(Param*)pParams;
// 	signed short x = pParameters.x;
// 	signed short z = pParameters.z;
// 	m2.Release();
// 	World &wWorld = *pParameters.wWorld;
// 	
// 	HANDLE threadHandle = GetCurrentThread();
// 	SetThreadPriority(threadHandle, THREAD_PRIORITY_ABOVE_NORMAL);
// 
// 	//VisibleListAccessMutex.Acquire();
// 	/*
// 	for(int N = 0; N < 6; N++)
// 		for(int i = 0; i < 16; i++)
// 			for(int k = 0; k < 16; k++)
// 				for(int j = 1; j <= 70; j++)
// 					wWorld.HideTile(i-8 + 16*x, -j, k-8 + 16*z, N);
// 	/**/
// 	for(int j = 1; j <= 70; j++)
// 		for(int i = 0; i < 16; i++)
// 			for(int k = 0; k < 16; k++)
// 				wWorld.RemoveTile(i-8 + 16*x, -j, k-8 + 16*z, false);
// 
// 	//VisibleListAccessMutex.Release();
// 	
// 	wWorld.DrawLoadedTiles();
// }

Character::Character()
{
	bFalling = true;
	for(int i = 0; i < 256; i++) bKeyboardDown[i] = true;


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
	*xerr = dDispCenterCoordX + TILE_SIZE/2;
	*yerr = dDispCenterCoordY;
	*zerr = dDispCenterCoordZ + TILE_SIZE/2;

	while(*yerr < -1) *yerr += TILE_SIZE;
	while(*yerr > TILE_SIZE + 1) *yerr -= TILE_SIZE;

	*yerr = abs(*yerr);
	if(*yerr > abs(*yerr - TILE_SIZE)) *yerr = abs(*yerr - TILE_SIZE); 

	while(*xerr < - 1) *xerr += TILE_SIZE;
	while(*xerr > TILE_SIZE + 1) *xerr -= TILE_SIZE;

	*xerr = abs(*xerr);
	if(*xerr > abs(*xerr - TILE_SIZE)) *xerr = abs(*xerr - TILE_SIZE); 

	while(*zerr < - 1) *zerr += TILE_SIZE;
	while(*zerr > TILE_SIZE + 1) *zerr -= TILE_SIZE;

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
			if(wWorld.AddTile(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, rand()%4+1, true))
				i++;
		}
	}
	if(bKeyboard['2'])
	{
		int i = 0;

		while(i < num)
		{
			if(wWorld.RemoveTile(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2, true))
				i++;
		}
	}
 
	if(bKeyboard['3'])
	{
		for(int i = -sqb2; i <= sqb2; i++)
		for(int j = -sqb2; j <= sqb2; j++)
		for(int k = -sqb2; k <= sqb2; k++)
		{
			wWorld.RemoveTile(i, j, k, true);	
		}
	}

	if(bKeyboard['4'])
	{
		if(bKeyboardDown['4'])
		{
			static Param par = {0, 0, &wWorld};

			HANDLE hThread;
			
			for(int i = 0; i < 1; i++)
			{
				hThread = (HANDLE) _beginthread( Thread, 0, &par);//&Param(par) );

				//WaitForSingleObject(hThread, 30);
				WaitForSingleObject(wWorld.parget, INFINITE);
				ResetEvent(wWorld.parget);

				par.x++;
				if(par.x == 10)
				{
					par.x = 0;
					par.z++;
				}
			}
			//bKeyboardDown['4'] = false;
		}
	}
// 	if(bKeyboard['5'])
// 	{
// 		if(bKeyboardDown['5'])
// 		{
// 			par2.wWorld = &wWorld;
// 
// 			HANDLE hThread;
// 			hThread = (HANDLE) _beginthread( Thread2, 0, &par2 );
// 
// 			WaitForMultipleObjects(1, &hThread, TRUE, 30);
// 
// 			par2.x++;
// 			if(par2.x == 10)
// 			{
// 				par2.x = 0;
// 				par2.z++;
// 			}
// 			//bKeyboardDown['5'] = false;
// 		}
// 	}
	if(bKeyboard['6'])
	{
		if(bKeyboardDown['6'])
		{
			LocInWorld x = 0, z = 0;
			auto loc = wWorld.AddLocation(x,z);

			if(loc)
			{
				for(int k = 0; k < 16; k++)
				{
					for(int i = 0; i < 16; i++)
					{
						for(int j = 0; j < 1; j++)
						{
							//if(rand()%100) wWorld.AddTile(i-8 + 16*x, -j, k-8 + 16*z, MAT_GRASS, false);
							wWorld.AddTile(i + 16*x, j, k + 16*z, rand()%4+1, false);
						}
					}
				}

				wWorld.DrawLoadedTiles(&*loc);
			}
			bKeyboardDown['6'] = false;
		}
	}

	if(bKeyboard['E']) 
	{
		wWorld.RemoveTile(sCenterCubeCoordX,sCenterCubeCoordY,sCenterCubeCoordZ, true);
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

		wWorld.AddTile(ix,iy,iz,MAT_DIRT, true);
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
