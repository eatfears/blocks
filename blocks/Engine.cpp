#include "Engine.h"
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Light.h"
#include "Primes.h"
#include "Platform.h"

GLfloat DayFogColor[4] = {0.50f, 0.67f, 1.00f, 1.00f};
GLfloat NightFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};
GLfloat WaterFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};

Engine::Engine()
	:stat(*this)
{
	m_Mousing = false;
	m_Fullscreen = false;

	width = 0;
	height = 0;
	FrameInterval = 0.0;
	// todo: time problem! how to measure?
    m_TimeOfDay = 6*100;
}

Engine::~Engine()
{
}

int Engine::initGL()
{
	m_World.MaterialLib.allocGLTextures();
	m_World.MaterialLib.loadGLTextures();

	m_World.player.position = PosInWorld(0, 100, 0);
	m_World.player.dSpinY = -90 - 45;
	glutSetCursor(GLUT_CURSOR_NONE);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);		// normales to 1

	return true;
}

void Engine::reshape(int width, int height)
{
    if (height == 0) {
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

	Character &player = m_World.player;
    if (player.underWater) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);
	}

	//Set position
	glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
	glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	glTranslated(-player.position.bx, -player.position.by, -player.position.bz);

	drawBottomBorder();
	drawSunMoon();

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);
	glHint(GL_FOG_HINT, GL_DONT_CARE);

    if (player.underWater)
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

    if (player.position.by < CHUNK_SIZE_Y + 16) {
		drawClouds();
	}

	glColor3d(1.0, 1.0, 1.0);

	static GLuint *tex = m_World.MaterialLib.m_Texture;
	glBindTexture(GL_TEXTURE_2D, tex[TERRAIN]);
	int render;

    if (player.bKeyboard['Z']) {
		player.bKeyboard['Z'] = 0;
		m_World.SoftLight = !m_World.SoftLight;
		m_World.LightToRefresh = true;
	}

	GLenum mod = GL_EXECUTE;

    if (m_World.LightToRefresh) {
		m_World.LightToRefresh = false;
		mod = GL_COMPILE;
	}

	stat.m_ReRenderedChunks = 0;

	render = 0;
    for (int bin = 0; bin < HASH_SIZE; bin++) {
		auto chunk = m_World.Chunks[bin].begin();
        while (chunk != m_World.Chunks[bin].end()) {
            if ((*chunk)->m_LightToUpdate) {
				m_World.UpdateLight(**chunk);
				(*chunk)->m_LightToUpdate = false;
			}

            if (mod == GL_COMPILE)
				(*chunk)->NeedToRender[0] = RENDER_MAYBE;
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
    for (int bin = 0; bin < HASH_SIZE; bin++) {
		auto chunk = m_World.Chunks[bin].begin();
        while (chunk != m_World.Chunks[bin].end()) {
            if (mod == GL_COMPILE)
				(*chunk)->NeedToRender[1] = RENDER_MAYBE;

			(*chunk)->render(MAT_WATER, &render);
			++chunk;
		}
	}

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	stat.m_ReRenderedChunks += render;

    if (player.position.by >= CHUNK_SIZE_Y + 16) {
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
        glutDestroyWindow(1);
		break;
	default:
#ifdef _WIN32
		button = VkKeyScan(button);
#endif
		m_World.player.bKeyboard[button] = KeyDown;
		break;
	}
}

void Engine::mouseMotion(int x, int y)
{
    if (m_Mousing) {
		Character &player = m_World.player;
		m_Mousing = false;

		player.dSpinY -= (x - width/2)/MOUSE_SENSIVITY;
		player.dSpinX -= (y - height/2)/MOUSE_SENSIVITY;

        while (player.dSpinY >= 360.0)
			player.dSpinY -= 360.0;

        while (player.dSpinY < 0.0)
			player.dSpinY += 360.0;

        if (player.dSpinX < -90.0) player.dSpinX = -90.0;
        if (player.dSpinX > 90.0) player.dSpinX = 90.0;

		glutWarpPointer(width/2,height/2);
	} else {
		m_Mousing = true;
	}
}

void Engine::mouseButton(int button, int state, int x, int y)
{
}

void Engine::initGame()
{
#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);
#endif // _WIN32

	unsigned int seed = std::time(0);

	m_World.lLandscape.init(seed);
	m_World.BuildWorld();
}

void Engine::drawSelectedItem()
{
	Character &player = m_World.player;
    if (!m_World.FindBlock(player.aimedBlock)) {
		return;
	}
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth (1.4f);

	GLdouble BorderSize = 1 + 0.05;
	GLdouble
		dXcoord = (player.aimedBlock.cx - player.position.cx)*CHUNK_SIZE_XZ + player.aimedBlock.bx,
		dYcoord = player.aimedBlock.by,
		dZcoord = (player.aimedBlock.cz - player.position.cz)*CHUNK_SIZE_XZ + player.aimedBlock.bz;

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
	Character &player = m_World.player;
	//glPushMatrix();
	//glPopMatrix();
	int WidthBy2  = width/2;
	int HeightBy2 = height/2;

	drawSelectedItem();

	//glLoadIdentity();
	openGL2d();

	//Underwater haze
    if ((player.underWater)&&(player.chunk)) {
		GLfloat Brightness;
		Light::getLight(*player.chunk, player.index, Brightness);

		GLdouble TextureRotation = player.dSpinY/90;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, m_World.MaterialLib.m_Texture[UNDERWATER]);
		glColor4f(Brightness, Brightness, Brightness, 0.9f);

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

	glBindTexture(GL_TEXTURE_2D, m_World.MaterialLib.m_Texture[VIGNETTE]);

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
	Character &player = m_World.player;
	GetFogColor();
	player.getMyPosition();
	display();
    player.getLocalTime(m_TimeOfDay);
	player.GetCenterCoords(width, height);

	player.Control(FrameInterval);
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
		m_World.player.bSpecial[button] = KeyDown;
	}
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
    if (m_World.player.underWater) {
		fovy = 60.0f;
	} else {
		fovy = 70.0f;
	}

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)width/(GLfloat)height, 0.2f, FARCUT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
}

void Engine::drawSunMoon()
{
	Character &player = m_World.player;
	glPushMatrix();
	glLoadIdentity();

	//Sun
	GLdouble SunSize = 100, sundist = FARCUT*0.8;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	glDisable(GL_CULL_FACE);

	glColor3f(1.0f, 1.0f, 1.0f);
	// Сбросить текущую матрицу
	glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
    glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	glRotated(-player.LocalTimeOfDay*360.0/DAY_TIME + 90.0, 1.0, 0.0, 0.0);

	glBindTexture(GL_TEXTURE_2D, m_World.MaterialLib.m_Texture[SUN]);

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

	glBindTexture(GL_TEXTURE_2D, m_World.MaterialLib.m_Texture[MOON]);
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
	Character &player = m_World.player;
	GLdouble BottomBorderSize = FARCUT;
	GLfloat res;

	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, m_World.MaterialLib.m_Texture[CLOUDS]);
	// todo: coords!!
	glTranslated(player.position.bx, 0, player.position.bz);
	glRotated(90, 1.0, 0.0, 0.0);

	res = 1.0 - m_World.SkyBright;
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
	PosInWorld pos = m_World.player.position;
	GLdouble CloudSize = FARCUT*2;///4;
	GLfloat res;

	static GLdouble time = 0.0;
	time += 0.0001;

	glPushMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, m_World.MaterialLib.m_Texture[CLOUDS]);

	res = 1.0 - m_World.SkyBright;
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
	static double Dawn = 100.0;
	static float NightBright = 0.93f;
	static float DayBright = 0.00f;
	double LocalTimeOfDay = m_World.player.LocalTimeOfDay;

    if ((LocalTimeOfDay > 600.0 + Dawn)&&(LocalTimeOfDay < 1800.0 - Dawn)) {
        for (int i = 0; i < 4; i++)
			FogColor[i] = DayFogColor[i];
		m_World.SkyBright = DayBright;
    } else if ((LocalTimeOfDay < 600.0 - Dawn)||(LocalTimeOfDay > 1800.0 + Dawn)) {
        for (int i = 0; i < 4; i++)
			FogColor[i] = NightFogColor[i];

		m_World.SkyBright = NightBright;
	} else {
		GLfloat ft = (LocalTimeOfDay - (600.0f - Dawn))*M_PI / (2.0 * Dawn);
		GLfloat f = (1.0f - cos(ft)) * 0.5f;

        if (LocalTimeOfDay > 1200.0) f = 1.0 - f;

        for (int i = 0; i < 4; i++) {
			FogColor[i] = NightFogColor[i]*(1.0f - f) + DayFogColor[i]*f;
		}
		m_World.SkyBright = NightBright*(1.0f - f) + DayBright * f;
	}

	static GLfloat prevBright = 0;
	GLfloat dif = fabs(m_World.SkyBright - prevBright);
    if (dif > 0.005f) {
		m_World.LightToRefresh = true;
		prevBright = m_World.SkyBright;
	}

	m_World.TorchBright = 1.0 - 0.05*((rand()%100)/100.0 - 0.5);
}
