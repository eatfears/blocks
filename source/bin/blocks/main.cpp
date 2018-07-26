#include "common_include/freeglut_static.h"
#include "engine/engine.h"
#include "logger/logger.h"

Engine *engine;
DEFINE_LOGGER(BLOCKS, logger)

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

void enableMultisample(bool msaa)
{
    if (msaa)
    {
        //glEnable(GL_MULTISAMPLE);
        glEnable(GL_MULTISAMPLE_ARB);
        glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
        glutSetOption(GLUT_MULTISAMPLE, 8);

        // detect current settings
        GLint iMultiSample = 0;
        GLint iNumSamples = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
        glGetIntegerv(GL_SAMPLES, &iNumSamples);
        logger.info() << "MSAA on, GL_SAMPLE_BUFFERS =" << iMultiSample << ", GL_SAMPLES =" << iNumSamples;
    }
    else
    {
        //glDisable(GL_MULTISAMPLE);
        glDisable(GL_MULTISAMPLE_ARB);
        logger.info() << "MSAA off";
    }
}

#include <iostream>

std::string readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}


GLuint LoadShader(const char *vertex_path, const char *fragment_path)
{
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shaders
    std::string vert_shader_str = readFile(vertex_path);
    std::string frag_shader_str = readFile(fragment_path);
    const char *vert_shader_src = vert_shader_str.c_str();
    const char *frag_shader_src = frag_shader_str.c_str();

    GLint result = GL_FALSE;
    int log_length;

    // Compile vertex shader
    std::cout << "Compiling vertex shader." << std::endl;
    glShaderSource(vert_shader, 1, &vert_shader_src, NULL);
    glCompileShader(vert_shader);

    // Check vertex shader
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> vertShaderError((log_length > 1) ? log_length : 1);
    glGetShaderInfoLog(vert_shader, log_length, NULL, &vertShaderError[0]);
    std::cout << &vertShaderError[0] << std::endl;

    // Compile fragment shader
    std::cout << "Compiling fragment shader." << std::endl;
    glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
    glCompileShader(frag_shader);

    // Check fragment shader
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> fragShaderError((log_length > 1) ? log_length : 1);
    glGetShaderInfoLog(frag_shader, log_length, NULL, &fragShaderError[0]);
    std::cout << &fragShaderError[0] << std::endl;

    std::cout << "Linking program" << std::endl;
    GLuint program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> programError( (log_length > 1) ? log_length : 1 );
    glGetProgramInfoLog(program, log_length, NULL, &programError[0]);
    std::cout << &programError[0] << std::endl;

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return program;
}

int main(int argc, char **argv)
{
    logger.info() << "Launching";

    engine = new Engine();

    glutInit(&argc, argv);
    glutInitDisplayMode(/*GLUT_ALPHA |*/ GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(RESX, RESY);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Blocks");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        logger.error() << err;
        delete engine;
        LOGGER_END;
        exit(1);
    }

    GLuint program = LoadShader("./shaders/shader.vert", "./shaders/shader.frag");
    glUseProgram(program);
//    glutExit();
//    exit(0);

    enableMultisample(false);

    bool gamemode = false;
    if (gamemode)
    {
        glutGameModeString("1920x1080:32");
        if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
        {
            glutEnterGameMode();
        }
        else
        {
            logger.error() << "Game mode is not possible";
            exit(1);
        }
    }

    engine->initGL();
    engine->initGame();

    GlutInit();
    glutMainLoop();

    delete engine;
    logger.info() << "Exit";
    LOGGER_END;

    return 0;
}
