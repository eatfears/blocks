#include "Blocks_Definitions.h"
#include <GL/freeglut.h>
#include "Engine.h"

#pragma comment (lib, "libpng15.lib")
#pragma comment (lib, "zlib.lib")

Engine *engine;

void Display() { engine->Loop(); }
void Reshape(int width, int height) { engine->Reshape(width, height); }
void SpecialDown(int button, int x, int y) { engine->Special(button, x, y, true); }
void SpecialUp(int button, int x, int y) { engine->Special(button, x, y, false); }
void MouseMotion(int x, int y) { engine->MouseMotion(x, y); }
void MouseButton(int button, int state, int x, int y) { engine->MouseButton(button, state, x, y); }
void KeyboardDown(unsigned char button, int x, int y) { engine->Keyboard(button, x, y, true); }
void KeyboardUp(unsigned char button, int x, int y) { engine->Keyboard(button, x, y, false); }
void MouseEntry (int state) { if (state == GLUT_ENTERED){} }

// void idle() { glutPostRedisplay(); }
// void visible(int vis) { glutIdleFunc(idle); }

void GlutInit()
{
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF) ;

	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(SpecialDown);
	glutSpecialUpFunc(SpecialUp);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	glutReshapeFunc(Reshape);
	glutEntryFunc(MouseEntry);
	//glutVisibilityFunc(visible);
}

int main(int argc, char **argv)
{
	engine = new Engine();

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(RESX, RESY);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Blocks");

	/*
	glutGameModeString("1280x1024:32");
	//glutGameModeString("1280x1024:16@60"); //Переход в полноэкранный режим

	if(glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
		glutEnterGameMode();
	else
		exit(1);
	*/

	engine->InitGL();
	engine->InitGame();

	GlutInit();
	glutMainLoop();

	return 0;
}
