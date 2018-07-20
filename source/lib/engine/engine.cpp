#include "engine.h"

#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>

#include "light.h"
#include "noise/primes.h"
#include "platform.h"


GLfloat DayFogColor[4] = {0.50f, 0.67f, 1.00f, 1.00f};
GLfloat NightFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};
GLfloat WaterFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};

Engine::Engine()
    :stat(*this)
{
    logger.info() << "Starting";

    m_Mousing = false;
    m_Fullscreen = false;

    width = 0;
    height = 0;
    FrameInterval = 0.0;
    // todo: time problem! how to measure?
    m_TimeOfDay = 550;
}

Engine::~Engine()
{
    logger.info() << "Exit";
    glutExit();
}

void Engine::initGL()
{
    logger.info() << "initGL";

    m_World.m_MaterialLib.loadGLTextures();

    glutSetCursor(GLUT_CURSOR_NONE);

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0f);

//    glDepthFunc(GL_LEQUAL);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);		// normales to 1
}

void Engine::initGame()
{
    logger.info() << "initGame";

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_HIDE);
#endif // _WIN32

    m_World.m_Landscape.init(std::time(0));
    m_World.buildWorld();

    m_World.m_Player.m_Position = PointInWorld(0, 100, 0);
    m_World.m_Player.m_SpinY = -90 - 45;
}

void Engine::reshape(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }
    this->width = width;
    this->height = height;

    openGL3d();
    //glutPostRedisplay();
}

void Engine::display()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Очистить экран и буфер глубины
    glLoadIdentity();											// Сбросить текущую матрицу
    openGL3d();

    Character &player = m_World.m_Player;
    if (player.m_UnderWater)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);
    }

    //Set position
    glRotated(-player.m_SpinX, 1.0, 0.0, 0.0);
    glRotated(-player.m_SpinY, 0.0, 1.0, 0.0);
    glTranslated(-player.m_Position.bx, -player.m_Position.by, -player.m_Position.bz);

    drawBottomBorder();
    drawSunMoon();

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE,  GL_LINEAR);
    glHint(GL_FOG_HINT, GL_DONT_CARE);

    if (player.m_UnderWater)
    {
        glFogfv(GL_FOG_COLOR, WaterFogColor);
        glFogf(GL_FOG_DENSITY, 20.0);
        glFogf(GL_FOG_START, 10);
        glFogf(GL_FOG_END, 30);
    }
    else
    {
        glFogfv(GL_FOG_COLOR, FogColor);
        glFogf(GL_FOG_DENSITY, FOG_DENSITY);
        glFogf(GL_FOG_START, FOG_START);
        glFogf(GL_FOG_END, MAX_VIEV_DIST);
    }

    if (player.m_Position.by < CHUNK_SIZE_Y + 16) {
        drawClouds();
    }

    glColor3d(1.0, 1.0, 1.0);

    static GLuint *tex = m_World.m_MaterialLib.m_Texture;
    glBindTexture(GL_TEXTURE_2D, tex[TERRAIN]);
    int render;

    if (player.m_Keyboard['Z'])
    {
        player.m_Keyboard['Z'] = 0;
        m_World.m_SoftLight = !m_World.m_SoftLight;
        m_World.m_LightToRefresh = true;
    }

    GLenum mod = GL_EXECUTE;

    if (m_World.m_LightToRefresh)
    {
        m_World.m_LightToRefresh = false;
        mod = GL_COMPILE;
    }

    stat.m_ReRenderedChunks = 0;

    render = 0;
    for (int bin = 0; bin < HASH_SIZE; bin++)
    {
        auto chunk = m_World.m_Chunks[bin].begin();
        while (chunk != m_World.m_Chunks[bin].end())
        {
            if ((*chunk)->m_LightToUpdate)
            {
                m_World.updateLight(**chunk);
                (*chunk)->m_LightToUpdate = false;
            }

            if (mod == GL_COMPILE)
                (*chunk)->m_NeedToRender[0] = RENDER_MAYBE;
            (*chunk)->render(MAT_NO, &render);
            ++chunk;
        }
    }

    stat.m_ReRenderedChunks += render;

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //transparent tiles here
    render = 0;
    for (int bin = 0; bin < HASH_SIZE; bin++)
    {
        auto chunk = m_World.m_Chunks[bin].begin();
        while (chunk != m_World.m_Chunks[bin].end())
        {
            if (mod == GL_COMPILE)
                (*chunk)->m_NeedToRender[1] = RENDER_MAYBE;

            (*chunk)->render(MAT_WATER, &render);
            ++chunk;
        }
    }

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

    stat.m_ReRenderedChunks += render;

    if (player.m_Position.by >= CHUNK_SIZE_Y + 16)
    {
        drawClouds();
    }

    glDisable(GL_FOG);
}

void Engine::keyboard(unsigned char button, int x, int y, bool KeyDown)
{
    switch(button)
    {
    case KEY_ESCAPE:
        // TODO: window id
        // glutDestroyWindow(1);
        glutLeaveMainLoop();
        break;
    default:
#ifdef _WIN32
        button = VkKeyScan(button);
#endif
        m_World.m_Player.m_Keyboard[button] = KeyDown;
        break;
    }
}

void Engine::special(int button, int x, int y, bool KeyDown)
{
    switch(button)
    {
    case GLUT_KEY_F1:
        if (KeyDown)
        {
            if (!m_Fullscreen) glutFullScreenToggle();
            else glutLeaveFullScreen();
            m_Fullscreen = !m_Fullscreen;
        }
        break;
        //	case GLUT_KEY_F2: 	glutLeaveGameMode();
        //		break;
    default:
        m_World.m_Player.m_SpecialKeys[button] = KeyDown;
    }
}

void Engine::mouseMotion(int x, int y)
{
    if (m_Mousing)
    {
        Character &player = m_World.m_Player;
        m_Mousing = false;

        player.m_SpinY -= (x - width/2)/MOUSE_SENSIVITY;
        player.m_SpinX -= (y - height/2)/MOUSE_SENSIVITY;

        while (player.m_SpinY >= 360.0)
            player.m_SpinY -= 360.0;

        while (player.m_SpinY < 0.0)
            player.m_SpinY += 360.0;

        if (player.m_SpinX < -90.0) player.m_SpinX = -90.0;
        if (player.m_SpinX > 90.0) player.m_SpinX = 90.0;

        glutWarpPointer(width/2,height/2);
    }
    else
    {
        m_Mousing = true;
    }
}

void Engine::mouseButton(int button, int state, int x, int y)
{
}

void Engine::drawSelectedItem()
{
    Character &player = m_World.m_Player;
    if (!m_World.findBlock(player.m_AimedBlock))
    {
        return;
    }
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth (1.4f);

    GLdouble BorderSize = 1 + 0.05;
    GLdouble
            dXcoord = (player.m_AimedBlock.cx - player.m_Position.cx)*CHUNK_SIZE_XZ + player.m_AimedBlock.bx,
            dYcoord = player.m_AimedBlock.by,
            dZcoord = (player.m_AimedBlock.cz - player.m_Position.cz)*CHUNK_SIZE_XZ + player.m_AimedBlock.bz;

    dXcoord -= BorderSize/2;
    dYcoord -= BorderSize/2 - 0.5;
    dZcoord -= BorderSize/2;

    glBegin(GL_QUADS);

    //Верхняя грань
    glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
    glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
    glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
    glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
    //Нижняя грань
    glVertex3d (dXcoord, dYcoord, dZcoord);
    glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);
    glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
    glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
    //Правая грань
    glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);
    glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
    glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
    glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
    //Левая грань
    glVertex3d (dXcoord, dYcoord, dZcoord);
    glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
    glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
    glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
    //Задняя грань
    glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
    glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
    glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
    glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
    //Передняя грань
    glVertex3d (dXcoord, dYcoord, dZcoord);
    glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
    glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
    glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);

    glEnd();

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void Engine::drawInterface()
{
    Character &player = m_World.m_Player;
    //glPushMatrix();
    //glPopMatrix();
    int WidthBy2  = width/2;
    int HeightBy2 = height/2;

    drawSelectedItem();

    //glLoadIdentity();
    openGL2d();

    //Underwater haze
    if ((player.m_UnderWater)&&(player.m_pChunk))
    {
        GLfloat brightness;
        brightness = Light::getLight(*player.m_pChunk, player.m_Index);

        GLdouble TextureRotation = player.m_SpinY/90;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[UNDERWATER]);
        glColor4f(brightness, brightness, brightness, 0.9f);

        glBegin(GL_QUADS);
        glTexCoord2d(0.0 - TextureRotation, 0.0);
        glVertex2i(WidthBy2- 3*HeightBy2, 0);
        glTexCoord2d(0.0 - TextureRotation, 1.0);
        glVertex2i(WidthBy2 - 3*HeightBy2, height);
        glTexCoord2d(3.0 - TextureRotation, 1.0);
        glVertex2i(WidthBy2 + 3*HeightBy2, height);
        glTexCoord2d(3.0 - TextureRotation, 0.0);
        glVertex2i(WidthBy2 + 3*HeightBy2, 0);
        glEnd();

        glDisable(GL_BLEND);
    }

    //Vignette
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[VIGNETTE]);

    glColor3f(0.4f, 0.4f, 0.4f);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2i(0, 0);
    glTexCoord2d(0.0, 1.0);
    glVertex2i(0, height);
    glTexCoord2d(1.0, 1.0);
    glVertex2i(width, height);
    glTexCoord2d(1.0, 0.0);
    glVertex2i(width, 0);
    glEnd();

    glDisable(GL_BLEND);

    //Cross
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth (2.0);

    glBegin(GL_LINES);
    glVertex2i(WidthBy2, -9 + HeightBy2);
    glVertex2i(WidthBy2,  9 + HeightBy2);

    glVertex2i(-9 + WidthBy2, HeightBy2);
    glVertex2i(-1 + WidthBy2, HeightBy2);
    glVertex2i( 1 + WidthBy2, HeightBy2);
    glVertex2i( 9 + WidthBy2, HeightBy2);
    glEnd();

    stat.printStat();

    glDisable(GL_BLEND);
}

void Engine::loop()
{
    GetFrameTime();
    Character &player = m_World.m_Player;
    GetFogColor();
    player.computeMyPosition();
    display();
    player.computeLocalTime(m_TimeOfDay);
    player.computeCenterCoords(width, height);

    player.control(FrameInterval);
    drawInterface();

    glutSwapBuffers();
    glFinish();				//may be bad!!!!!!!
}

void Engine::GetFrameTime()
{
    static double koef = 0.0005;
    static double max_FPS = 30;
    static int sleep_time;

    double currentTime = GetMillisecTime();
    static double frameTime = currentTime;  // Время последнего кадра

    //Интервал времени, прошедшего с прошлого кадра
    FrameInterval = currentTime - frameTime;
    sleep_time = (int) (1000.0/max_FPS - FrameInterval);
    if (sleep_time > 0) {
        Sleep(sleep_time);
        currentTime = GetMillisecTime();
        FrameInterval = currentTime - frameTime;
    }
    frameTime = currentTime;
    stat.computeFPS(FrameInterval);

    FrameInterval *= koef;

    m_TimeOfDay += FrameInterval;
    while (m_TimeOfDay >= DAY_TIME) m_TimeOfDay -= DAY_TIME;
}

void Engine::openGL2d()
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void Engine::openGL3d()
{
    float fovy;
    if (m_World.m_Player.m_UnderWater)
    {
        fovy = 60.0f;
    }
    else
    {
        fovy = 70.0f;
    }

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (GLfloat)width/(GLfloat)height, 1.2f, FARCUT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
}

void Engine::drawSunMoon()
{
    Character &player = m_World.m_Player;
    glPushMatrix();
    glLoadIdentity();

    //Sun
    GLdouble SunSize = 100, sundist = FARCUT*0.8;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glDisable(GL_CULL_FACE);

    glColor3f(1.0f, 1.0f, 1.0f);
    // Сбросить текущую матрицу
    glRotated(-player.m_SpinX, 1.0, 0.0, 0.0);
    glRotated(-player.m_SpinY, 0.0, 1.0, 0.0);
    glRotated(-player.m_LocalTimeOfDay*360.0/DAY_TIME + 90.0, 1.0, 0.0, 0.0);

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[SUN]);

    glTranslated(0.0, 0.0, sundist);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-SunSize, -SunSize);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(-SunSize, SunSize);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(SunSize, SunSize);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(SunSize, -SunSize);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[MOON]);
    glTranslated(0.0, 0.0, -2*sundist);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-SunSize, -SunSize);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(-SunSize, SunSize);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(SunSize, SunSize);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(SunSize, -SunSize);
    glEnd();

    glDisable(GL_BLEND);

    glPopMatrix();
}

void Engine::drawBottomBorder()
{
    Character &player = m_World.m_Player;
    GLdouble BottomBorderSize = FARCUT;
    GLfloat res;

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[CLOUDS]);
    // todo: coords!!
    glTranslated(player.m_Position.bx, 0, player.m_Position.bz);
    glRotated(90, 1.0, 0.0, 0.0);

    res = 1.0 - m_World.m_SkyBright;
    glColor3f(res, res, res);

    glBegin(GL_QUADS);
    glVertex2d(-BottomBorderSize, -BottomBorderSize);
    glVertex2d(-BottomBorderSize, BottomBorderSize);
    glVertex2d(BottomBorderSize, BottomBorderSize);
    glVertex2d(BottomBorderSize, -BottomBorderSize);
    glEnd();

    glPopMatrix();
}

void Engine::drawClouds()
{
    PointInWorld pos = m_World.m_Player.m_Position;
    GLdouble CloudSize = FARCUT*2;///4;
    GLfloat res;

    static GLdouble time = 0.0;
    time += 0.0001;

    glPushMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[CLOUDS]);

    res = 1.0 - m_World.m_SkyBright;
    glColor4f(res, res, res, 0.8f);
    // todo: wtf with coords

    GLdouble Xposition = pos.bx/(2.5*CloudSize);
    GLdouble Zposition = pos.bz/(2.5*CloudSize);

    glTranslated(pos.bx, CHUNK_SIZE_Y + 16, pos.bz);
    glRotated(90, 1.0, 0.0, 0.0);
    glBegin(GL_QUADS);

    glTexCoord2d(0.0 + time + Xposition, 0.0 + Zposition);
    glVertex2d(-CloudSize, -CloudSize);
    glTexCoord2d(0.0 + time + Xposition, 1.0 + Zposition);
    glVertex2d(-CloudSize, CloudSize);
    glTexCoord2d(1.0 + time + Xposition, 1.0 + Zposition);
    glVertex2d(CloudSize, CloudSize);
    glTexCoord2d(1.0 + time + Xposition, 0.0 + Zposition);
    glVertex2d(CloudSize, -CloudSize);

    glEnd();

    glDisable(GL_BLEND);
    glPopMatrix();
}

void Engine::GetFogColor()
{
    static double dawn = 100.0;
    static float night_bright = 0.93f;
    static float day_bright = 0.00f;
    double local_time_of_day = m_World.m_Player.m_LocalTimeOfDay;

    if ((local_time_of_day > 600.0 + dawn)&&(local_time_of_day < 1800.0 - dawn))
    {
        for (int i = 0; i < 4; i++)
            FogColor[i] = DayFogColor[i];
        m_World.m_SkyBright = day_bright;
    }
    else if ((local_time_of_day < 600.0 - dawn)||(local_time_of_day > 1800.0 + dawn))
    {
        for (int i = 0; i < 4; i++)
            FogColor[i] = NightFogColor[i];

        m_World.m_SkyBright = night_bright;
    }
    else
    {
        GLfloat ft = (local_time_of_day - (600.0f - dawn))*M_PI / (2.0 * dawn);
        GLfloat f = (1.0f - cos(ft)) * 0.5f;

        if (local_time_of_day > 1200.0) f = 1.0 - f;

        for (int i = 0; i < 4; i++)
        {
            FogColor[i] = NightFogColor[i]*(1.0f - f) + DayFogColor[i]*f;
        }
        m_World.m_SkyBright = night_bright*(1.0f - f) + day_bright * f;
    }

    static GLfloat prevBright = 0;
    GLfloat dif = fabs(m_World.m_SkyBright - prevBright);
    if (dif > 0.005f)
    {
        m_World.m_LightToRefresh = true;
        prevBright = m_World.m_SkyBright;
    }

    m_World.m_TorchBright = 1.0 - 0.05*((rand()%100)/100.0 - 0.5);
}
