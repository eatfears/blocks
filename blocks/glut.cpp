#define FREEGLUT_STATIC
#include <GL/glut.h>

#include "Engine.h"

Engine *engine;


void Display(void)
{
	engine->Loop();
}

void Reshape(int width, int height)
{
	engine->Reshape(width, height);
}

void MouseButton(int button,int state,int x,int y)
{
//	switch(button)
//	{
// 	case GLUT_LEFT_BUTTON:
// 		if (state==GLUT_DOWN) glutIdleFunc(spinDisplay); break;
// 
// 	case GLUT_RIGHT_BUTTON: 
// 		if (state==GLUT_DOWN) glutIdleFunc(NULL); break;
//	}
}

void Keyboard(unsigned char button, int x, int y)
{
	engine->Keyboard(button, x, y, false);
}

void KeyboardUp(unsigned char button, int x, int y)
{
	engine->Keyboard(button, x, y, true);
}

void MouseEntry ( int state )
{
	if (state == GLUT_ENTERED){}
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


void MouseMotion(int x, int y)
{
	engine->MouseMotion(x, y);
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

	engine = new Engine();
	engine->Init();

// 	glutGameModeString("1280x1024:16@60"); //Переход в полноэкранный режим
// 	glutEnterGameMode(); //Собственно сам переход

	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);

	//glutSpecialFunc( void (* callback)( int, int, int ) );
	glutReshapeFunc(Reshape);
	glutVisibilityFunc(visible);
	glutDisplayFunc(Display);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutEntryFunc(MouseEntry);

	glutMainLoop();

	return 0;
}