#pragma once

#include "common_include/freeglut_static.h"
#include "logger/logger.h"
#include "world.h"
#include "character.h"
#include "statistics.h"

class Engine
{
public:
    Engine();
    ~Engine();

    void initGL();
    void initGame();

    void reshape(GLsizei width, GLsizei height);
    void display();
    void keyboard(unsigned char button, int x, int y, bool KeyDown);
    void mouseMotion(int x, int y);
    void mouseButton(int button, int state, int x, int y);
    void special(int button, int x, int y, bool KeyDown);
    void loop();

private:
    int m_Width, m_Height;

    bool m_Fullscreen;
    bool m_Mousing;

    void drawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char side);
    void drawInterface();
    void drawSelectedItem();

    void drawBottomBorder();
    void drawSunMoon();
    void drawClouds();

    void openGL2d();
    void openGL3d();

    World m_World;

    double m_TimeOfDay;
    void getFogColor();
    GLfloat m_FogColor[4];

    void getFrameTime();
    double m_FrameInterval;
    friend class Statistics;
    Statistics m_StatWindow;

    DEFINE_LOGGER(ENGINE, logger)
};
