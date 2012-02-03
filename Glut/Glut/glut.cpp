#define FREEGLUT_STATIC
#include <GL/glut.h>

GLfloat spin=0.0;
//int currentWindow;

double dPositionX = 10;
double dPositionY;
double dPositionZ;
double dSpinX;
double dSpinY;
bool mousing = false;
#define MOUSE_SENSIVITY 1.0;

void init(void)
{
	//glClearColor(0.0,0.0,0.0,0.0);
	//glShadeModel(GL_FLAT);

	glShadeModel(GL_SMOOTH);									// Разрешить плавное цветовое сглаживание
	glClearColor(0.0,0.0,0.0,0.0);								// Очистка экрана в черный цвет
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
}

void display(void)
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

void spinDisplay(void)
{
	spin=spin+0.10;

	if(spin>360.0) spin=spin-360.0;

	glutPostRedisplay();
}

void reshape(int width, int height)
{
	if( height == 0 )											// Предотвращение деления на ноль 
	{
		height = 1;
	}

	glViewport( 0, 0, width, height );							// Сброс текущей области вывода
	glMatrixMode( GL_PROJECTION );								// Выбор матрицы проекций
	glLoadIdentity();											// Сброс матрицы проекции

	//Вычисление соотношения геометрических размеров для окна
	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 10000);
	glMatrixMode( GL_MODELVIEW );								// Выбор матрицы вида модели
	glLoadIdentity();											// Сброс матрицы вида модели

	/*
	glViewport(0,0,(GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0,50.0,-50.0,50.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/
}

void mouse(int button,int state,int x,int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if (state==GLUT_DOWN) glutIdleFunc(spinDisplay); break;

	case GLUT_RIGHT_BUTTON: 
		if (state==GLUT_DOWN) glutIdleFunc(NULL); break;
	}
}

void keyboard( unsigned char c, int a, int b)
{
	switch(c)
	{
	case '1': 

		break;
	case VK_ESCAPE: exit(0);
		break;
	}
}

void mouseEntry ( int state )
{
	if (state == GLUT_ENTERED)
	{

	}
	else
	{

	}
}

void idle(void)
{
	//glutPostWindowRedisplay(currentWindow);
	glutPostRedisplay();
}

void visible(int vis)
{
	glutIdleFunc(idle);
}


void motion(int x,int y)
{
	static int Lastx, Lasty;

	if(mousing)
	{
		mousing = false;


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

		glutWarpPointer(120,120);

	}
	else 
	{
		mousing = true;
		Lastx = x;
		Lasty = y;
	}

// 	if(mousing)
// 	{
// 		glutWarpPointer(120,120);
// 		mousing = false;
// 	}
// 	else mousing = true;

}

int main(int argc, char **argv)
{

#ifdef WIN32 
	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);
#endif

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(250,250);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Glut");

	init();
	

// 	glutGameModeString("1280x1024:16@60"); //Переход в полноэкранный режим
// 	glutEnterGameMode(); //Собственно сам переход

	glutSetCursor(GLUT_CURSOR_NONE); //Выставляем на НЕТ КУРСОРА
	//currentWindow = glutGetWindow(); //Присваиваем currentWindow glutовский ID окна

	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard);

	//glutSpecialFunc( void (* callback)( int, int, int ) );
	glutReshapeFunc(reshape);
	glutVisibilityFunc(visible);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutEntryFunc(mouseEntry);

	glutMainLoop();

	return 0;
}