#include "Engine.h"
#include <time.h>
#include <Mmsystem.h>

GLfloat fogColor[4]= {FOG_COLOR};

Engine::Engine()
	:player(wWorld)
{
	bMousing = false;
	fullscreen = false;
}

Engine::~Engine()
{
}

int Engine::InitGL()
{
	wWorld.MaterialLib.AllocGLTextures();
	wWorld.MaterialLib.LoadGLTextures();

	player.dPositionY = 100*BLOCK_SIZE+00.0;
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

void Engine::Reshape(int width, int height)
{
	if(height == 0)											// Предотвращение деления на ноль 
		height = 1;

	glViewport(0, 0, width, height);							// Сброс текущей области вывода
	glMatrixMode(GL_PROJECTION);								// Выбор матрицы проекций
	glLoadIdentity();											// Сброс матрицы проекции

	//Вычисление соотношения геометрических размеров для окна
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, BLOCK_SIZE + MAX_VIEV_DIST);
	glMatrixMode(GL_MODELVIEW);								// Выбор матрицы вида модели
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Очистить экран и буфер глубины
	glLoadIdentity();											// Сбросить текущую матрицу

	glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
	glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	glTranslated(-player.dPositionX, -player.dPositionY, -player.dPositionZ);

	/*
	GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	
	glTranslated(0, 30, 0);
	GLfloat light2_diffuse[] = {0.9f, 0.9f, 0.9f};
	GLfloat light2_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0);
	glTranslated(0, -30, 0);
	*/
	//glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);		//Тип тумана
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, FOG_DENSITY);	//Насколько густым будет туман
	glHint(GL_FOG_HINT, GL_DONT_CARE);		//Вспомогательная установка тумана
	glFogf(GL_FOG_START, FOG_START);		//Глубина, с которой начинается туман
	glFogf(GL_FOG_END, MAX_VIEV_DIST);		//Глубина, где туман заканчивается

	glColor3d(1.0, 1.0, 1.0);

	static BlockInChunk x, y, z;
	static GLuint *tex = wWorld.MaterialLib.texture;

	glBindTexture(GL_TEXTURE_2D, tex[0]);


	GLenum mod = GL_EXECUTE;
	if(player.bKeyboard['Z'])
	{
		player.bKeyboard['Z'] = 0;
		mod = GL_COMPILE;
	}
	
	for (int bin = 0; bin < HASH_SIZE; bin++)
	{
		auto loc = wWorld.Chunks[bin].begin();
		while(loc != wWorld.Chunks[bin].end())
		{
#ifndef _DEBUG
			_try 
			{
#endif // _DEBUG

				(*loc)->Render(mod, MAT_NO);
				++loc;

#ifndef _DEBUG
			}
			_except (EXCEPTION_EXECUTE_HANDLER)
			{
				break;
			}
#endif // _DEBUG
		}
	}



	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//transparent tiles here

	for (int bin = 0; bin < HASH_SIZE; bin++)
	{
		auto loc = wWorld.Chunks[bin].begin();
		while(loc != wWorld.Chunks[bin].end())
		{
#ifndef _DEBUG
			_try 
			{
#endif // _DEBUG

				(*loc)->Render(mod, MAT_WATER);
				++loc;

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


	glDisable(GL_FOG);
	glDisable(GL_LIGHT2);


#ifdef DEBUG_OUT
	FILE *out;
	out = fopen(DEBUG_FILE, "w");
	fprintf(out,"Debug\n");
	fclose(out);
#endif

	// 
	// 	glClear(GL_COLOR_BUFFER_BIT);
	// 	glPushMatrix();
	// 	glRotatef(spin,0.0,0.0,1.0);
	// 	glColor3f(1.0,1.0,1.0);
	// 	glRectf(-25.0,-25.0,25.0,25.0);
	// 	glPopMatrix();
	// 	glutSwapBuffers();
}

void Engine::Keyboard(unsigned char button, int x, int y, bool KeyDown)
{
	switch(button)
	{
	case VK_ESCAPE: glutExit();
		break;
	default:
#ifdef _WIN32
		button = VkKeyScan(button);
#endif
		player.bKeyboard[button] = KeyDown;
		break;
	}
}

void Engine::MouseMotion(int x, int y)
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
#endif // _WIN32

	srand((unsigned int)time(NULL));
	int seed = rand();

	wWorld.lLandscape.Init(seed);
	wWorld.BuildWorld();
}

void Engine::DrawSelectedItem()
{
	if(!wWorld.FindBlock(player.sCenterBlockCoordX,player.sCenterBlockCoordY,player.sCenterBlockCoordZ))
		return;

	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3d(0.1, 0.1, 0.1);
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
	DrawSelectedItem();

	//Сбросить текущую матрицу
	glLoadIdentity();

	//Рисование прицела
	glTranslated(0, 0, -0.11);
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3d(1.0, 1.0, 1.0);
	glLineWidth (2.0);
	glBegin(GL_LINES);
	glVertex2d(0.0,-0.001);
	glVertex2d(0.0,0.001);

	glVertex2d(-0.001,0.0);
	glVertex2d(0.001,0.0);
	glEnd();
	glTranslated(0, 0, 0.11);
}

void Engine::Loop()
{

	Display();
	player.GetCenterCoords(width, height);

	player.Control(FrameInterval);
	DrawInterface();

	GetFrameTime();

	glutSwapBuffers();

/*
	if(player.bKeyboardPress[VK_F1])						// Is F1 Being Pressed?
	{
		player.bKeyboardPress[VK_F1] = false;					// If So Make Key FALSE
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
		player.bSpecial[button] = KeyDown;
	}
}
