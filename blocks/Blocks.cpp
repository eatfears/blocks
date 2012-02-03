#include "Blocks_Definitions.h"

#include <GL/glut.h>

#include "Engine.h"

Engine *engine;

void Display(void) { engine->Loop(); }
void Reshape(int width, int height) { engine->Reshape(width, height); }
void MouseMotion(int x, int y) { engine->MouseMotion(x, y); }
void MouseButton(int button,int state,int x,int y) { engine->MouseButton(button, state, x, y); }
void KeyboardDown(unsigned char button, int x, int y) { engine->Keyboard(button, x, y, true); }
void KeyboardUp(unsigned char button, int x, int y) { engine->Keyboard(button, x, y, false); }
void MouseEntry (int state) { if (state == GLUT_ENTERED){} }

void idle(void) { glutPostRedisplay(); }
void visible(int vis) { glutIdleFunc(idle); }

int main(int argc, char **argv)
{

#ifdef WIN32 
	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);
#endif

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(RESX, RESY);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Blocks");

	engine = new Engine();
	engine->Init();
	engine->InitGame();

// 	glutGameModeString("1280x1024:16@60"); //Переход в полноэкранный режим
// 	glutEnterGameMode(); //Собственно сам переход

	glutKeyboardFunc(KeyboardDown);
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