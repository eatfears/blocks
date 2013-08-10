#include "Character.h"
#include <math.h>
#include <process.h>
#include "Threads.h"
#include "World.h"
#include "Primes.h"

Character::Character(World& ww)
	: wWorld(ww)
{
	for(int i = 0; i < 256; i++) {
		bKeyboard[i] = false;
		bSpecial[i] = false;
	}

	dVelocityX = 0;
	dVelocityY = 0;
	dVelocityZ = 0;

	dSpinX = 0;
	dSpinY = 0;

	sCenterBlockCoord.cx = 0;
	sCenterBlockCoord.cz = 0;
	sCenterBlockCoord.bx = 0;
	sCenterBlockCoord.by = 0;
	sCenterBlockCoord.bz = 0;
	LocalTimeOfDay = 0;
	LocalTimeOfWinal = 0;
		
	Longitude = 0;
	Longitude2 = 0;

	underWater = false;
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

	if(bKeyboard['W']) {
		dVelocityX -= FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
		dVelocityZ -= FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
	}
	if(bKeyboard['S']) {
		dVelocityX += FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
		dVelocityZ += FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
	}
	if(bKeyboard['D']) {
		dVelocityX += FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
		dVelocityZ -= FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
	}
	if(bKeyboard['A']) {
		dVelocityX -= FrameInterval*AIR_ACCEL*step*cos(TORAD(dSpinY));
		dVelocityZ += FrameInterval*AIR_ACCEL*step*sin(TORAD(dSpinY));
	}
	if(bKeyboard['R']) {
		dVelocityY += FrameInterval*AIR_ACCEL*step;
	}
	if(bKeyboard['F']) {
		dVelocityY -= FrameInterval*AIR_ACCEL*step;
	}

	GLdouble ko = dVelocityX*dVelocityX + dVelocityZ*dVelocityZ;
	if(ko > WALK_SPEED*WALK_SPEED*SPRINT_KOEF*SPRINT_KOEF) {
		ko = pow(ko, 0.5);
		dVelocityX = dVelocityX*WALK_SPEED*SPRINT_KOEF/ko;
		dVelocityZ = dVelocityZ*WALK_SPEED*SPRINT_KOEF/ko;
	}

	if(bKeyboard[VK_SPACE]) {
	}
	

	if(bKeyboard['X']) {
		dVelocityX = 0;
		dVelocityY = 0;
		dVelocityZ = 0;
	}
	/*
	GLdouble yerr, xerr, zerr;
	GetPlane(&xerr, &yerr, &zerr);

	if((zerr < xerr)&&(zerr < yerr)) {
		BlockInWorld pos;
		sCenterBlockCoordX = (BlockInWorld) floor(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		sCenterBlockCoordY = (BlockInWorld) floor(dDispCenterCoordY/BLOCK_SIZE);

		if(dPositionZ < dDispCenterCoordZ) sCenterBlockCoordZ = (BlockInWorld) Primes::Round(dDispCenterCoordZ/BLOCK_SIZE + 0.5);
		if(dPositionZ > dDispCenterCoordZ) sCenterBlockCoordZ = (BlockInWorld) Primes::Round(dDispCenterCoordZ/BLOCK_SIZE - 0.5);
	}
	if((xerr < zerr)&&(xerr < yerr)) {
		sCenterBlockCoordZ = (BlockInWorld) floor(dDispCenterCoordZ/BLOCK_SIZE + 0.5);
		sCenterBlockCoordY = (BlockInWorld) floor(dDispCenterCoordY/BLOCK_SIZE);

		if(dPositionX < dDispCenterCoordX) sCenterBlockCoordX = (BlockInWorld) Primes::Round(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		if(dPositionX > dDispCenterCoordX) sCenterBlockCoordX = (BlockInWorld) Primes::Round(dDispCenterCoordX/BLOCK_SIZE - 0.5);
	}
	if((yerr < xerr)&&(yerr < zerr)) {
		sCenterBlockCoordX = (BlockInWorld) floor(dDispCenterCoordX/BLOCK_SIZE + 0.5);
		sCenterBlockCoordZ = (BlockInWorld) floor(dDispCenterCoordZ/BLOCK_SIZE + 0.5);

		if(dPositionY < dDispCenterCoordY) sCenterBlockCoordY = (BlockInWorld) Primes::Round(dDispCenterCoordY/BLOCK_SIZE);
		if(dPositionY > dDispCenterCoordY) sCenterBlockCoordY = (BlockInWorld) Primes::Round(dDispCenterCoordY/BLOCK_SIZE - 1.0);
	}
	*/

	int num = 100;
	int sq = 100;
	int sqb2 = sq/2;

	if(bKeyboard['1']) {
		int i = 0;
		while(i < num) {
			if(wWorld.AddBlock(BlockInWorld(rand()%sq-sqb2, abs(rand()%sq-sqb2), rand()%sq-sqb2), rand()%14+1, true))
				i++;
		}
	}
	if(bKeyboard['2']) {
		int i = 0;
		while(i < num) {
			if(wWorld.RemoveBlock(BlockInWorld(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2), true))
				i++;
		}
	}

	if(bKeyboard['3']) {
		for(BlockCoord i = -sqb2; i <= sqb2; i++) {
			for(BlockCoord j = -sqb2; j <= sqb2; j++) {
				for(BlockCoord k = -sqb2; k <= sqb2; k++) {
					wWorld.RemoveBlock(BlockInWorld(i, j, k), true);
				}
			}
		}
	}

	if(bKeyboard['4']) {
		wWorld.LoadChunk(0, 0);
		bKeyboard['4'] = false;
	}
	if(bKeyboard['5']) {
		wWorld.UnLoadChunk(0, 0);
		bKeyboard['5'] = false;
	}
	if(bKeyboard['6']) {
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 8; j++)
				wWorld.LoadChunk(i, j);
		bKeyboard['6'] = false;
	}
	if(bKeyboard['7']) {
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 8; j++)
				wWorld.UnLoadChunk(i, j);
		bKeyboard['7'] = false;
	}

	if(bKeyboard['0']) {
		static Param par = {0, 1, &wWorld};

		_beginthread(LoadNGenerate, 0, &par);

		WaitForSingleObject(wWorld.parget2, INFINITE);
		ResetEvent(wWorld.parget2);

		par.z++;
		bKeyboard['0'] = false;
	}
	/*
	if(bKeyboard['C']) {
		Chunk *chunk;
		int index;
		wWorld.FindBlock(sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ, &chunk, &index);
		if ((chunk)&&(chunk->bBlocks[index].cMaterial == MAT_DIRT)) {
			chunk->bBlocks[index].bVisible ^= (1 << SNOWCOVERED);
		}
	}
	if(bKeyboard['V']) {
		Chunk *chunk;
		int index;
		wWorld.FindBlock(sCenterBlockCoordX, sCenterBlockCoordY, sCenterBlockCoordZ, &chunk, &index);
		if ((chunk)&&(chunk->bBlocks[index].cMaterial == MAT_DIRT)) {
			chunk->bBlocks[index].bVisible ^= (1 << GRASSCOVERED);
		}
	}
	if(bKeyboard['E']) {
		wWorld.RemoveBlock(sCenterBlockCoordX,sCenterBlockCoordY,sCenterBlockCoordZ, true);
	}

	if(bKeyboard['Q']) {
		signed short ix = sCenterBlockCoordX, iy = sCenterBlockCoordY, iz = sCenterBlockCoordZ;
		if((zerr < xerr)&&(zerr < yerr)) {
			if(dPositionZ < dDispCenterCoordZ) iz = sCenterBlockCoordZ - 1;
			if(dPositionZ > dDispCenterCoordZ) iz = sCenterBlockCoordZ + 1;
		}
		if((xerr < zerr)&&(xerr < yerr)) {
			if(dPositionX < dDispCenterCoordX) ix = sCenterBlockCoordX - 1;
			if(dPositionX > dDispCenterCoordX) ix = sCenterBlockCoordX + 1;
		}
		if((yerr < xerr)&&(yerr < zerr)) {
			if(dPositionY < dDispCenterCoordY) iy = sCenterBlockCoordY - 1;
			if(dPositionY > dDispCenterCoordY) iy = sCenterBlockCoordY + 1;
		}

		wWorld.AddBlock(ix, iy, iz, MAT_PUMPKIN_SHINE, true);
	}
	*/
	if(bKeyboard['O']) {
		wWorld.SaveChunks();
	}

	position = position + PosInWorld(FrameInterval*dVelocityX, FrameInterval*dVelocityY, FrameInterval*dVelocityZ);

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
	GLint    viewport[4];		// ��������� viewport-a.
	GLdouble projection[16];	// ������� ��������.
	GLdouble modelview[16];		// ������� �������.
	GLfloat vz;					// ���������� ������� ���� � ������� ��������� viewport-a.

	glGetIntegerv(GL_VIEWPORT,viewport);           // ����� ��������� viewport-a.
	glGetDoublev(GL_PROJECTION_MATRIX,projection); // ����� ������� ��������.
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);   // ����� ������� �������.

	glReadPixels(width/2, height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
	gluUnProject((double) width/2,(double) height/2,(double) vz, modelview, projection, viewport, &dDispCenterCoordX, &dDispCenterCoordY, &dDispCenterCoordZ);
}

void Character::GetLocalTime(double TimeOfDay, double TimeOfWinal)
{
	LocalTimeOfDay = TimeOfDay + Longitude*DAY_TIME;
	LocalTimeOfWinal = TimeOfWinal + Longitude2*WINAL_TIME;

	while (LocalTimeOfDay >= DAY_TIME) LocalTimeOfDay -= DAY_TIME;
	while (LocalTimeOfDay < 0.0) LocalTimeOfDay += DAY_TIME;

	while (LocalTimeOfWinal >= WINAL_TIME) LocalTimeOfWinal -= WINAL_TIME;
	while (LocalTimeOfWinal < 0.0) LocalTimeOfWinal += WINAL_TIME;
}

void Character::GetMyPosition()
{
	// checks if player is under water level
	PosInWorld pos(position);
	pos.by += 0.125 - 0.5;
	BlockInWorld waterPos(pos);

	wWorld.FindBlock(waterPos, &chunk, &index);
	underWater = chunk && chunk->bBlocks[index].cMaterial == MAT_WATER;

	Longitude = (position.bz/BLOCK_SIZE+position.cz*CHUNK_SIZE_XZ)/(BLOCK_SIZE*CHUNK_SIZE_XZ*160);
	Longitude2 = -(position.bx/BLOCK_SIZE+position.cx*CHUNK_SIZE_XZ)/(BLOCK_SIZE*CHUNK_SIZE_XZ*160);
}
