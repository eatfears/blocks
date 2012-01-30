#include "Game.h"
#include "Blocks_Definitions.h"

GLfloat fogColor[4]= {FOG_COLOR};

Game::Game()
	:player(wWorld)
{
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

// ����� ����� ����������� ��� ����������
int Game::DrawGLScene()   
{
	glEnable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// �������� ����� � ����� �������
	glLoadIdentity();											// �������� ������� �������

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
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);		//��� ������
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, FOG_DENSITY);	//��������� ������ ����� �����
	glHint(GL_FOG_HINT, GL_DONT_CARE);		//��������������� ��������� ������
	glFogf(GL_FOG_START, FOG_START);		//�������, � ������� ���������� �����
	glFogf(GL_FOG_END, MAX_VIEV_DIST);		//�������, ��� ����� �������������

	GLdouble dBrightness = 1.0;
	glColor3d(dBrightness, dBrightness, dBrightness);


	int iCurrentTexture = -1;
	static Tile *tile;

	auto loc = wWorld.lLocations.begin();
	TileInLoc x, y, z;
	GLuint *tex = wWorld.MaterialLib.texture;


	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glBegin(GL_QUADS);

	for(int i = 0; i < 6; i++)
	{
		loc = wWorld.lLocations.begin();
#ifndef _DEBUG
		_try 
		{
#endif // _DEBUG

			while(loc != wWorld.lLocations.end())
			{

				auto it = loc->DisplayedTiles[i].begin();

				while(it != loc->DisplayedTiles[i].end())
				{
#ifndef _DEBUG
					_try 
					{
#endif // _DEBUG

						loc->GetTilePositionByPointer(*it, &x, &y, &z);
						if(	(abs((x + (loc->x/*-player.lnwPositionX*/)*LOCATION_SIZE_XZ)*TILE_SIZE - player.dPositionX) < MAX_VIEV_DIST + 10*TILE_SIZE) && 
							(abs(y*TILE_SIZE - player.dPositionY) < MAX_VIEV_DIST + 10*TILE_SIZE) && 
							(abs((z + (loc->z/*-player.lnwPositionZ*/)*LOCATION_SIZE_XZ)*TILE_SIZE - player.dPositionZ) < MAX_VIEV_DIST + 10*TILE_SIZE))
							DrawTileSide(x + loc->x*LOCATION_SIZE_XZ, y, z + loc->z*LOCATION_SIZE_XZ, (*it)->cMaterial, i);

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
			glDisable(GL_LIGHT2);
			return true;
		}
#endif // _DEBUG
	}

	//transparent here

#ifdef DEBUG_OUT
	FILE *out;
	out = fopen(DEBUG_FILE, "w");
	fprintf(out,"Debug\n");
	fclose(out);
#endif

	glEnd();

	glDepthMask(false);

	glDepthMask(true);


	glDisable(GL_LIGHT2);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_FOG);

	return true;
}


void Game::DrawInterface()
{

	DrawSelectedItem();
	
	//�������� ������� �������
	glLoadIdentity();

	//��������� �������


	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_COLOR); 

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

	glDisable(GL_BLEND);
}

//void Game::DrawVisibleTileSide(Tile *tTile, char N)
void Game::DrawTileSide(signed short sXcoord, signed short sYcoord, signed short sZcoord, int material, char N)
{
	GLdouble 
// 		dXcoord = (sXcoord-player.lnwPositionX*LOCATION_SIZE_XZ)*TILE_SIZE, 
// 		dYcoord = sYcoord*TILE_SIZE, 
// 		dZcoord = (sZcoord-player.lnwPositionZ*LOCATION_SIZE_XZ)*TILE_SIZE;
		dXcoord = sXcoord*TILE_SIZE, 
		dYcoord = sYcoord*TILE_SIZE, 
		dZcoord = sZcoord*TILE_SIZE;

	dXcoord -= TILE_SIZE/2;
	dZcoord -= TILE_SIZE/2;

	static double space = 0.0005;
	static double offsetx;
	static double offsety;

	double br;

	br = (rand()%100)/100.0;
	//glColor3d(br, br, br);

	wWorld.MaterialLib.GetTextureOffsets(offsetx, offsety, material, N);

	switch(N)
	{
	case TOP:
		{
			//������� �����
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case DOWN:
		{
			//������ �����
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
		}break;
	case RIGHT:
		{
			//������ �����
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
		}break;
	case LEFT:
		{
			//����� �����
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case BACK:
		{
			//������ �����
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
		}break;
	case FRONT:
		{
			//�������� �����
			glTexCoord2d(0.0625 - space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2d(0.0625 - space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0 + space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2d(0.0 + space + offsetx, 0.0625 - space + offsety);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
		}break;
	}
}

void Game::DrawSelectedItem()
{
	if(!wWorld.FindTile(player.sCenterCubeCoordX,player.sCenterCubeCoordY,player.sCenterCubeCoordZ))
		return;

	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3d(0.1, 0.1, 0.1);
	glLineWidth (1.4f);

	GLdouble BorderSize = TILE_SIZE*(1 + 0.005);
	GLdouble 
		dXcoord = player.sCenterCubeCoordX*TILE_SIZE, 
		dYcoord = player.sCenterCubeCoordY*TILE_SIZE, 
		dZcoord = player.sCenterCubeCoordZ*TILE_SIZE;

	dXcoord -= BorderSize/2;
	dZcoord -= BorderSize/2;

	glBegin(GL_QUADS);


	//������� �����
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
	//������ �����
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
	//������ �����
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
	//����� �����
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
	//������ �����
	glVertex3d (dXcoord, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord + BorderSize);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord + BorderSize);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord + BorderSize);
	//�������� �����
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord + BorderSize, dZcoord);
	glVertex3d (dXcoord + BorderSize, dYcoord, dZcoord);

	glEnd();

	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}


void Game::GetFrameTime()
{
	static double koef = 0.025;
	static double max_FPS = 30;
	static int sleep_time;
	
	double currentTime = (double)timeGetTime() * koef;

	static double frameTime = currentTime;  // ����� ���������� �����

	//�������� �������, ���������� � �������� �����
	FrameInterval = currentTime - frameTime;
	sleep_time = (int) (1000.0/max_FPS - FrameInterval/koef);
	if(sleep_time > 0) Sleep(sleep_time);

	frameTime = currentTime;
	//g_FrameInterval = 0.1;

}

bool Game::Loop()
{

	if(player.bKeyboard[VK_ESCAPE])			// ���� �� ������ ������� ESC?
		return true;			// ESC ������� �� �������� ���������� ���������

	DrawGLScene();
	player.GetCenterCoords(glwWnd->width, glwWnd->height);

	player.Control(FrameInterval);
	DrawInterface();

	GetFrameTime();

	SwapBuffers(glwWnd->hDC);		// ������ ����� (������� �����������)

	if(player.bKeyboard[VK_F1])						// Is F1 Being Pressed?
	{
		player.bKeyboard[VK_F1] = false;					// If So Make Key FALSE
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

	return false;
}
