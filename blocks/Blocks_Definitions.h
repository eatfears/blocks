#pragma once

#define FREEGLUT_STATIC

#define DEBUG_OUT
#undef DEBUG_OUT

#define DEBUG_FILE		"1.txt"

#define TORAD(ang)		ang*0.01745329251994329576923690768489
#define TODEG(ang)		ang*57.295779513082320876798154814105

#define RESX			856
#define RESY			482
#define BITS			32

#define BLOCK_SIZE		5.0

#define MOUSE_SENSIVITY 3.5

#define STEP_DOWNSTEP	0.6
#define MAX_DOWNSTEP	7.0
#define PLAYER_HEIGHT	15.0
#define AIR_ACCEL		1.2
#define JUMP_STR		4.0
#define WALK_SPEED		150

#define SPRINT_KOEF		2.0

#define FOG_DENSITY		6.0
#define FOG_START		CHUNK_SIZE_XZ*BLOCK_SIZE*4
#define MAX_VIEV_DIST	CHUNK_SIZE_XZ*BLOCK_SIZE*32
#define FARCUT			CHUNK_SIZE_XZ*BLOCK_SIZE*42

#define TOP				0
#define BOTTOM			1
#define RIGHT			2
#define LEFT			3
#define BACK			4
#define FRONT			5
#define SNOWCOVERED		6
#define GRASSCOVERED	7


#define CHUNK_SIZE_XZ	16
#define CHUNK_SIZE_Y	128

typedef unsigned short	BlockInChunk;
typedef signed short	ChunkInWorld;
typedef signed short	BlockInWorld;

#define GL_EXECUTE		0x1302
#define RENDER_NO_NEED	0
#define RENDER_NEED		1
#define RENDER_MAYBE	2

#define HASH_SIZE		0x1000

#define DAYLIGHT		15
