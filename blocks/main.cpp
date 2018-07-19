#include "Definitions.h"
#include <GL/freeglut.h>
#include "Engine.h"

#pragma comment (lib, "libpng15.lib")
#pragma comment (lib, "zlib.lib")

Engine *engine;

void Display() { engine->loop(); }
void Reshape(int width, int height) { engine->reshape(width, height); }
void SpecialDown(int button, int x, int y) { engine->special(button, x, y, true); }
void SpecialUp(int button, int x, int y) { engine->special(button, x, y, false); }
void MouseMotion(int x, int y) { engine->mouseMotion(x, y); }
void MouseButton(int button, int state, int x, int y) { engine->mouseButton(button, state, x, y); }
void KeyboardDown(unsigned char button, int x, int y) { engine->keyboard(button, x, y, true); }
void KeyboardUp(unsigned char button, int x, int y) { engine->keyboard(button, x, y, false); }
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

    engine->initGL();
    engine->initGame();

    GlutInit();
    glutMainLoop();

    glutExit();

    return 0;
}
