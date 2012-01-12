#include "Game.h"
#include "Blocks_Definitions.h"

#include "Mutex.h"
extern Mutex VisibleListAccessMutex;

GLfloat fogColor[4]= {FOG_COLOR};

Game::Game()
{
	player.wWorld = &wWorld;
}

Game::~Game()
{
}

void Game::InitGame(GLWindow *glwWnd)
{
	HANDLE threadHandle = GetCurrentThread();
	SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);

	this->glwWnd = glwWnd;
	wWorld.MaterialLib.AllocGLTextures();
	wWorld.MaterialLib.LoadGLTextures();
	wWorld.BuildWorld();
}

// Здесь будет происходить вся прорисовка
int Game::DrawGLScene()   
{
	glEnable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// Очистить экран и буфер глубины
	glLoadIdentity();											// Сбросить текущую матрицу

	glRotated( -player.dSpinX, 1.0, 0.0, 0.0 );
	glRotated( -player.dSpinY, 0.0, 1.0, 0.0 );
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

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);		//Тип тумана
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, FOG_DENSITY);	//Насколько густым будет туман
	glHint(GL_FOG_HINT, GL_DONT_CARE);		//Вспомогательная установка тумана
	glFogf(GL_FOG_START, FOG_START);		//Глубина, с которой начинается туман
	glFogf(GL_FOG_END, MAX_VIEV_DIST);		//Глубина, где туман заканчивается

	GLdouble dBrightness = 1.0;
	glColor3d(dBrightness, dBrightness, dBrightness);


	static int iCurrentTexture = 0;
	static Tile tile;

	glBegin(GL_QUADS);
	//int iTextureChangingNum = 0;

	for( int i = 0; i < 6; i++)
	{
		auto it = begin(wWorld.DisplayedTiles[i]);
		GLuint *tex = wWorld.MaterialLib.texture;

		while(it != wWorld.DisplayedTiles[i].end())
		{
			_try 
			{
				tile = *(*it);
				++it;
			}
			_except (EXCEPTION_EXECUTE_HANDLER)
			{
				glEnd();

				glDisable(GL_LIGHT2);

				return true;
			}

			if(	(abs(tile.sCoordX*TILE_SIZE - player.dPositionX) < MAX_VIEV_DIST + 10*TILE_SIZE) && 
				(abs(tile.sCoordY*TILE_SIZE - player.dPositionY) < MAX_VIEV_DIST + 10*TILE_SIZE) && 
				(abs(tile.sCoordZ*TILE_SIZE - player.dPositionZ) < MAX_VIEV_DIST + 10*TILE_SIZE))
			{
				if(iCurrentTexture != wWorld.MaterialLib.mMaterial[tile.cMaterial].iTexture[i])
				{
					//iTextureChangingNum++;
					iCurrentTexture = wWorld.MaterialLib.mMaterial[tile.cMaterial].iTexture[i];
					glEnd();
					glBindTexture(GL_TEXTURE_2D, tex[iCurrentTexture]);
					glBegin(GL_QUADS);
				}
				DrawVisibleTileSide(&tile, i);
			}
		}
	}

//-------------------------
// 	FILE *out;
// 	out = fopen("1.txt", "w");
// 	fprintf(out,"changing tex num: %d", iTextureChangingNum);
// 	fclose(out);
//-------------------------

	glEnd();

	glDisable(GL_LIGHT2);

	return true;
}


void Game::DrawInterface()
{
	//Выделение куба
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3d(0.1, 0.1, 0.1);
	glLineWidth (2.0);
	glBegin(GL_QUADS);
	if (Tile *temp = wWorld.FindTile(player.sCenterCubeCoordX,player.sCenterCubeCoordY,player.sCenterCubeCoordZ))
		for(int i = 0; i < 6; i++)
			DrawVisibleTileSide(temp,i);
	glEnd();
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

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

void Game::DrawVisibleTileSide(Tile *tTile, char N)
{
	GLdouble dXcoord = tTile->sCoordX*TILE_SIZE, dYcoord = tTile->sCoordY*TILE_SIZE, dZcoord = tTile->sCoordZ*TILE_SIZE;

	dXcoord -= TILE_SIZE/2;
	dZcoord -= TILE_SIZE/2;

	switch(N)
	{
	case TOP:
		{	
			//Верхняя грань
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case DOWN:
		{
			//Нижняя грань
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
		}break;
	case RIGHT:
		{
			//Правая грань
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case LEFT:
		{
			//Левая грань
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case BACK:
		{
			//Задняя грань
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
		}break;
	case FRONT:
		{
			//Передняя грань
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
		}break;
	}
}

void Game::GetFrameTime()
{
	double currentTime = (double)timeGetTime() * 0.025;

	static double frameTime = currentTime;  // Время последнего кадра

	//Интервал времени, прошедшего с прошлого кадра
	FrameInterval = currentTime - frameTime;

	frameTime = currentTime;
	//g_FrameInterval = 0.1;
}

bool Game::Loop()
{

	if(player.bKeyboard[VK_ESCAPE])			// Было ли нажата клавиша ESC?
		return true;			// ESC говорит об останове выполнения программы

	DrawGLScene();
	player.GetCenterCoords(glwWnd->width, glwWnd->height);

	player.Control(FrameInterval, wWorld);
	DrawInterface();

	GetFrameTime();

	SwapBuffers(glwWnd->hDC);		// Меняем буфер (двойная буферизация)

	if(player.bKeyboard[VK_F1])						// Is F1 Being Pressed?
	{
		player.bKeyboard[VK_F1] = false;					// If So Make Key FALSE
		glwWnd->KillGLWindow();						// Kill Our Current Window
		glwWnd->fullscreen = !glwWnd->fullscreen;				// Toggle Fullscreen / Windowed Mode
		glwWnd->bMousing = false;
		
		// Recreate Our OpenGL Window
		if(!glwWnd->CreateGLWindow("Blocks", RESX, RESY, BITS))
		//if(!glwWnd.CreateGLWindow("Blocks", 1366, 768, BITS))
		{
			return true;						// Quit If Window Was Not Created
		}
		wWorld.MaterialLib.LoadGLTextures();
	}

	return false;
}