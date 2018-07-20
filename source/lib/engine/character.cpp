#include <math.h>

#include "character.h"
#include "threads.h"
#include "world.h"
#include "noise/primes.h"


Character::Character(World& ww)
    : wWorld(ww)
{
    for(int i = 0; i < 256; i++)
    {
        bKeyboard[i] = false;
        bSpecial[i] = false;
    }

    dVelocityX = 0;
    dVelocityY = 0;
    dVelocityZ = 0;

    dSpinX = 0;
    dSpinY = 0;

    aimedBlock.cx = 0;
    aimedBlock.cz = 0;
    aimedBlock.bx = 0;
    aimedBlock.by = 0;
    aimedBlock.bz = 0;
    LocalTimeOfDay = 0;

    Longitude = 0;

    underWater = false;
}

void Character::getPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr)
{
    *xerr = centerPos.bx + 0.5;
    *yerr = centerPos.by;
    *zerr = centerPos.bz + 0.5;

    // todo: bad resolution
    while(*yerr < 0) *yerr += 1;
    while(*yerr > 1) *yerr -= 1;

    if(*yerr > abs(*yerr - 1)) *yerr = abs(*yerr - 1);

    while(*xerr < 0) *xerr += 1;
    while(*xerr > 1) *xerr -= 1;

    if(*xerr > abs(*xerr - 1)) *xerr = abs(*xerr - 1);

    while(*zerr < 0) *zerr += 1;
    while(*zerr > 1) *zerr -= 1;

    if(*zerr > abs(*zerr - 1)) *zerr = abs(*zerr - 1);
}

void Character::control(GLdouble FrameInterval)
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

    //	if(bKeyboard[VK_SPACE]) {
    //	}

    if(bKeyboard['X'])
    {
        dVelocityX = 0;
        dVelocityY = 0;
        dVelocityZ = 0;
    }

    GLdouble yerr, xerr, zerr;
    getPlane(&xerr, &yerr, &zerr);

    PointInWorld pos;
    if(zerr < xerr && zerr < yerr)
    {
        if((position.bz < centerPos.bz && position.cz == centerPos.cz) || position.cz < centerPos.cz) {
            pos = PointInWorld(0, 0, 0.5);
        } else {
            pos = PointInWorld(0, 0, -0.5);
        }
    } else if(xerr < zerr && xerr < yerr) {
        if((position.bx < centerPos.bx && position.cx == centerPos.cx) || position.cx < centerPos.cx) {
            pos = PointInWorld(0.5, 0, 0);
        } else {
            pos = PointInWorld(-0.5, 0, 0);
        }
    } else if(yerr < xerr && yerr < zerr) {
        if(position.by < centerPos.by) {
            pos = PointInWorld(0, 0.5, 0);
        } else {
            pos = PointInWorld(0, -0.5, 0);
        }
    }
    aimedBlock = BlockInWorld(centerPos + pos);
    freeBlock = BlockInWorld(centerPos + pos.inv());

    int num = 100;
    int sq = 100;
    int sqb2 = sq/2;

    if(bKeyboard['1'])
    {
        int i = 0;
        while(i < num)
        {
            if(wWorld.addBlock(BlockInWorld(rand()%sq-sqb2, abs(rand()%sq-sqb2), rand()%sq-sqb2), rand()%14+1, true))
                i++;
        }
    }
    if(bKeyboard['2']) {
        int i = 0;
        while(i < num) {
            if(wWorld.removeBlock(BlockInWorld(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2), true))
                i++;
        }
    }

    if(bKeyboard['3']) {
        for(BlockCoord i = -sqb2; i <= sqb2; i++) {
            for(BlockCoord j = -sqb2; j <= sqb2; j++) {
                for(BlockCoord k = -sqb2; k <= sqb2; k++) {
                    wWorld.removeBlock(BlockInWorld(i, j, k), true);
                }
            }
        }
    }

    if(bKeyboard['4']) {
        wWorld.loadChunk(0, 0);
        bKeyboard['4'] = false;
    }
    if(bKeyboard['5']) {
        wWorld.unLoadChunk(0, 0);
        bKeyboard['5'] = false;
    }
    if(bKeyboard['6']) {
        for(int i = 0; i < 8; i++)
            for(int j = 0; j < 8; j++)
                wWorld.loadChunk(i, j);
        bKeyboard['6'] = false;
    }
    if(bKeyboard['7']) {
        for(int i = 0; i < 8; i++)
            for(int j = 0; j < 8; j++)
                wWorld.unLoadChunk(i, j);
        bKeyboard['7'] = false;
    }

    //	if(bKeyboard['0']) {
    //		static Param par = {0, 1, &wWorld};

    //		_beginthread(LoadNGenerate, 0, &par);

    //		WaitForSingleObject(wWorld.parget2, INFINITE);
    //		ResetEvent(wWorld.parget2);

    //		par.z++;
    //		bKeyboard['0'] = false;
    //	}

    if(bKeyboard['C']) {
        Chunk *chunk;
        int index;
        wWorld.findBlock(aimedBlock, &chunk, &index);
        if ((chunk)&&(chunk->bBlocks[index].cMaterial == MAT_DIRT)) {
            chunk->bBlocks[index].bVisible ^= (1 << SNOWCOVERED);
        }
    }
    if(bKeyboard['V']) {
        Chunk *chunk;
        int index;
        wWorld.findBlock(aimedBlock, &chunk, &index);
        if ((chunk)&&(chunk->bBlocks[index].cMaterial == MAT_DIRT)) {
            chunk->bBlocks[index].bVisible ^= (1 << GRASSCOVERED);
        }
    }
    if(bKeyboard['E']) {
        wWorld.removeBlock(aimedBlock, true);
    }
    if(bKeyboard['Q']) {
        wWorld.addBlock(freeBlock, MAT_PUMPKIN_SHINE, true);
    }
    if(bKeyboard['O']) {
        wWorld.saveChunks();
    }

    position = position + PointInWorld(FrameInterval*dVelocityX, FrameInterval*dVelocityY, FrameInterval*dVelocityZ);

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

void Character::getCenterCoords(GLsizei width, GLsizei height)
{
    GLint    viewport[4];		// параметры viewport-a.
    GLdouble projection[16];	// матрица проекции.
    GLdouble modelview[16];		// видовая матрица.
    GLfloat vz;					// координаты курсора мыши в системе координат viewport-a.
    GLdouble dispCenterX, dispCenterY, dispCenterZ;			// возвращаемые мировые координаты центра

    glGetIntegerv(GL_VIEWPORT,viewport);           // узнаём параметры viewport-a.
    glGetDoublev(GL_PROJECTION_MATRIX,projection); // узнаём матрицу проекции.
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);   // узнаём видовую матрицу.

    glReadPixels(width/2, height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
    gluUnProject((double) width/2,(double) height/2,(double) vz, modelview, projection, viewport, &dispCenterX, &dispCenterY, &dispCenterZ);
    centerPos = position;

    centerPos.bx = 0;
    centerPos.by = 0;
    centerPos.bz = 0;

    centerPos = centerPos + PointInWorld(dispCenterX, dispCenterY, dispCenterZ);

}

void Character::getLocalTime(double TimeOfDay)
{
    LocalTimeOfDay = TimeOfDay + Longitude*DAY_TIME;

    while (LocalTimeOfDay >= DAY_TIME) LocalTimeOfDay -= DAY_TIME;
    while (LocalTimeOfDay < 0.0) LocalTimeOfDay += DAY_TIME;
}

void Character::getMyPosition()
{
    // checks if player is under water level
    PointInWorld pos(position);
    pos.by += 0.125 - 0.5;
    BlockInWorld waterPos(pos);

    wWorld.findBlock(waterPos, &chunk, &index);
    underWater = chunk && chunk->bBlocks[index].cMaterial == MAT_WATER;

    Longitude = (position.bz/CHUNK_SIZE_XZ + position.cz)/160;
}
