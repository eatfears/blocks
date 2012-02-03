#include "Engine.h"
#include "Blocks_Definitions.h"
#include <time.h>
#include <Mmsystem.h>

Engine::Engine(void)
	:player(wWorld)
{
	srand((unsigned int)time(NULL));
	int seed = rand();
	randNumGen = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(randNumGen, seed);

	active = true;
	bMousing = false;
	fullscreen = false;
}

Engine::~Engine(void)
{
}

int Engine::Init()
{
	player.dPositionY = 10;//100*BLOCK_SIZE+00.0;
	player.dSpinY = -90 - 45;
	glutSetCursor(GLUT_CURSOR_NONE);							//Выставляем на НЕТ КУРСОР


	glShadeModel(GL_SMOOTH);									// Разрешить плавное цветовое сглаживание
	glClearColor(FOG_COLOR);									// Очистка экрана в черный цвет
	glClearDepth(1.0f);											// Разрешить очистку буфера глубины

	glEnable(GL_DEPTH_TEST);									// Разрешить тест глубины

	glDepthFunc(GL_LEQUAL);										// Тип теста глубины
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Улучшение в вычислении перспективы

	//рассчет освещения 
	//glEnable(GL_LIGHTING);

	//2-sided tiles
	//glEnable(GL_CULL_FACE);

	//рассчет текстур
	glEnable(GL_TEXTURE_2D);
	//двухсторонний расчет освещения 
	//	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	//автоматическое приведение нормалей к единичной длине
	glEnable(GL_NORMALIZE);

	return true;	
}

void Engine::Reshape( int width, int height )
{
	if( height == 0 )											// Предотвращение деления на ноль 
	{
		height = 1;
	}

	glViewport( 0, 0, width, height );							// Сброс текущей области вывода
	glMatrixMode( GL_PROJECTION );								// Выбор матрицы проекций
	glLoadIdentity();											// Сброс матрицы проекции

	//Вычисление соотношения геометрических размеров для окна
	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, BLOCK_SIZE + MAX_VIEV_DIST);
	glMatrixMode( GL_MODELVIEW );								// Выбор матрицы вида модели
	glLoadIdentity();											// Сброс матрицы вида модели

	this->width = width;
	this->height = height;
	/*
	glViewport(0,0,(GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0,50.0,-50.0,50.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/
}

void Engine::Display()
{
	glEnable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// Очистить экран и буфер глубины
	glLoadIdentity();											// Сбросить текущую матрицу

	glRotated( -player.dSpinX, 1.0, 0.0, 0.0 );
	glRotated( -player.dSpinY, 0.0, 1.0, 0.0 );
	glTranslated(-player.dPositionX, -player.dPositionY, -player.dPositionZ);


	GLdouble dBrightness = 1.0;
	glColor3d(dBrightness, dBrightness, dBrightness);


	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);

	glutSolidCube(5);

	glEnd();


	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//transparent tiles here


	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);


	glDisable(GL_FOG);
	glDisable(GL_LIGHT2);


	// 
	// 	glClear(GL_COLOR_BUFFER_BIT);
	// 	glPushMatrix();
	// 	glRotatef(spin,0.0,0.0,1.0);
	// 	glColor3f(1.0,1.0,1.0);
	// 	glRectf(-25.0,-25.0,25.0,25.0);
	// 	glPopMatrix();
	// 	glutSwapBuffers();
}

void Engine::Keyboard( unsigned char button, int x, int y, bool KeyUp)
{
	switch(button)
	{
	case '1': 

		break;
	case VK_ESCAPE: exit(0);
		break;
	}
}

void Engine::MouseMotion( int x, int y )
{
	static int Lastx, Lasty;

	if(bMousing)
	{
		bMousing = false;
		{
			player.dSpinY -= (x - Lastx)/MOUSE_SENSIVITY;
			player.dSpinX -= (y - Lasty)/MOUSE_SENSIVITY;
		}

		while(player.dSpinY >= 360.0)
			player.dSpinY -= 360.0;

		while(player.dSpinY < 0.0)
			player.dSpinY += 360.0;

		if(player.dSpinX < -90.0) player.dSpinX = -90.0;
		if(player.dSpinX > 90.0) player.dSpinX = 90.0;

		Lastx = x;
		Lasty = y;

		glutWarpPointer(width/2,height/2);

	}
	else 
	{
		bMousing = true;
		Lastx = x;
		Lasty = y;
	}
}

void Engine::InitGame()// GLWindow *glwWnd )
{
	//HANDLE threadHandle = GetCurrentThread();
	//SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);

	//this->glwWnd = glwWnd;
	wWorld.MaterialLib.AllocGLTextures();
	wWorld.MaterialLib.LoadGLTextures();
	wWorld.BuildWorld();
}

void Engine::DrawTile( BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char N )
{
	GLdouble 
		// 		dXcoord = (sXcoord-player.lnwPositionX*LOCATION_SIZE_XZ)*TILE_SIZE, 
		// 		dYcoord = sYcoord*TILE_SIZE, 
		// 		dZcoord = (sZcoord-player.lnwPositionZ*LOCATION_SIZE_XZ)*TILE_SIZE;
		dXcoord = sXcoord*BLOCK_SIZE, 
		dYcoord = sYcoord*BLOCK_SIZE, 
		dZcoord = sZcoord*BLOCK_SIZE;

	dXcoord -= BLOCK_SIZE/2;
	dZcoord -= BLOCK_SIZE/2;

	static double space = 0.0005;
	static double offsetx;
	static double offsety;

	wWorld.MaterialLib.GetTextureOffsets(offsetx, offsety, material, N);

	switch(N)
	{
	case TOP:
		{
			//Верхняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BOTTOM:
		{
			//Нижняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case RIGHT:
		{
			//Правая грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case LEFT:
		{
			//Левая грань
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BACK:
		{
			//Задняя грань
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
		}break;
	case FRONT:
		{
			//Передняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
		}break;
	}
}

void Engine::Loop()
{

	Display();
	player.GetCenterCoords(width, height);

	player.Control(FrameInterval);
	//DrawInterface();

	GetFrameTime();

	glutSwapBuffers();

/*
	if(player.bKeyboard[VK_F1])						// Is F1 Being Pressed?
	{
		player.bKeyboard[VK_F1] = false;					// If So Make Key FALSE
		glwWnd->KillGLWindow();						// Kill Our Current Window
		glwWnd->fullscreen = !glwWnd->fullscreen;				// Toggle Fullscreen / Windowed Mode
		glwWnd->bMousing = false;

		int resx = RESX;
		int resy = RESY;
		if (glwWnd->fullscreen)
		{
			resx = GetSystemMetrics(SM_CXSCREEN);
			resy = GetSystemMetrics(SM_CYSCREEN);
		}
		// Recreate Our OpenGL Window
		if(!glwWnd->CreateGLWindow("Blocks", resx, resy, BITS))
			//if(!glwWnd.CreateGLWindow("Blocks", 1366, 768, BITS))
		{
			return true;						// Quit If Window Was Not Created
		}
		wWorld.MaterialLib.LoadGLTextures();
	}

	*/
}

void Engine::GetFrameTime()
{
	static double koef = 0.025;
	static double max_FPS = 30;
	static int sleep_time;

	double currentTime = (double)timeGetTime() * koef;

	static double frameTime = currentTime;  // Время последнего кадра

	//Интервал времени, прошедшего с прошлого кадра
	FrameInterval = currentTime - frameTime;
	sleep_time = (int) (1000.0/max_FPS - FrameInterval/koef);
	if(sleep_time > 0) Sleep(sleep_time);

	frameTime = currentTime;
	//g_FrameInterval = 0.1;
}
