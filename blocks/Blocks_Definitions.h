#pragma once

#define TORAD(gfPosX) gfPosX*0.01745329251994329576923690768489
#define TODEG(gfPosX) gfPosX*57.295779513082320876798154814105

#define TILE_SIZE 10.0

#define STEP_DOWNSTEP	0.6
#define MAX_DOWNSTEP	7.0
#define PLAYER_HEIGHT	15.0
#define AIR_ACCEL		0.12
#define JUMP_STR		4.0
#define WALK_SPEED		1.5

#define SPRINT_KOEF		2.0

#define FOG_COLOR		0.5f,0.5f,0.8f,1.0f
#define FOG_DENSITY		10.0
#define FOG_START		TILE_SIZE*10
#define MAX_VIEV_DIST	TILE_SIZE*80

#define TOP			0
#define DOWN		1
#define RIGHT		2
#define LEFT		3
#define BACK		4
#define FRONT		5
