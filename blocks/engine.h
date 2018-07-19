#pragma once

#include "definitions.h"
#include <GL/freeglut.h>

#include "world.h"
#include "character.h"
#include "statistics.h"

class Engine
{
public:
    Engine();
    ~Engine();

    int initGL();
    void reshape(GLsizei width, GLsizei height);
    void display();
    void keyboard(unsigned char button, int x, int y, bool KeyDown);
    void mouseMotion(int x, int y);
    void mouseButton(int button, int state, int x, int y);
    void special(int button, int x, int y, bool KeyDown);
    void loop();

    void initGame();

private:
    int width, height;

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
    void GetFogColor();
    GLfloat FogColor[4];

    void GetFrameTime();
    double FrameInterval;
    friend class Statistics;
    Statistics stat;
};
