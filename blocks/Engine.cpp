#include "Engine.h"
#include <time.h>
#include <Mmsystem.h>

GLfloat fogColor[4]= {FOG_COLOR};

Engine::Engine()
	:player(wWorld)
{
	active = true;
	bMousing = false;
	fullscreen = false;
}

Engine::~Engine()
{
}

int Engine::Init()
{
	player.dPositionY = 100*BLOCK_SIZE+00.0;
	player.dSpinY = -90 - 45;
	glutSetCursor(GLUT_CURSOR_NONE);							//Выставляем на НЕТ КУРСОР


	glShadeModel(GL_SMOOTH);									// Разрешить плавное цветовое сглаживание
	glClearColor(FOG_COLOR);									// Очистка экрана в черный цвет
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

	return true;	
}

void Engine::Reshape(int width, int height)
{
	if(height == 0)											// Предотвращение деления на ноль 
		height = 1;

	glViewport(0, 0, width, height);							// Сброс текущей области вывода
	glMatrixMode(GL_PROJECTION);								// Выбор матрицы проекций
	glLoadIdentity();											// Сброс матрицы проекции

	//Вычисление соотношения геометрических размеров для окна
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, BLOCK_SIZE + MAX_VIEV_DIST);
	glMatrixMode(GL_MODELVIEW);								// Выбор матрицы вида модели
	glLoadIdentity();											// Сброс матрицы вида модели

	this->width = width;
	this->height = height;
	/*
	glViewport(0,0,(GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0,50.0,-50.0,50.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/
}

void Engine::Display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Очистить экран и буфер глубины
	glLoadIdentity();											// Сбросить текущую матрицу

	glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
	glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	glTranslated(-player.dPositionX, -player.dPositionY, -player.dPositionZ);
	

	/*
	GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	
	glTranslated(0, 30, 0);
	GLfloat light2_diffuse[] = {0.9f, 0.9f, 0.9f};
	GLfloat light2_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0);
	glTranslated(0, -30, 0);
	*/
	//glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);		//Тип тумана
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, FOG_DENSITY);	//Насколько густым будет туман
	glHint(GL_FOG_HINT, GL_DONT_CARE);		//Вспомогательная установка тумана
	glFogf(GL_FOG_START, FOG_START);		//Глубина, с которой начинается туман
	glFogf(GL_FOG_END, MAX_VIEV_DIST);		//Глубина, где туман заканчивается

	GLdouble dBrightness = 1.0;
	glColor3d(dBrightness, dBrightness, dBrightness);

	BlockInChunk x, y, z;
	GLuint *tex = wWorld.MaterialLib.texture;


	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glBegin(GL_QUADS);

	auto loc = wWorld.Chunks.begin();

	for(int i = 0; i < 6; i++)
	{
		loc = wWorld.Chunks.begin();
#ifndef _DEBUG
		_try 
		{
#endif // _DEBUG

			while(loc != wWorld.Chunks.end())
			{

				auto it = loc->DisplayedTiles[i].begin();

				while(it != loc->DisplayedTiles[i].end())
				{
#ifndef _DEBUG
					_try 
					{
#endif // _DEBUG

						loc->GetBlockPositionByPointer(*it, &x, &y, &z);


						GLdouble br;
						BlockInWorld	
							xx = x + loc->x*CHUNK_SIZE_XZ,
							yy = y,
							zz = z + loc->z*CHUNK_SIZE_XZ;
						BlockInWorld
							xlight = x, ylight = y, zlight = z;
						BlockInChunk 
							xloclight, 
							yloclight, 
							zloclight;

						switch(i)
						{
						case TOP:		ylight++; break;
						case BOTTOM:	ylight--; break;
						case RIGHT:		xlight++; break;
						case LEFT:		xlight--; break;
						case FRONT:		zlight--; break;
						case BACK:		zlight++; break;
						}
						Chunk *temploc;
						if((xlight >= CHUNK_SIZE_XZ)||(xlight < 0)||(zlight >= CHUNK_SIZE_XZ)||(zlight < 0))
							temploc = wWorld.GetChunkByBlock(xlight + loc->x*CHUNK_SIZE_XZ, zlight + loc->z*CHUNK_SIZE_XZ);
						else temploc = &*loc;
						if(temploc)
						{
							wWorld.GetPosInChunkByWorld(xlight, ylight, zlight, &xloclight, &yloclight, &zloclight);
							int index = temploc->GetIndexByPosition(xloclight, yloclight, zloclight);
							//wWorld.lLocations.begin()->GetIndexByPosition(sXcoord, sXcoord, sXcoord);

							//	loc->GetIndexByPosition(sXcoord, sYcoord+1, sZcoord);
							br = 0.2 + temploc->SkyLight[index];
							glColor3d(br, br, br);
						}

						if(	(abs(xx*BLOCK_SIZE - player.dPositionX) < MAX_VIEV_DIST + 10*BLOCK_SIZE) && 
							(abs(yy*BLOCK_SIZE - player.dPositionY) < MAX_VIEV_DIST + 10*BLOCK_SIZE) && 
							(abs(zz*BLOCK_SIZE - player.dPositionZ) < MAX_VIEV_DIST + 10*BLOCK_SIZE))
							DrawTile(xx, yy, zz, (*it)->cMaterial, i);

						++it;
#ifndef _DEBUG
					}
					_except (EXCEPTION_EXECUTE_HANDLER)
					{
						break;
					}
#endif // _DEBUG
				}
				++loc;
			}
#ifndef _DEBUG
		}
		_except (EXCEPTION_EXECUTE_HANDLER)
		{
			glEnd();
			glDisable(GL_FOG);
			glDisable(GL_LIGHT2);
			return;
		}
#endif // _DEBUG
	}

#ifdef DEBUG_OUT
	FILE *out;
	out = fopen(DEBUG_FILE, "w");
	fprintf(out,"Debug\n");
	fclose(out);
#endif

	glEnd();


	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//transparent tiles here


	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);


	glDisable(GL_FOG);
	glDisable(GL_LIGHT2);


	// 
	// 	glClear(GL_COLOR_BUFFER_BIT);
	// 	glPushMatrix();
	// 	glRotatef(spin,0.0,0.0,1.0);
	// 	glColor3f(1.0,1.0,1.0);
	// 	glRectf(-25.0,-25.0,25.0,25.0);
	// 	glPopMatrix();
	// 	glutSwapBuffers();
}

void Engine::Keyboard(unsigned char button, int x, int y, bool KeyDown)
{
	switch(button)
	{
	case VK_ESCAPE: exit(0);
		break;
	default:
		{
			player.bKeyboardPress[button] = KeyDown;
			if(!KeyDown) player.bKeyboardHit[button] = true;
		}
		break;
	}
}

void Engine::MouseMotion(int x, int y)
{
	static int Lastx, Lasty;

	if(bMousing)
	{
		bMousing = false;
		{
			player.dSpinY -= (x - Lastx)/MOUSE_SENSIVITY;
			player.dSpinX -= (y - Lasty)/MOUSE_SENSIVITY;
		}

		while(player.dSpinY >= 360.0)
			player.dSpinY -= 360.0;

		while(player.dSpinY < 0.0)
			player.dSpinY += 360.0;

		if(player.dSpinX < -90.0) player.dSpinX = -90.0;
		if(player.dSpinX > 90.0) player.dSpinX = 90.0;

		Lastx = x;
		Lasty = y;

		glutWarpPointer(width/2,height/2);

	}
	else 
	{
		bMousing = true;
		Lastx = x;
		Lasty = y;
	}
}

void Engine::MouseButton(int button, int state, int x, int y)
{
// 	switch(button)
// 	{
// 	case GLUT_LEFT_BUTTON:
// 		if (state==GLUT_DOWN) glutIdleFunc(spinDisplay); break;
// 
// 	case GLUT_RIGHT_BUTTON: 
// 		if (state==GLUT_DOWN) glutIdleFunc(NULL); break;
// 	}
}

void Engine::InitGame()// GLWindow *glwWnd)
{
	//HANDLE threadHandle = GetCurrentThread();
	//SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);

	//this->glwWnd = glwWnd;
	srand((unsigned int)time(NULL));
	int seed = rand();


	wWorld.MaterialLib.AllocGLTextures();
	wWorld.MaterialLib.LoadGLTextures();
	wWorld.lLandscape.Init(seed);
	wWorld.BuildWorld();
}

void Engine::DrawTile(BlockInWorld sXcoord, BlockInWorld sYcoord, BlockInWorld sZcoord, int material, char N)
{
	GLdouble 
		// 		dXcoord = (sXcoord-player.lnwPositionX*LOCATION_SIZE_XZ)*TILE_SIZE, 
		// 		dYcoord = sYcoord*TILE_SIZE, 
		// 		dZcoord = (sZcoord-player.lnwPositionZ*LOCATION_SIZE_XZ)*TILE_SIZE;
		dXcoord = sXcoord*BLOCK_SIZE, 
		dYcoord = sYcoord*BLOCK_SIZE, 
		dZcoord = sZcoord*BLOCK_SIZE;

	dXcoord -= BLOCK_SIZE/2;
	dZcoord -= BLOCK_SIZE/2;

	static double space = 0.0005;
	static double offsetx;
	static double offsety;

	wWorld.MaterialLib.GetTextureOffsets(offsetx, offsety, material, N);

	switch(N)
	{
	case TOP:
		{
			//Верхняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BOTTOM:
		{
			//Нижняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case RIGHT:
		{
			//Правая грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
		}break;
	case LEFT:
		{
			//Левая грань
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
		}break;
	case BACK:
		{
			//Задняя грань
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord + BLOCK_SIZE);
		}break;
	case FRONT:
		{
			//Передняя грань
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord + BLOCK_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + BLOCK_SIZE, dYcoord, dZcoord);
		}break;
	}
}

void Engine::DrawSelectedItem()
{
	if(!wWorld.FindBlock(player.sCenterBlockCoordX,player.sCenterBlockCoordY,player.sCenterBlockCoordZ))
		return;

	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3d(0.1, 0.1, 0.1);
	glLineWidth (1.4f);

	GLdouble BorderSize = BLOCK_SIZE*(1 + 0.005);
	GLdouble 
		dXcoord = player.sCenterBlockCoordX*BLOCK_SIZE, 
		dYcoord = player.sCenterBlockCoordY*BLOCK_SIZE, 
		dZcoord = player.sCenterBlockCoordZ*BLOCK_SIZE;

	dXcoord -= BorderSize/2;
	dZcoord -= BorderSize/2;

	glBegin(GL_QUADS);


	//Верхняя грань
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
	//Нижняя грань
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
	//Правая грань
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
	//Левая грань
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
	//Задняя грань
	glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
	//Передняя грань
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);

	glEnd();

	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void Engine::DrawInterface()
{
	DrawSelectedItem();

	//Сбросить текущую матрицу
	glLoadIdentity();

	//Рисование прицела
	glTranslated(0, 0, -0.1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3d(1.0, 1.0, 1.0);
	glLineWidth (2.0);
	glBegin(GL_LINES);
	glVertex2d(0.0,-0.001);
	glVertex2d(0.0,0.001);

	glVertex2d(-0.001,0.0);
	glVertex2d(0.001,0.0);
	glEnd();
	glTranslated(0, 0, 0.1);
}

void Engine::Loop()
{

	Display();
	player.GetCenterCoords(width, height);

	player.Control(FrameInterval);
	DrawInterface();

	GetFrameTime();

	glutSwapBuffers();

/*
	if(player.bKeyboardPress[VK_F1])						// Is F1 Being Pressed?
	{
		player.bKeyboardPress[VK_F1] = false;					// If So Make Key FALSE
		glwWnd->KillGLWindow();						// Kill Our Current Window
		glwWnd->fullscreen = !glwWnd->fullscreen;				// Toggle Fullscreen / Windowed Mode
		glwWnd->bMousing = false;

		int resx = RESX;
		int resy = RESY;
		if (glwWnd->fullscreen)
		{
			resx = GetSystemMetrics(SM_CXSCREEN);
			resy = GetSystemMetrics(SM_CYSCREEN);
		}
		// Recreate Our OpenGL Window
		if(!glwWnd->CreateGLWindow("Blocks", resx, resy, BITS))
			//if(!glwWnd.CreateGLWindow("Blocks", 1366, 768, BITS))
		{
			return true;						// Quit If Window Was Not Created
		}
		wWorld.MaterialLib.LoadGLTextures();
	}

	*/
}

void Engine::GetFrameTime()
{
	static double koef = 0.025;
	static double max_FPS = 30;
	static int sleep_time;

	double currentTime = (double)timeGetTime() * koef;

	static double frameTime = currentTime;  // Время последнего кадра

	//Интервал времени, прошедшего с прошлого кадра
	FrameInterval = currentTime - frameTime;
	sleep_time = (int) (1000.0/max_FPS - FrameInterval/koef);
	if(sleep_time > 0) Sleep(sleep_time);

	frameTime = currentTime;
	//g_FrameInterval = 0.1;
}
