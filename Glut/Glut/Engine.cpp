#include "Engine.h"
#include "Blocks_Definitions.h"
#include <time.h>


Engine::Engine(void)
{
	srand((unsigned int)time(NULL));
	int seed = rand();
	randNumGen = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(randNumGen, seed);

	active = true;
	bMousing = false;
	fullscreen = false;

	dPositionX = 10;
}


Engine::~Engine(void)
{
}

int Engine::Init()
{
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

	glRotated( -dSpinX, 1.0, 0.0, 0.0 );
	glRotated( -dSpinY, 0.0, 1.0, 0.0 );
	glTranslated(-dPositionX, -dPositionY, -dPositionZ);


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


	glutSwapBuffers();


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
			dSpinY -= (x - Lastx)/MOUSE_SENSIVITY;
			dSpinX -= (y - Lasty)/MOUSE_SENSIVITY;
		}

		while(dSpinY >= 360.0)
			dSpinY -= 360.0;

		while(dSpinY < 0.0)
			dSpinY += 360.0;

		if(dSpinX < -90.0) dSpinX = -90.0;
		if(dSpinX > 90.0) dSpinX = 90.0;

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
