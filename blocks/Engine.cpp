#include "Engine.h"
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Light.h"
#include "Primes.h"
#include "PlatformDefinitions.h"

GLfloat DayFogColor[4] = {0.50f, 0.67f, 1.00f, 1.00f};
GLfloat NightFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};
GLfloat WaterFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};

Engine::Engine()
	:stat(*this)
{
	bMousing = false;
	fullscreen = false;

	width = 0;
	height = 0;
	FrameInterval = 0.0;
	TimeOfDay = 100.0;
}

Engine::~Engine()
{
}

int Engine::InitGL()
{
	wWorld.MaterialLib.AllocGLTextures();
	wWorld.MaterialLib.LoadGLTextures();

	wWorld.player.dPositionY = 100*BLOCK_SIZE+00.0;
	wWorld.player.dSpinY = -90 - 45;
	glutSetCursor(GLUT_CURSOR_NONE);							//Выставляем на НЕТ КУРСОР


	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0f);											// Разрешить очистку буфера глубины

	glEnable(GL_DEPTH_TEST);									// Разрешить тест глубины

	glDepthFunc(GL_LEQUAL);										// Тип теста глубины
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Улучшение в вычислении перспективы

	//рассчет текстур
	glEnable(GL_TEXTURE_2D);

	//автоматическое приведение нормалей к единичной длине
	glEnable(GL_NORMALIZE);

	return true;
}

void Engine::Reshape(int width, int height)
{
	if(height == 0)											// Предотвращение деления на ноль
		height = 1;

	this->width = width;
	this->height = height;

	OpenGL3d();
	//glutPostRedisplay();
}

void Engine::Display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Очистить экран и буфер глубины
	glLoadIdentity();											// Сбросить текущую матрицу
	OpenGL3d();

	Character &player = wWorld.player;
	if(player.UnderWater)
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	else
		glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);


	//Set position
	glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
	glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	glTranslated(-player.dPositionX, -player.dPositionY, -player.dPositionZ);

	DrawBottomBorder();
	DrawSunMoon();

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);
	glHint(GL_FOG_HINT, GL_DONT_CARE);

	if(player.UnderWater)
	{
		glFogfv(GL_FOG_COLOR, WaterFogColor);
		glFogf(GL_FOG_DENSITY, 20.0);
		glFogf(GL_FOG_START, BLOCK_SIZE*10);
		glFogf(GL_FOG_END, BLOCK_SIZE*30);
	}
	else
	{
		glFogfv(GL_FOG_COLOR, FogColor);
		glFogf(GL_FOG_DENSITY, FOG_DENSITY);
		glFogf(GL_FOG_START, FOG_START);
		glFogf(GL_FOG_END, MAX_VIEV_DIST);
	}

	if(player.dPositionY < (CHUNK_SIZE_Y + 16)*BLOCK_SIZE)
		DrawClouds();


	glColor3d(1.0, 1.0, 1.0);

	static GLuint *tex = wWorld.MaterialLib.texture;
	glBindTexture(GL_TEXTURE_2D, tex[TERRAIN]);
	int render;

	if(player.bKeyboard['Z'])
	{
		player.bKeyboard['Z'] = 0;
		wWorld.SoftLight = !wWorld.SoftLight;
		wWorld.LightToRefresh = true;
	}

	GLenum mod = GL_EXECUTE;

	if(wWorld.LightToRefresh)
	{
		wWorld.LightToRefresh = false;
		mod = GL_COMPILE;
	}

	stat.reRenderedChunks = 0;

	render = 0;
	for (int bin = 0; bin < HASH_SIZE; bin++)
	{
		auto chunk = wWorld.Chunks[bin].begin();
		while(chunk != wWorld.Chunks[bin].end())
		{
			if((*chunk)->LightToUpdate)
			{
				wWorld.UpdateLight(**chunk);
				(*chunk)->LightToUpdate = false;
			}

#ifndef _DEBUG
			_try
			{
#endif // _DEBUG
				if (mod == GL_COMPILE)
					(*chunk)->NeedToRender[0] = RENDER_MAYBE;
				(*chunk)->Render(MAT_NO, &render);
				++chunk;

#ifndef _DEBUG
			}
			_except (EXCEPTION_EXECUTE_HANDLER)
			{
				break;
			}
#endif // _DEBUG
		}
	}

	stat.reRenderedChunks += render;

	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//transparent tiles here
	render = 0;
	for (int bin = 0; bin < HASH_SIZE; bin++)
	{
		auto chunk = wWorld.Chunks[bin].begin();
		while(chunk != wWorld.Chunks[bin].end())
		{
#ifndef _DEBUG
			_try
			{
#endif // _DEBUG
				if (mod == GL_COMPILE)
					(*chunk)->NeedToRender[1] = RENDER_MAYBE;

				(*chunk)->Render(MAT_WATER, &render);
				++chunk;

#ifndef _DEBUG
			}
			_except (EXCEPTION_EXECUTE_HANDLER)
			{
				break;
			}
#endif // _DEBUG
		}
	}

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	stat.reRenderedChunks += render;


	if(player.dPositionY >= (CHUNK_SIZE_Y + 16)*BLOCK_SIZE)
		DrawClouds();

	glDisable(GL_FOG);


#ifdef DEBUG_OUT
	FILE *out;
	out = fopen(DEBUG_FILE, "w");
	fprintf(out,"Debug\n");
	fclose(out);
#endif

}

void Engine::Keyboard(unsigned char button, int x, int y, bool KeyDown)
{
	switch(button)
	{
	case KEY_ESCAPE: glutExit();
		break;
	default:
#ifdef _WIN32
		button = VkKeyScan(button);
#endif
		wWorld.player.bKeyboard[button] = KeyDown;
		break;
	}
}

void Engine::MouseMotion(int x, int y)
{
	if(bMousing)
	{
		Character &player = wWorld.player;
		bMousing = false;

		player.dSpinY -= (x - width/2)/MOUSE_SENSIVITY;
		player.dSpinX -= (y - height/2)/MOUSE_SENSIVITY;

		while(player.dSpinY >= 360.0)
			player.dSpinY -= 360.0;

		while(player.dSpinY < 0.0)
			player.dSpinY += 360.0;

		if(player.dSpinX < -90.0) player.dSpinX = -90.0;
		if(player.dSpinX > 90.0) player.dSpinX = 90.0;

		glutWarpPointer(width/2,height/2);
	}
	else
		bMousing = true;
}

void Engine::MouseButton(int button, int state, int x, int y)
{
// 	switch(button)
// 	{
// 	case GLUT_LEFT_BUTTON:
// 		if (state==GLUT_DOWN) glutIdleFunc(spinDisplay); break;
//
// 	case GLUT_RIGHT_BUTTON:
// 		if (state==GLUT_DOWN) glutIdleFunc(NULL); break;
// 	}
}

void Engine::InitGame()
{
#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);
#endif // _WIN32

	unsigned int seed = std::time(0);

	wWorld.lLandscape.Init(seed);
	wWorld.BuildWorld();
}

void Engine::DrawSelectedItem()
{
	Character &player = wWorld.player;
	if(!wWorld.FindBlock(player.sCenterBlockCoordX,player.sCenterBlockCoordY,player.sCenterBlockCoordZ))
		return;
	if((player.sCenterBlockCoordY >= CHUNK_SIZE_Y)||(player.sCenterBlockCoordY < 0))
		return;

	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth (1.4f);

	GLdouble BorderSize = BLOCK_SIZE*(1 + 0.005);
	GLdouble
		dXcoord = player.sCenterBlockCoordX*BLOCK_SIZE,
		dYcoord = player.sCenterBlockCoordY*BLOCK_SIZE,
		dZcoord = player.sCenterBlockCoordZ*BLOCK_SIZE;

	dXcoord -= BorderSize/2;
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

void Engine::DrawInterface()
{
	Character &player = wWorld.player;
	//glPushMatrix();
	//glPopMatrix();
	int WidthBy2  = width/2;
	int HeightBy2 = height/2;

	DrawSelectedItem();

	//glLoadIdentity();
	OpenGL2d();

	//Underwater haze
	if((player.UnderWater)&&(player.chunk))
	{
		GLfloat Brightness;
		Light::GetLight(*player.chunk, player.index, Brightness);

		GLdouble TextureRotation = player.dSpinY/90;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[UNDERWATER]);
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

	glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[VIGNETTE]);

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

	//Statistics
	stat.PrintStat();

	glDisable(GL_BLEND);
}

void Engine::Loop()
{
	Character &player = wWorld.player;
	GetFogColor();
	player.GetMyPosition();
	Display();
	player.GetCenterCoords(width, height);

	player.Control(FrameInterval);
	DrawInterface();

	GetFrameTime();

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
	if(sleep_time > 0)
	{
		Sleep(sleep_time);
		currentTime = GetMillisecTime();
		FrameInterval = currentTime - frameTime;
	}
	frameTime = currentTime;
	stat.ComputeFPS(FrameInterval);

	FrameInterval *= koef;

	TimeOfDay += FrameInterval;
	while (TimeOfDay >= 2400.0) TimeOfDay -= 2400.0;
}

void Engine::Special(int button, int x, int y, bool KeyDown)
{
	switch(button)
	{
	case GLUT_KEY_F1:
		if(KeyDown)
		{
			if(!fullscreen) glutFullScreenToggle();
			else glutLeaveFullScreen();
			fullscreen = !fullscreen;
		}
		break;
//	case GLUT_KEY_F2: 	glutLeaveGameMode();
//		break;
	default:
		wWorld.player.bSpecial[button] = KeyDown;
	}
}

void Engine::OpenGL2d()
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void Engine::OpenGL3d()
{
	float fovy;
	if (wWorld.player.UnderWater)
		fovy = 60.0f;
	else
		fovy = 70.0f;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)width/(GLfloat)height, 0.2f, FARCUT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
}

void Engine::DrawSunMoon()
{
	Character &player = wWorld.player;
	glPushMatrix();

	glLoadIdentity();

	//Sun
	GLdouble SunSize = 100*BLOCK_SIZE, sundist = FARCUT*0.8;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	glDisable(GL_CULL_FACE);

	glColor3f(1.0f, 1.0f, 1.0f);
											// Сбросить текущую матрицу
	glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
	glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	glRotated(-TimeOfDay*360.0/2400.0 + 90.0, 1.0, 0.0, 0.0);

	glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[SUN]);

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

	glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[MOON]);
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

void Engine::DrawBottomBorder()
{
	Character &player = wWorld.player;
	GLdouble BottomBorderSize = FARCUT;
	GLfloat res;

	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[CLOUDS]);

	glTranslated(player.dPositionX, 0, player.dPositionZ);
	glRotated(90, 1.0, 0.0, 0.0);

	res = 1.0 - wWorld.SkyBright;
	//res = res - wWorld.SkyBright;
	glColor3f(res, res, res);

	glBegin(GL_QUADS);
	//glTexCoord2d(0.0 + time + Xposition, 0.0 + Zposition);
	glVertex2d(-BottomBorderSize, -BottomBorderSize);
	//glTexCoord2d(0.0 + time + Xposition, 1.0 + Zposition);
	glVertex2d(-BottomBorderSize, BottomBorderSize);
	//glTexCoord2d(1.0 + time + Xposition, 1.0 + Zposition);
	glVertex2d(BottomBorderSize, BottomBorderSize);
	//glTexCoord2d(1.0 + time + Xposition, 0.0 + Zposition);
	glVertex2d(BottomBorderSize, -BottomBorderSize);
	glEnd();


	glPopMatrix();
}

void Engine::DrawClouds()
{
	Character &player = wWorld.player;
	GLdouble CloudSize = FARCUT*2;///4;
	GLfloat res;

	static GLdouble time = 0.0;
	time += 0.0001;

	glPushMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[CLOUDS]);

	res = 1.0 - wWorld.SkyBright;
	glColor4f(res, res, res, 0.8f);

	GLdouble Xposition = player.dPositionX/(2.5*CloudSize);
	GLdouble Zposition = player.dPositionZ/(2.5*CloudSize);

	glTranslated(player.dPositionX, (CHUNK_SIZE_Y + 16)*BLOCK_SIZE, player.dPositionZ);
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

	if ((TimeOfDay > 600.0 + Dawn)&&(TimeOfDay < 1800.0 - Dawn))
	{
		for(int i = 0; i < 4; i++)
			FogColor[i] = DayFogColor[i];
		wWorld.SkyBright = DayBright;
	}
	else if ((TimeOfDay < 600.0 - Dawn)||(TimeOfDay > 1800.0 + Dawn))
	{
		for(int i = 0; i < 4; i++)
			FogColor[i] = NightFogColor[i];

		wWorld.SkyBright = NightBright;
	}
	else
	{
		GLfloat ft = (TimeOfDay - (600.0f - Dawn))*M_PI / (2.0 * Dawn);
		GLfloat f = (1.0f - cos(ft)) * 0.5f;

		if(TimeOfDay > 1200.0) f = 1.0 - f;

		for(int i = 0; i < 4; i++)
			FogColor[i] = NightFogColor[i]*(1.0f - f) + DayFogColor[i]*f;

		wWorld.SkyBright = NightBright*(1.0f - f) + DayBright * f;
	}

	static GLfloat prevBright = 0;
	GLfloat dif = fabs(wWorld.SkyBright - prevBright);
	if(dif > 0.005f)
	{
		wWorld.LightToRefresh = true;
		prevBright = wWorld.SkyBright;
	}
}
