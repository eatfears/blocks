#include "GLWindow.h"

LRESULT  CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

#define TORAD(gfPosX) gfPosX*0.01745329251994329576923690768489
#define TODEG(gfPosX) gfPosX*57.295779513082320876798154814105

#define TILE_SIZE 10.0

#define STEP_DOWNSTEP	0.5
#define MAX_DOWNSTEP	7.0
#define MAX_SPEED		3.0
#define PLAYER_HEIGHT	15.0
#define AIR_ACCEL		0.05
#define JUMP_STR		3.2
#define WALK_SPEED		1.5


GLWindow::GLWindow(void)
{	
	active = true;
	bMousing = false;
	player.falling = true;

	tTiles = new Tiles[0x100000];
	
	visible = new std::deque<Tile *>[6];
	/*
	AddTile(0,-1,0,MATERIAL_YES);
	AddTile(3,-1,0,MATERIAL_YES);
	AddTile(2,-1,0,MATERIAL_YES);
	AddTile(1,-1,0,MATERIAL_YES);
	AddTile(0,-1,1,MATERIAL_YES);
	AddTile(0,-1,2,MATERIAL_YES);
	AddTile(0,-1,3,MATERIAL_YES);

	/*
	AddTile(10,0,0,MATERIAL_YES);
	AddTile(0,1,0,MATERIAL_YES);
	AddTile(0,-3,0,MATERIAL_YES);
	AddTile(1,0,0,MATERIAL_YES);
	AddTile(-1,0,0,MATERIAL_YES);
	/**/
	
	building = true;

	for (int i = 0 ; i< 10 ; i++)
	{
		for (int k = 0 ; k < 1 ; k++)
		{
			AddTile(rand()%100-50,k,rand()%100-50,MATERIAL_YES);
		}
	}

	for (int j = 1; j <= 100; j++)
	{
		for (int i = 0; i < 1; i++)
		{
			for (int k = 0; k < 200; k++)
			{

				AddTile(i-0,-j,k-100,MATERIAL_YES);
			}
		}
	}

	/**/
	for (int i = 0; i < 0x100000; i++)
	{
		auto it = begin(tTiles[i]);

		while(it < tTiles[i].end())
		{
			if(!FindTile(it->x, it->y + 1, it->z)) visible[0].push_front(&*it);
			if(!FindTile(it->x, it->y - 1, it->z)) visible[1].push_front(&*it);
			if(!FindTile(it->x + 1, it->y, it->z)) visible[2].push_front(&*it);
			if(!FindTile(it->x - 1, it->y, it->z)) visible[3].push_front(&*it);
			if(!FindTile(it->x, it->y, it->z + 1)) visible[4].push_front(&*it);
			if(!FindTile(it->x, it->y, it->z - 1)) visible[5].push_front(&*it);
			it++;
		}
	}
	building = false;
}

GLuint textures[10];

GLvoid LoadGLTextures()
{
	// �������� ��������
	AUX_RGBImageRec *texture1;
	glGenTextures(3, textures);


	// �������� ��������
	texture1 = auxDIBImageLoad("stone_side.bmp");
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture1->sizeX, texture1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1->data);


	// �������� ��������
	texture1 = auxDIBImageLoad("stone_top.bmp");
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture1->sizeX, texture1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1->data);

	// �������� ��������
	texture1 = auxDIBImageLoad("stone_down.bmp");
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture1->sizeX, texture1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1->data);
}

int GLWindow::AddTile(signed long x, signed long y, signed long z, char mat)
{
	signed long bin = Hash(x, y, z);
	auto it = begin(tTiles[bin]);
	
	while(it < tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it != tTiles[bin].end()) return 0;
	
	tTiles[bin].push_front(Tile(x, y, z, mat));
	
	if (!building)
	{
		if(!FindTile(x, y + 1, z)) visible[0].push_front(&(tTiles[bin][0]));
		else FindTileN(x, y + 1, z, 1);
		if(!FindTile(x, y - 1, z)) visible[1].push_front(&(tTiles[bin][0]));
		else FindTileN(x, y - 1, z, 0);
		if(!FindTile(x + 1, y, z)) visible[2].push_front(&(tTiles[bin][0]));
		else FindTileN(x + 1, y, z, 3);
		if(!FindTile(x - 1, y, z)) visible[3].push_front(&(tTiles[bin][0]));
		else FindTileN(x - 1, y, z, 2);
		if(!FindTile(x, y, z + 1)) visible[4].push_front(&(tTiles[bin][0]));
		else FindTileN(x, y, z + 1, 5);
		if(!FindTile(x, y, z - 1)) visible[5].push_front(&(tTiles[bin][0]));
		else FindTileN(x, y, z - 1, 4);
	}

	return 1;
}

void GLWindow::FindTileN(signed long x, signed long y, signed long z, char N)
{
	auto it = begin(visible[N]);
	
	while(it < visible[N].end())
	{
		if ((it[0]->z == z)&&(it[0]->x == x)&&(it[0]->y == y)) break;
		it++;
	}
	if (it == visible[N].end()) return;

	visible[N].erase(it);
	return;
}

Tile* GLWindow::FindTile(signed long x, signed long y, signed long z)
{
	signed long bin = Hash(x, y, z);
	auto it = begin(tTiles[bin]);

	while(it < tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it == tTiles[bin].end()) return NULL;

	return &(*it);
}

int GLWindow::RmTile(signed long x, signed long y, signed long z)
{
	signed long bin = Hash(x, y, z);
	auto it = begin(tTiles[bin]);

	while(it < tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it == tTiles[bin].end()) return 0;

	tTiles[bin].erase(it);

	
	Tile *temp;
	temp = FindTile(x, y + 1, z); 
	if(!temp) FindTileN(x, y, z, 0);
	else visible[1].push_front(temp);
	temp = FindTile(x, y - 1, z); 
	if(!temp) FindTileN(x, y, z, 1);
	else visible[0].push_front(temp);
	temp = FindTile(x + 1, y, z); 
	if(!temp) FindTileN(x, y, z, 2);
	else visible[3].push_front(temp);
	temp = FindTile(x - 1, y, z); 
	if(!temp) FindTileN(x, y, z, 3);
	else visible[2].push_front(temp);
	temp = FindTile(x, y, z + 1); 
	if(!temp) FindTileN(x, y, z, 4);
	else visible[5].push_front(temp);
	temp = FindTile(x, y, z - 1); 
	if(!temp) FindTileN(x, y, z, 5);
	else visible[4].push_front(temp);

	return 1;
}

signed long GLWindow::Hash(signed long x, signed long y, signed long z)
{
	return (x & 0xff) + ((y & 0xff)<<8) + ((z & 0x0f)<<16);
}

GLWindow::~GLWindow(void)			// ���������� ���������� ����
{

}

void GLWindow::KillGLWindow(void)
{
	if( hRC )										// ���������� �� �������� ����������?
	{
		if( !wglMakeCurrent( NULL, NULL ) )			// �������� �� ���������� RC � DC?
		{ 
			MessageBox( NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		if( !wglDeleteContext( hRC ) )				// �������� �� ������� RC?
		{
			MessageBox( NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		hRC = NULL;									// ���������� RC � NULL
	}

	if( hDC && !ReleaseDC( hWnd, hDC ) )			// �������� �� ���������� DC?
	{
		MessageBox( NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hDC=NULL;									// ���������� DC � NULL
	}

	if(hWnd && !DestroyWindow(hWnd))				// �������� �� ���������� ����?
	{
		MessageBox( NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hWnd = NULL;								// ���������� hWnd � NULL
	}

	if( !UnregisterClass( "OpenGL", hInstance ) )	// �������� �� ����������������� �����
	{
		MessageBox( NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;							// ���������� hInstance � NULL
	}
}

int GLWindow::InitGL(void)											// ��� ��������� ������� OpenGL ���������� �����
{
	glShadeModel( GL_SMOOTH );									// ��������� ������� �������� �����������
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);						// ������� ������ � ������ ����
	glClearDepth( 1.0f );										// ��������� ������� ������ �������

	glEnable( GL_DEPTH_TEST );									// ��������� ���� �������

	glDepthFunc( GL_LEQUAL );									// ��� ����� �������
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );		// ��������� � ���������� �����������

	//glClearColor (10.3, 0.3, 0.3, 1.0);

	// ������� ��������� 
	glEnable(GL_LIGHTING);

	// ������� �������
	glEnable(GL_TEXTURE_2D);
	LoadGLTextures();
	
	// ������������� ������ ��������� 
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// �������������� ���������� �������� � ��������� �����
	glEnable(GL_NORMALIZE);

	return true;												// ������������� ������ �������
}

void GLWindow::ReSizeGLScene( GLsizei width, GLsizei height )	// �������� ������ � ���������������� ���� GL 
{
	if( height == 0 )											// �������������� ������� �� ���� 
	{
		height = 1;
	}

	glViewport( 0, 0, width, height );							// ����� ������� ������� ������
	glMatrixMode( GL_PROJECTION );								// ����� ������� ��������
	glLoadIdentity();											// ����� ������� ��������

	// ���������� ����������� �������������� �������� ��� ����
	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 10000.0f );
	glMatrixMode( GL_MODELVIEW );								// ����� ������� ���� ������
	glLoadIdentity();											// ����� ������� ���� ������

	this->width = width;
	this->height = height;
}

bool GLWindow::CreateGLWindow( LPCSTR title, GLsizei width, GLsizei height, int bits)
{
	GLuint    PixelFormat;								// ������ ��������� ����� ������
	WNDCLASS  wc;										// ��������� ������ ����
	DWORD    dwExStyle;									// ����������� ����� ����

	DWORD    dwStyle;									// ������� ����� ����
	RECT WindowRect;									// Grabs Rectangle Upper Left / Lower Right Values

	WindowRect.left=(long)0;							// ���������� ����� ������������ � 0
	WindowRect.right=(long)width;						// ���������� ������ ������������ � Width
	WindowRect.top=(long)0;								// ���������� ������� ������������ � 0
	WindowRect.bottom=(long)height;						// ���������� ������ ������������ � Height

	this->width = width;
	this->height = height;

	hInstance    = GetModuleHandle(NULL);				// ������� ���������� ������ ����������
	wc.style    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// ���������� ��� ����������� � ������ ������� DC
	wc.lpfnWndProc    =  (WNDPROC) WndProc;						// ��������� ��������� ���������
	wc.cbClsExtra    = 0;								// ��� �������������� ���������� ��� ����
	wc.cbWndExtra    = 0;								// ��� �������������� ���������� ��� ����
	wc.hInstance    = hInstance;						// ������������� ����������
	wc.hIcon    = LoadIcon(NULL, IDI_WINLOGO);			// ��������� ������ �� ���������
	wc.hCursor    = LoadCursor(NULL, IDC_ARROW);		// ��������� ��������� �����
	wc.hbrBackground  = NULL;							// ��� �� ��������� ��� GL
	wc.lpszMenuName    = NULL;							// ���� � ���� �� �����
	wc.lpszClassName  = "OpenGL";						// ������������� ��� ������

	if( !RegisterClass( &wc ) )							// �������� ���������������� ����� ����
	{
		MessageBox( NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;									// ����� � ����������� �������� �������� false
	}

	dwExStyle  =   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// ����������� ����� ����
	dwStyle    =   WS_OVERLAPPEDWINDOW;				// ������� ����� ����
	
	AdjustWindowRectEx( &WindowRect, dwStyle, false, dwExStyle );   // ��������� ���� ���������� �������

	if( !( hWnd = CreateWindowEx(  dwExStyle,			// ����������� ����� ��� ����
		"OpenGL",							// ��� ������
		title,								// ��������� ����
		WS_CLIPSIBLINGS |					// ��������� ����� ��� ����
		WS_CLIPCHILDREN |					// ��������� ����� ��� ����
		dwStyle,							// ���������� ����� ��� ����
		0, 0,								// ������� ����
		WindowRect.right-WindowRect.left,   // ���������� ���������� ������
		WindowRect.bottom-WindowRect.top,   // ���������� ���������� ������
		NULL,								// ��� �������������
		NULL,								// ��� ����
		hInstance,							// ���������� ����������
		NULL ) ) )							// �� ������� ������ �� WM_CREATE (???)
	{
		KillGLWindow();						// ������������ �����
		MessageBox( NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// ������� false
	}

	static  PIXELFORMATDESCRIPTOR pfd=      // pfd �������� Windows ����� ����� ����� �� ����� ������� �������
	{
		sizeof(PIXELFORMATDESCRIPTOR),      // ������ ����������� ������� ������� ��������
		1,									// ����� ������
		PFD_DRAW_TO_WINDOW |				// ������ ��� ����
		PFD_SUPPORT_OPENGL |				// ������ ��� OpenGL
		PFD_DOUBLEBUFFER,					// ������ ��� �������� ������
		PFD_TYPE_RGBA,						// ��������� RGBA ������
		bits,								// ���������� ��� ������� �����
		0, 0, 0, 0, 0, 0,					// ������������� �������� �����
		0,									// ��� ������ ������������
		0,									// ��������� ��� ������������
		0,									// ��� ������ ����������
		0, 0, 0, 0,							// ���� ���������� ������������
		32,									// 32 ������ Z-����� (����� �������)
		0,									// ��� ������ ���������
		0,									// ��� ��������������� �������
		PFD_MAIN_PLANE,						// ������� ���� ���������
		0,									// ���������������
		0, 0, 0								// ����� ���� ������������
	};

	if( !( hDC = GetDC( hWnd ) ) )          // ����� �� �� �������� �������� ����������?
	{
		KillGLWindow();						// ������������ �����
		MessageBox( NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// ������� false
	}

	if( !( PixelFormat = ChoosePixelFormat( hDC, &pfd ) ) )     // ������ �� ���������� ������ �������?
	{
		KillGLWindow();						// ������������ �����
		MessageBox( NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// ������� false
	}

	if( !SetPixelFormat( hDC, PixelFormat, &pfd ) )     // �������� �� ���������� ������ �������?
	{
		KillGLWindow();						// ������������ �����
		MessageBox( NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );

		return false;						// ������� false
	}

	if( !( hRC = wglCreateContext( hDC ) ) )			// �������� �� ���������� �������� ����������?
	{
		KillGLWindow();						// ������������ �����
		MessageBox( NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;						// ������� false
	}

	if( !wglMakeCurrent( hDC, hRC ) )					// ����������� ������������ �������� ����������
	{
		KillGLWindow();						// ������������ �����
		MessageBox( NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// ������� false
	}

	ShowWindow( hWnd, SW_SHOW );            // �������� ����
	SetForegroundWindow( hWnd );            // ������ ������� ���������
	SetFocus( hWnd );						// ���������� ����� ���������� �� ���� ����
	ReSizeGLScene( width, height );         // �������� ����������� ��� ������ OpenGL ������.

	if( !InitGL() )							// ������������� ������ ��� ���������� ����
	{
		KillGLWindow();						// ������������ �����

		MessageBox( NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// ������� false
	}

	return true;							// �� � �������!
}

double round(double x)
{
	if ( x - floor(x) >= 0.5) return ceil(x);
	return floor(x);
}

void GLWindow::GetCenterCoords(GLdouble *wx, GLdouble *wy, GLdouble *wz)
{
	GLint    viewport[4];		// ��������� viewport-a.
	GLdouble projection[16];	// ������� ��������.
	GLdouble modelview[16];		// ������� �������.
	GLfloat vz;					// ���������� ������� ���� � ������� ��������� viewport-a.

	glGetIntegerv(GL_VIEWPORT,viewport);           // ����� ��������� viewport-a.
	glGetDoublev(GL_PROJECTION_MATRIX,projection); // ����� ������� ��������.
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);   // ����� ������� �������.

	glReadPixels(width/2, height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
	gluUnProject((double) width/2,(double) height/2,(double) vz, modelview, projection, viewport, wx, wy, wz);
}

void GetPlane(GLdouble wx,GLdouble wy,GLdouble wz,GLdouble *xerr,GLdouble *yerr,GLdouble *zerr)
{
	*xerr = wx + TILE_SIZE/2;
	*yerr = wy;
	*zerr = wz + TILE_SIZE/2;

	while (*yerr < -1) *yerr += TILE_SIZE;
	while (*yerr > TILE_SIZE + 1) *yerr -= TILE_SIZE;

	*yerr = abs(*yerr);
	if(*yerr > abs(*yerr - TILE_SIZE)) *yerr = abs(*yerr - TILE_SIZE); 

	while (*xerr < - 1) *xerr += TILE_SIZE;
	while (*xerr > TILE_SIZE + 1) *xerr -= TILE_SIZE;

	*xerr = abs(*xerr);
	if(*xerr > abs(*xerr - TILE_SIZE)) *xerr = abs(*xerr - TILE_SIZE); 

	while (*zerr < - 1) *zerr += TILE_SIZE;
	while (*zerr > TILE_SIZE + 1) *zerr -= TILE_SIZE;

	*zerr = abs(*zerr);
	if(*zerr > abs(*zerr - TILE_SIZE)) *zerr = abs(*zerr - TILE_SIZE); 
}


void GLWindow::GetFrameTime()
{
	double currentTime = (double)timeGetTime() * 0.02;

    static double frameTime = currentTime;  // ����� ���������� �����

    // �������� �������, ���������� � �������� �����
    g_FrameInterval = currentTime - frameTime;

    frameTime = currentTime;
	//g_FrameInterval = 0.1;
}

void GLWindow::Control()
{
	GLdouble step = g_FrameInterval*WALK_SPEED;

	if(keys['W']) 
	{
		if(!player.falling)
		{
			player.gfVelX -= step*sin(TORAD(player.gfSpinY));
			player.gfVelZ -= step*cos(TORAD(player.gfSpinY));
		}
		else
		{
			player.gfVelX -= g_FrameInterval*AIR_ACCEL*step*sin(TORAD(player.gfSpinY));
			player.gfVelZ -= g_FrameInterval*AIR_ACCEL*step*cos(TORAD(player.gfSpinY));
		}
	}
	if(keys['S']) 
	{
		if(!player.falling)
		{
			player.gfVelX += step*sin(TORAD(player.gfSpinY));
			player.gfVelZ += step*cos(TORAD(player.gfSpinY));
		}
		else
		{
			player.gfVelX += g_FrameInterval*AIR_ACCEL*step*sin(TORAD(player.gfSpinY));
			player.gfVelZ += g_FrameInterval*AIR_ACCEL*step*cos(TORAD(player.gfSpinY));
		}
	}
	if(keys['D']) 
	{
		if(!player.falling)
		{
			player.gfVelX += step*cos(TORAD(player.gfSpinY));
			player.gfVelZ -= step*sin(TORAD(player.gfSpinY));
		}
		else
		{
			player.gfVelX += g_FrameInterval*AIR_ACCEL*step*cos(TORAD(player.gfSpinY));
			player.gfVelZ -= g_FrameInterval*AIR_ACCEL*step*sin(TORAD(player.gfSpinY));
		}
	}
	if(keys['A']) 
	{
		if(!player.falling)
		{
			player.gfVelX -= step*cos(TORAD(player.gfSpinY));
			player.gfVelZ += step*sin(TORAD(player.gfSpinY));
		}
		else
		{
			player.gfVelX -= g_FrameInterval*AIR_ACCEL*step*cos(TORAD(player.gfSpinY));
			player.gfVelZ += g_FrameInterval*AIR_ACCEL*step*sin(TORAD(player.gfSpinY));
		}
	}
	
	GLdouble ko = player.gfVelX*player.gfVelX + player.gfVelZ*player.gfVelZ;
	if(ko > g_FrameInterval*g_FrameInterval*MAX_SPEED*MAX_SPEED)
	{
		ko = pow(ko, 0.5);
		player.gfVelX = player.gfVelX*g_FrameInterval*MAX_SPEED/ko;
		player.gfVelZ = player.gfVelZ*g_FrameInterval*MAX_SPEED/ko;
	}
	
	if(keys['X'])
	{
		if(!player.falling)
		{
			player.gfVelY = -g_FrameInterval*JUMP_STR;
			player.falling = true;
			player.gfPosY += 0.1;
		}
	}

	if(keys['E']) 
	{
		GLdouble wx,wy,wz;			// ������������ ������� ����������.
		GetCenterCoords(&wx, &wy, &wz);

		GLdouble yerr, xerr, zerr;
		GetPlane(wx, wy, wz, &xerr, &yerr, &zerr);

		signed long xx, yy, zz;

		if((zerr < xerr)&&(zerr < yerr))
		{
			xx = floor(wx/TILE_SIZE + 0.5);
			yy = floor(wy/TILE_SIZE);

			if(player.gfPosZ < wz) zz = round(wz/TILE_SIZE + 0.5);
			if(player.gfPosZ > wz) zz = round(wz/TILE_SIZE - 0.5);
		}

		if((xerr < zerr)&&(xerr < yerr))
		{
			zz = floor(wz/TILE_SIZE + 0.5);
			yy = floor(wy/TILE_SIZE);

			if(player.gfPosX < wx) xx = round(wx/TILE_SIZE + 0.5);
			if(player.gfPosX > wx) xx = round(wx/TILE_SIZE - 0.5);
		}

		if((yerr < xerr)&&(yerr < zerr))
		{
			xx = floor(wx/TILE_SIZE + 0.5);
			zz = floor(wz/TILE_SIZE + 0.5);

			if(player.gfPosY < wy) yy = round(wy/TILE_SIZE);
			if(player.gfPosY > wy) yy = round(wy/TILE_SIZE - 1.0);
		}

		RmTile(xx,yy,zz);
	}
	
	if(keys['Q']) 
	{
		GLdouble wx,wy,wz;			// ������������ ������� ����������.
		GetCenterCoords(&wx, &wy, &wz);

		GLdouble yerr, xerr, zerr;
		GetPlane(wx, wy, wz, &xerr, &yerr, &zerr);
		signed long xx, yy, zz;

		if((zerr < xerr)&&(zerr < yerr))
		{
			xx = floor(wx/TILE_SIZE + 0.5);
			yy = floor(wy/TILE_SIZE);

			if(player.gfPosZ < wz) zz = round(wz/TILE_SIZE + 0.5) - 1;
			if(player.gfPosZ > wz) zz = round(wz/TILE_SIZE - 0.5) + 1;
		}

		if((xerr < zerr)&&(xerr < yerr))
		{
			zz = floor(wz/TILE_SIZE + 0.5);
			yy = floor(wy/TILE_SIZE);

			if(player.gfPosX < wx) xx = round(wx/TILE_SIZE + 0.5) - 1;
			if(player.gfPosX > wx) xx = round(wx/TILE_SIZE - 0.5) + 1;
		}

		if((yerr < xerr)&&(yerr < zerr))
		{
			xx = floor(wx/TILE_SIZE + 0.5);
			zz = floor(wz/TILE_SIZE + 0.5);

			if(player.gfPosY < wy) yy = round(wy/TILE_SIZE) - 1;
			if(player.gfPosY > wy) yy = round(wy/TILE_SIZE - 1.0) + 1;
		}

		AddTile(xx,yy,zz,MATERIAL_YES);
		// 		FILE * file;
		// 		file = fopen("out.txt", "w+");
		// 		fprintf(file, "%f\t%f\t%f|\t%d\t%d\t%d", wx, wy, wz, xx, yy, zz);
		// 		fclose(file);
	}

	{
		signed long xx, yy, zz;
		GLdouble wx = player.gfPosX + player.gfVelX;
		GLdouble wy = player.gfPosY - PLAYER_HEIGHT + 0.1;
		GLdouble wz = player.gfPosZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if((FindTile(xx, yy, zz) == NULL)&&(FindTile(xx, yy + 1, zz) == NULL))
			player.gfPosX += player.gfVelX;
		else player.gfVelX = 0;
	}
	{
		signed long xx, yy, zz;
		GLdouble wx = player.gfPosX;
		GLdouble wy = player.gfPosY - PLAYER_HEIGHT + 0.1;
		GLdouble wz = player.gfPosZ + player.gfVelZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if((FindTile(xx, yy, zz) == NULL)&&(FindTile(xx, yy + 1, zz) == NULL))
			player.gfPosZ += player.gfVelZ;
		else player.gfVelZ = 0;
	}

	if(player.falling)
	{
		player.gfPosY -= player.gfVelY;
		if(player.gfVelY < g_FrameInterval*MAX_DOWNSTEP)
			player.gfVelY += g_FrameInterval*g_FrameInterval*STEP_DOWNSTEP;
	}
	
	{
		signed long xx, yy, zz;
		GLdouble wx = player.gfPosX;
		GLdouble wy = player.gfPosY - PLAYER_HEIGHT;
		GLdouble wz = player.gfPosZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if(FindTile(xx, yy, zz) == NULL) player.falling = true;
		else
		{
			player.gfVelY = 0;
			if(player.falling)
			{	
				player.falling = false;
				player.gfPosY = (yy + 1)*TILE_SIZE + PLAYER_HEIGHT - 0.001;
			}
		}

	}

	if(!player.falling)
	{
		player.gfVelX = 0;
		player.gfVelZ = 0;
	}
	//player.falling = 1;
	
}
// ����� ����� ����������� ��� ����������
int GLWindow::DrawGLScene()   
{
	glEnable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// �������� ����� � ����� �������
	glLoadIdentity();											// �������� ������� �������

	glRotated( -player.gfSpinX, 1.0, 0.0, 0.0 );
	glRotated( -player.gfSpinY, 0.0, 1.0, 0.0 );
	glTranslated(-player.gfPosX, -player.gfPosY, -player.gfPosZ);

	GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);

	glTranslated(0, 30, 0);
	GLfloat light2_diffuse[] = {0.2, 0.2, 0.2};
	GLfloat light2_position[] = {0.0, 0.0, 0.0, 1.0};
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0);
	glTranslated(0, -30, 0);

	GLdouble fXcoord = 0, fYcoord = 0, fZcoord = 0;
	GLdouble dBrightness = 0.6;

	glNormal3d(0.0, 1.0, 0.0);
	glColor3d(dBrightness, dBrightness, dBrightness);


	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_QUADS);
		
	for( int i = 0; i < 6; i++)
	{
		auto it = begin(visible[i]);

		if(i == 1)
		{
			glEnd();
			glBindTexture(GL_TEXTURE_2D, textures[2]);
			glBegin(GL_QUADS);
		}
		if(i == 2)
		{
			glEnd();
			glNormal3d(-1.0, 0.0, 0.0);
			glBindTexture(GL_TEXTURE_2D, textures[0]);
			glBegin(GL_QUADS);
		}
		if(i == 4)
		{
			glEnd();
			glNormal3d(0.0, 0.0, -1.0);
			glBegin(GL_QUADS);
		}

		while(it < visible[i].end())
		{
			GlTile(it[0]->x,it[0]->y,it[0]->z, i);
			it++;
		}
	}

		glEnd();

	glDisable(GL_LIGHT2);

	return true;
}

void GLWindow::GlTile(signed long  X, signed long Y, signed long Z, char N)
{
	GLdouble dXcoord = X*TILE_SIZE, dYcoord = Y*TILE_SIZE, dZcoord = Z*TILE_SIZE;

	dXcoord -= TILE_SIZE/2;
	dZcoord -= TILE_SIZE/2;

	//glBindTexture(GL_TEXTURE_2D, textures[0]);

	//glBegin(GL_QUADS);
	//glNormal3f(0.0, 0.0, 1.0);
	if(N == 5)
	{
		glTexCoord2f(1.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
	}

	if(N == 4)
	{
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
	}
	//glNormal3f(1.0, 0.0, 0.0);

	if(N == 3)
	{
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
	}

	if(N == 2)
	{
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
	//glEnd();
	}

	//glBindTexture(GL_TEXTURE_2D, textures[2]);

	//glBegin(GL_QUADS);
	//glNormal3f(0.0, 1.0, 0.0);
	if(N == 1)
	{
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord, dZcoord);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
}
	//glEnd();

	//glBindTexture(GL_TEXTURE_2D, textures[1]);

	//glBegin(GL_QUADS);
	if(N == 0)
	{	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
	}
	//glEnd();
}
