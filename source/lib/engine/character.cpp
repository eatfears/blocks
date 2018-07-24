#include <math.h>

#include "character.h"
#include "world.h"
#include "noise/primes.h"


Character::Character(World &world)
    : m_World(world)
{
    for (int i = 0; i < 256; i++)
    {
        m_Keyboard[i] = false;
        m_SpecialKeys[i] = false;
    }

    m_VelocityX = 0;
    m_VelocityY = 0;
    m_VelocityZ = 0;

    m_SpinX = 0;
    m_SpinY = 0;

    m_AimedBlock.cx = 0;
    m_AimedBlock.cz = 0;
    m_AimedBlock.bx = 0;
    m_AimedBlock.by = 0;
    m_AimedBlock.bz = 0;
    m_LocalTimeOfDay = 0;

    m_Longitude = 0;

    m_UnderWater = false;
}

void Character::getPlane(GLdouble *xerr,GLdouble *yerr,GLdouble *zerr) const
{
    *xerr = m_CenterPos.bx + 0.5;
    *yerr = m_CenterPos.by;
    *zerr = m_CenterPos.bz + 0.5;

    // todo: bad resolution
    while (*yerr < 0) *yerr += 1;
    while (*yerr > 1) *yerr -= 1;

    if (*yerr > abs(*yerr - 1)) *yerr = abs(*yerr - 1);

    while (*xerr < 0) *xerr += 1;
    while (*xerr > 1) *xerr -= 1;

    if (*xerr > abs(*xerr - 1)) *xerr = abs(*xerr - 1);

    while (*zerr < 0) *zerr += 1;
    while (*zerr > 1) *zerr -= 1;

    if (*zerr > abs(*zerr - 1)) *zerr = abs(*zerr - 1);
}

void Character::control(GLdouble frame_interval)
{
    GLdouble step = WALK_SPEED;
    if (m_SpecialKeys[GLUT_KEY_SHIFT_L])
        step *= SPRINT_KOEF;

    if (m_Keyboard['W']) {
        m_VelocityX -= frame_interval*AIR_ACCEL*step*sin(TORAD(m_SpinY));
        m_VelocityZ -= frame_interval*AIR_ACCEL*step*cos(TORAD(m_SpinY));
    }
    if (m_Keyboard['S']) {
        m_VelocityX += frame_interval*AIR_ACCEL*step*sin(TORAD(m_SpinY));
        m_VelocityZ += frame_interval*AIR_ACCEL*step*cos(TORAD(m_SpinY));
    }
    if (m_Keyboard['D']) {
        m_VelocityX += frame_interval*AIR_ACCEL*step*cos(TORAD(m_SpinY));
        m_VelocityZ -= frame_interval*AIR_ACCEL*step*sin(TORAD(m_SpinY));
    }
    if (m_Keyboard['A']) {
        m_VelocityX -= frame_interval*AIR_ACCEL*step*cos(TORAD(m_SpinY));
        m_VelocityZ += frame_interval*AIR_ACCEL*step*sin(TORAD(m_SpinY));
    }
    if (m_Keyboard['R']) {
        m_VelocityY += frame_interval*AIR_ACCEL*step;
    }
    if (m_Keyboard['F']) {
        m_VelocityY -= frame_interval*AIR_ACCEL*step;
    }

    GLdouble ko = m_VelocityX*m_VelocityX + m_VelocityZ*m_VelocityZ;
    if (ko > WALK_SPEED*WALK_SPEED*SPRINT_KOEF*SPRINT_KOEF) {
        ko = pow(ko, 0.5);
        m_VelocityX = m_VelocityX*WALK_SPEED*SPRINT_KOEF/ko;
        m_VelocityZ = m_VelocityZ*WALK_SPEED*SPRINT_KOEF/ko;
    }

    //	if (bKeyboard[VK_SPACE]) {
    //	}

    if (m_Keyboard['X'])
    {
        m_VelocityX = 0;
        m_VelocityY = 0;
        m_VelocityZ = 0;
    }

    GLdouble yerr, xerr, zerr;
    getPlane(&xerr, &yerr, &zerr);

    PointInWorld pos;
    if (zerr < xerr && zerr < yerr)
    {
        if ((m_Position.bz < m_CenterPos.bz && m_Position.cz == m_CenterPos.cz) || m_Position.cz < m_CenterPos.cz) {
            pos = PointInWorld(0, 0, 0.5);
        } else {
            pos = PointInWorld(0, 0, -0.5);
        }
    } else if (xerr < zerr && xerr < yerr) {
        if ((m_Position.bx < m_CenterPos.bx && m_Position.cx == m_CenterPos.cx) || m_Position.cx < m_CenterPos.cx) {
            pos = PointInWorld(0.5, 0, 0);
        } else {
            pos = PointInWorld(-0.5, 0, 0);
        }
    } else if (yerr < xerr && yerr < zerr) {
        if (m_Position.by < m_CenterPos.by) {
            pos = PointInWorld(0, 0.5, 0);
        } else {
            pos = PointInWorld(0, -0.5, 0);
        }
    }
    m_AimedBlock = BlockInWorld(m_CenterPos + pos);
    m_FreeBlock = BlockInWorld(m_CenterPos + pos.inv());

    int num = 100;
    int sq = 100;
    int sqb2 = sq/2;

    if (m_Keyboard['1'])
    {
        int i = 0;
        while (i < num)
        {
            if (m_World.addBlock(BlockInWorld(rand()%sq-sqb2, abs(rand()%sq-sqb2), rand()%sq-sqb2), rand()%14+1))
                i++;
        }
    }
    if (m_Keyboard['2']) {
        int i = 0;
        while (i < num) {
            if (m_World.removeBlock(BlockInWorld(rand()%sq-sqb2, rand()%sq-sqb2, rand()%sq-sqb2)))
                i++;
        }
    }

    if (m_Keyboard['3']) {
        for (BlockCoord i = -sqb2; i <= sqb2; i++) {
            for (BlockCoord j = -sqb2; j <= sqb2; j++) {
                for (BlockCoord k = -sqb2; k <= sqb2; k++) {
                    m_World.removeBlock(BlockInWorld(i, j, k));
                }
            }
        }
    }

    if (m_Keyboard['4']) {
        m_World.loadChunk(0, 0);
        m_Keyboard['4'] = false;
    }
    if (m_Keyboard['5']) {
        m_World.unLoadChunk(0, 0);
        m_Keyboard['5'] = false;
    }
    if (m_Keyboard['6']) {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                m_World.loadChunk(i, j);
        m_Keyboard['6'] = false;
    }
    if (m_Keyboard['7']) {
        for (int i = 0; i < 1; i++)
            for (int j = 0; j < 1; j++)
                m_World.unLoadChunk(i, j);
        m_Keyboard['7'] = false;
    }

    //	if (bKeyboard['0']) {
    //		static Param par = {0, 1, &wWorld};

    //		_beginthread(LoadNGenerate, 0, &par);

    //		WaitForSingleObject(wWorld.parget2, INFINITE);
    //		ResetEvent(wWorld.parget2);

    //		par.z++;
    //		bKeyboard['0'] = false;
    //	}

    if (m_Keyboard['C']) {
        Chunk *chunk;
        unsigned int index;
        m_World.findBlock(m_AimedBlock, chunk, index);
        if ((chunk)&&(chunk->m_pBlocks[index].material == MAT_DIRT)) {
            chunk->m_pBlocks[index].visible ^= (1 << SNOWCOVERED);
        }
    }
    if (m_Keyboard['V']) {
        Chunk *chunk;
        unsigned int index;
        m_World.findBlock(m_AimedBlock, chunk, index);
        if ((chunk)&&(chunk->m_pBlocks[index].material == MAT_DIRT)) {
            chunk->m_pBlocks[index].visible ^= (1 << GRASSCOVERED);
        }
    }
    if (m_Keyboard['E']) {
        m_World.removeBlock(m_AimedBlock);
    }
    if (m_Keyboard['Q']) {
        m_World.addBlock(m_FreeBlock, MAT_PUMPKIN_SHINE);
    }
    if (m_Keyboard['O']) {
        m_World.saveChunks();
    }

    m_Position = m_Position + PointInWorld(frame_interval*m_VelocityX, frame_interval*m_VelocityY, frame_interval*m_VelocityZ);
}

void Character::computeCenterCoords(GLsizei width, GLsizei height)
{
    GLint    viewport[4];		// параметры viewport-a.
    GLdouble projection[16];	// матрица проекции.
    GLdouble modelview[16];		// видовая матрица.
    GLfloat vz;					// координаты курсора мыши в системе координат viewport-a.
    GLdouble disp_center_x, disp_center_y, disp_center_z;			// возвращаемые мировые координаты центра

    glGetIntegerv(GL_VIEWPORT, viewport);           // узнаём параметры viewport-a.
    glGetDoublev(GL_PROJECTION_MATRIX, projection); // узнаём матрицу проекции.
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);   // узнаём видовую матрицу.

    glReadPixels(width/2, height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
    gluUnProject((double) width/2, (double) height/2, (double) vz, modelview, projection, viewport, &disp_center_x, &disp_center_y, &disp_center_z);
    m_CenterPos = m_Position;

    m_CenterPos.bx = 0;
    m_CenterPos.by = 0;
    m_CenterPos.bz = 0;

    m_CenterPos = m_CenterPos + PointInWorld(disp_center_x, disp_center_y, disp_center_z);
}

void Character::computeLocalTime(double time_of_day)
{
    m_LocalTimeOfDay = time_of_day + m_Longitude*DAY_TIME;

    while (m_LocalTimeOfDay >= DAY_TIME) m_LocalTimeOfDay -= DAY_TIME;
    while (m_LocalTimeOfDay < 0.0) m_LocalTimeOfDay += DAY_TIME;
}

void Character::computeMyPosition()
{
    // checks if player is under water level
    PointInWorld pos(m_Position);
    pos.by += 0.125 - 0.5;
    BlockInWorld water_pos(pos);

    m_World.findBlock(water_pos, m_pChunk, m_Index);
    m_UnderWater = m_pChunk && m_pChunk->m_pBlocks[m_Index].material == MAT_WATER;
    m_Longitude = (m_Position.bz/CHUNK_SIZE_XZ + m_Position.cz)/160;
}
