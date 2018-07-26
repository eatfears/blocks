#include "engine.h"

#include <chrono>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>

#include "light.h"
#include "noise/primes.h"
#include "platform.h"


static const GLfloat DayFogColor[4] = {0.50f, 0.67f, 1.00f, 1.00f};
static const GLfloat NightFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};
static const GLfloat WaterFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};

Engine::Engine()
    : m_StatWindow(*this)
{
    logger.info() << "Starting";

    m_Mousing = false;
    m_Fullscreen = false;

    m_Width = 0;
    m_Height = 0;
    m_FrameInterval = 0.0;
}

Engine::~Engine()
{
    logger.info() << "Exiting";
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
    m_TimeOfDay = 1300;
}

void Engine::reshape(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }
    m_Width = width;
    m_Height = height;

    openGL3d();
    //glutPostRedisplay();
}

void Engine::display()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Очистить экран и буфер глубины
    openGL3d();

    Character &player = m_World.m_Player;
    if (player.m_UnderWater)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        glClearColor(m_FogColor[0], m_FogColor[1], m_FogColor[2], m_FogColor[3]);
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
        glFogfv(GL_FOG_COLOR, m_FogColor);
        glFogf(GL_FOG_DENSITY, FOG_DENSITY);
        glFogf(GL_FOG_START, FOG_START);
        glFogf(GL_FOG_END, MAX_VIEV_DIST);
    }

    if (player.m_Position.by < CHUNK_SIZE_Y + 16)
    {
        drawClouds();
    }

    glColor3d(1.0, 1.0, 1.0);

    static GLuint *tex = m_World.m_MaterialLib.m_Texture;
    glBindTexture(GL_TEXTURE_2D, tex[TERRAIN]);

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

    m_StatWindow.m_ReRenderedChunks = 0;

    int render = 0;
    for (auto const &it : m_World.m_Chunks)
    {
        Chunk *chunk = it.second;
        if (chunk->m_LightToUpdate)
        {
            m_World.updateLight(*chunk, false);
            chunk->m_LightToUpdate = false;
        }

        if (mod == GL_COMPILE)
            chunk->m_NeedToRender[0] = RENDER_MAYBE;

        chunk->render(MAT_NO, &render);
    }

    m_StatWindow.m_ReRenderedChunks += render;

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //transparent tiles here
    render = 0;
    for (auto const &it : m_World.m_Chunks)
    {
        auto chunk = it.second;
        if (mod == GL_COMPILE)
            chunk->m_NeedToRender[1] = RENDER_MAYBE;

        chunk->render(MAT_WATER, &render);
    }

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

    m_StatWindow.m_ReRenderedChunks += render;

    if (player.m_Position.by >= CHUNK_SIZE_Y + 16)
    {
        drawClouds();
    }

    glDisable(GL_FOG);
}

void Engine::keyboard(unsigned char button, int x, int y, bool key_down)
{
    button = toupper(button);

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
        m_World.m_Player.m_Keyboard[button] = key_down;
        break;
    }
}

void Engine::special(int button, int x, int y, bool key_down)
{
    switch(button)
    {
    case GLUT_KEY_F1:
        if (key_down)
        {
            if (!m_Fullscreen) glutFullScreenToggle();
            else glutLeaveFullScreen();
            m_Fullscreen = !m_Fullscreen;
        }
        break;
        //	case GLUT_KEY_F2: 	glutLeaveGameMode();
        //		break;
    default:
        m_World.m_Player.m_SpecialKeys[button] = key_down;
    }
}

void Engine::mouseMotion(int x, int y)
{
    if (m_Mousing)
    {
        Character &player = m_World.m_Player;
        m_Mousing = false;

        player.m_SpinY -= (x - m_Width/2)*MOUSE_SENSIVITY;
        player.m_SpinX -= (y - m_Height/2)*MOUSE_SENSIVITY;

        while (player.m_SpinY >= 360.0)
            player.m_SpinY -= 360.0;

        while (player.m_SpinY < 0.0)
            player.m_SpinY += 360.0;

        if (player.m_SpinX < -90.0) player.m_SpinX = -90.0;
        if (player.m_SpinX > 90.0) player.m_SpinX = 90.0;

        glutWarpPointer(m_Width/2, m_Height/2);
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

    GLdouble border_size = 1 + 0.05;
    GLdouble
            x_coord = (player.m_AimedBlock.cx - player.m_Position.cx)*CHUNK_SIZE_XZ + player.m_AimedBlock.bx,
            y_coord = player.m_AimedBlock.by,
            z_coord = (player.m_AimedBlock.cz - player.m_Position.cz)*CHUNK_SIZE_XZ + player.m_AimedBlock.bz;

    x_coord -= border_size/2;
    y_coord -= border_size/2 - 0.5;
    z_coord -= border_size/2;

    glBegin(GL_QUADS);

    //Верхняя грань
    glVertex3d(x_coord, y_coord + border_size, z_coord);
    glVertex3d(x_coord, y_coord + border_size, z_coord + border_size);
    glVertex3d(x_coord + border_size, y_coord + border_size, z_coord + border_size);
    glVertex3d(x_coord + border_size, y_coord + border_size, z_coord);
    //Нижняя грань
    glVertex3d(x_coord, y_coord, z_coord);
    glVertex3d(x_coord + border_size, y_coord, z_coord);
    glVertex3d(x_coord + border_size, y_coord, z_coord + border_size);
    glVertex3d(x_coord, y_coord, z_coord + border_size);
    //Правая грань
    glVertex3d(x_coord + border_size, y_coord, z_coord);
    glVertex3d(x_coord + border_size, y_coord + border_size, z_coord);
    glVertex3d(x_coord + border_size, y_coord + border_size, z_coord + border_size);
    glVertex3d(x_coord + border_size, y_coord, z_coord + border_size);
    //Левая грань
    glVertex3d(x_coord, y_coord, z_coord);
    glVertex3d(x_coord, y_coord, z_coord + border_size);
    glVertex3d(x_coord, y_coord + border_size, z_coord + border_size);
    glVertex3d(x_coord, y_coord + border_size, z_coord);
    //Задняя грань
    glVertex3d(x_coord, y_coord, z_coord + border_size);
    glVertex3d(x_coord + border_size, y_coord, z_coord + border_size);
    glVertex3d(x_coord + border_size, y_coord + border_size, z_coord + border_size);
    glVertex3d(x_coord, y_coord + border_size, z_coord + border_size);
    //Передняя грань
    glVertex3d(x_coord, y_coord, z_coord);
    glVertex3d(x_coord, y_coord + border_size, z_coord);
    glVertex3d(x_coord + border_size, y_coord + border_size, z_coord);
    glVertex3d(x_coord + border_size, y_coord, z_coord);

    glEnd();

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void Engine::drawInterface()
{
    Character &player = m_World.m_Player;
    //glPushMatrix();
    //glPopMatrix();
    int width_by_2  = m_Width/2;
    int height_by_2 = m_Height/2;

    drawSelectedItem();

    //glLoadIdentity();
    openGL2d();

    //Underwater haze
    if ((player.m_UnderWater)&&(player.m_pChunk))
    {
        GLfloat brightness;
        brightness = Light::getLight(*player.m_pChunk, player.m_Index);

        GLdouble texture_rotation = player.m_SpinY/90;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[UNDERWATER]);
        glColor4f(brightness, brightness, brightness, 0.9f);

        glBegin(GL_QUADS);
        glTexCoord2d(0.0 - texture_rotation, 0.0);
        glVertex2i(width_by_2- 3*height_by_2, 0);
        glTexCoord2d(0.0 - texture_rotation, 1.0);
        glVertex2i(width_by_2 - 3*height_by_2, m_Height);
        glTexCoord2d(3.0 - texture_rotation, 1.0);
        glVertex2i(width_by_2 + 3*height_by_2, m_Height);
        glTexCoord2d(3.0 - texture_rotation, 0.0);
        glVertex2i(width_by_2 + 3*height_by_2, 0);
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
    glVertex2i(0, m_Height);
    glTexCoord2d(1.0, 1.0);
    glVertex2i(m_Width, m_Height);
    glTexCoord2d(1.0, 0.0);
    glVertex2i(m_Width, 0);
    glEnd();

    glDisable(GL_BLEND);

    //Cross
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth (2.0);

    glBegin(GL_LINES);
    glVertex2i(width_by_2, -9 + height_by_2);
    glVertex2i(width_by_2,  9 + height_by_2);

    glVertex2i(-9 + width_by_2, height_by_2);
    glVertex2i(-1 + width_by_2, height_by_2);
    glVertex2i( 1 + width_by_2, height_by_2);
    glVertex2i( 9 + width_by_2, height_by_2);
    glEnd();

    m_StatWindow.printStat();

    glDisable(GL_BLEND);
}

void Engine::loop()
{
    getFrameTime();
    getFogColor();

    Character &player = m_World.m_Player;
    player.computeMyPosition();
    display();
    player.computeLocalTime(m_TimeOfDay);
    player.computeCenterCoords(m_Width, m_Height);

    /*********************************************************/

    auto pos = player.m_Position;
    int size = 10;
    int max_dist = 11;
    int loaded = 0;

    for (int dist = 0; dist < size; dist++)
    {
        for (ChunkCoord i = -dist; i <= dist; i++)
        {
            for (ChunkCoord j = -dist; j <= dist; j++)
            {
                if ((abs(i) == dist || abs(j) == dist) && loaded < 1)
                {
                    if (m_World.loadChunk(i + pos.cx, j + pos.cz))
                    {
                        loaded++;
                    }
                }
            }
        }
    }

    int unloaded = 0;
    auto it = m_World.m_Chunks.begin();
    while (it != m_World.m_Chunks.end())
    {
        Chunk *chunk = it->second;
        if (abs(chunk->cx - pos.cx) >= max_dist || abs(chunk->cz - pos.cz) >= max_dist)
        {
            chunk->save();
            delete chunk;
            it = m_World.m_Chunks.erase(it);
            unloaded++;
        }
        else
        {
            it++;
        }
        if (unloaded > 10)
        {
            break;
        }
    }

    /*********************************************************/

    player.control(m_FrameInterval);
    drawInterface();

    glutSwapBuffers();
    //glFinish();				//may be bad
}

void Engine::getFrameTime()
{
    static const double koef = 0.0005;
    static const double max_fps = 100;
    static int sleep_time;

    double current_time = GetMillisecTime();
    static double frame_time = current_time;  // Время последнего кадра

    //Интервал времени, прошедшего с прошлого кадра
    m_FrameInterval = current_time - frame_time;
    sleep_time = (int) (1000.0/max_fps - m_FrameInterval);
    if (sleep_time > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        current_time = GetMillisecTime();
        m_FrameInterval = current_time - frame_time;
    }
    frame_time = current_time;
    m_StatWindow.computeFPS(m_FrameInterval);
    m_FrameInterval *= koef;

    m_TimeOfDay += m_FrameInterval;
    while (m_TimeOfDay >= DAY_TIME) m_TimeOfDay -= DAY_TIME;
}

void Engine::openGL2d()
{
    glViewport(0, 0, m_Width, m_Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_Width, 0, m_Height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void Engine::openGL3d()
{
    float fov_y;
    if (m_World.m_Player.m_UnderWater)
    {
        fov_y = 60.0f;
    }
    else
    {
        fov_y = 70.0f;
    }

    glViewport(0, 0, m_Width, m_Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov_y, (GLfloat) m_Width/(GLfloat) m_Height, 0.1f, FARCUT);
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
    GLdouble sun_size = 100, sun_dist = FARCUT*0.8;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glDisable(GL_CULL_FACE);

    glColor3f(1.0f, 1.0f, 1.0f);
    // Сбросить текущую матрицу
    glRotated(-player.m_SpinX, 1.0, 0.0, 0.0);
    glRotated(-player.m_SpinY, 0.0, 1.0, 0.0);
    glRotated(-player.m_LocalTimeOfDay*360.0/DAY_TIME + 90.0, 1.0, 0.0, 0.0);

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[SUN]);

    glTranslated(0.0, 0.0, sun_dist);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-sun_size, -sun_size);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(-sun_size, sun_size);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(sun_size, sun_size);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(sun_size, -sun_size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[MOON]);
    glTranslated(0.0, 0.0, -2*sun_dist);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-sun_size, -sun_size);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(-sun_size, sun_size);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(sun_size, sun_size);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(sun_size, -sun_size);
    glEnd();

    glDisable(GL_BLEND);

    glPopMatrix();
}

void Engine::drawBottomBorder()
{
    Character &player = m_World.m_Player;
    GLdouble bottom_border_size = FARCUT;

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[CLOUDS]);
    // todo: coords!!
    glTranslated(player.m_Position.bx, 0, player.m_Position.bz);
    glRotated(90, 1.0, 0.0, 0.0);

    GLfloat res = m_World.m_SkyBright;
    glColor3f(res, res, res);

    glBegin(GL_QUADS);
    glVertex2d(-bottom_border_size, -bottom_border_size);
    glVertex2d(-bottom_border_size, bottom_border_size);
    glVertex2d(bottom_border_size, bottom_border_size);
    glVertex2d(bottom_border_size, -bottom_border_size);
    glEnd();

    glPopMatrix();
}

void Engine::drawClouds()
{
    GLdouble cloud_size = FARCUT*2;///4;
    static GLdouble time = 0.0;
    time += 0.0001;

    glPushMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, m_World.m_MaterialLib.m_Texture[CLOUDS]);

    GLfloat res = m_World.m_SkyBright;
    glColor4f(res, res, res, 0.8f);

    PointInWorld pos = m_World.m_Player.m_Position;
    GLdouble x_position = (pos.cx*CHUNK_SIZE_XZ + pos.bx)/(2.5*cloud_size);
    GLdouble z_position = (pos.cz*CHUNK_SIZE_XZ + pos.bz)/(2.5*cloud_size);

    glTranslated(pos.bx, CHUNK_SIZE_Y + 16, pos.bz);
    glRotated(90, 1.0, 0.0, 0.0);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0 + time + x_position, 0.0 + z_position);
    glVertex2d(-cloud_size, -cloud_size);
    glTexCoord2d(0.0 + time + x_position, 1.0 + z_position);
    glVertex2d(-cloud_size, cloud_size);
    glTexCoord2d(1.0 + time + x_position, 1.0 + z_position);
    glVertex2d(cloud_size, cloud_size);
    glTexCoord2d(1.0 + time + x_position, 0.0 + z_position);
    glVertex2d(cloud_size, -cloud_size);
    glEnd();

    glDisable(GL_BLEND);
    glPopMatrix();
}

void Engine::getFogColor()
{
    static double dawn = 100.0;
    static float night_bright = 0.1f;
    static float day_bright = 1.00f;
    double local_time_of_day = m_World.m_Player.m_LocalTimeOfDay;

    if ((local_time_of_day > 600.0 + dawn)&&(local_time_of_day < 1800.0 - dawn))
    {
        for (int i = 0; i < 4; i++)
        {
            m_FogColor[i] = DayFogColor[i];
        }
        m_World.m_SkyBright = day_bright;
    }
    else if ((local_time_of_day < 600.0 - dawn)||(local_time_of_day > 1800.0 + dawn))
    {
        for (int i = 0; i < 4; i++)
        {
            m_FogColor[i] = NightFogColor[i];
        }
        m_World.m_SkyBright = night_bright;
    }
    else
    {
        GLfloat ft = (local_time_of_day - (600.0f - dawn))*M_PI / (2.0 * dawn);
        GLfloat f = (1.0f - cos(ft)) * 0.5f;

        if (local_time_of_day > 1200.0) f = 1.0 - f;

        for (int i = 0; i < 4; i++)
        {
            m_FogColor[i] = NightFogColor[i]*(1.0f - f) + DayFogColor[i]*f;
        }
        m_World.m_SkyBright = night_bright*(1.0f - f) + day_bright*f;
    }

    static GLfloat prev_bright = 0;
    GLfloat diff = fabs(m_World.m_SkyBright - prev_bright);
    if (diff > 0.005f)
    {
        m_World.m_LightToRefresh = true;
        prev_bright = m_World.m_SkyBright;
    }

    m_World.m_TorchBright = 1.0 - 0.05*((rand()%100)/100.0 - 0.5);
}
