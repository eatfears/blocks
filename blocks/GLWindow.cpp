#include "GLWindow.h"
#include "resource.h"

LRESULT  CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

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

GLuint textures[100];
GLfloat fogColor[4]= {FOG_COLOR}; // ���� ������

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

	//100 500 500 1,5 GB
	for (int j = 1; j <= 100; j++)
	{
		for (int i = 0; i < 100; i++)
		{
			for (int k = 0; k < 100; k++)
			{
				AddTile(i-50,-j,k-50,MATERIAL_YES);
			}
		}
	}

	/**/
	for (int i = 0; i < 0x100000; i++)
	{
		Tiles::iterator it = tTiles[i].begin();
		
		while(it != tTiles[i].end())
		{
			if(!FindTile(it->x, it->y + 1, it->z)) visible[0].push_back(&*it);
			if(!FindTile(it->x, it->y - 1, it->z)) visible[1].push_back(&*it);
			if(!FindTile(it->x + 1, it->y, it->z)) visible[2].push_back(&*it);
			if(!FindTile(it->x - 1, it->y, it->z)) visible[3].push_back(&*it);
			if(!FindTile(it->x, it->y, it->z + 1)) visible[4].push_back(&*it);
			if(!FindTile(it->x, it->y, it->z - 1)) visible[5].push_back(&*it);
			it++;
		}
	}
	building = false;
}

GLvoid LoadGLTextures()
{
	HBITMAP hBMP;                   // ��������� �� �����������
	BITMAP  BMP;                    // ��������� �����������

	// ��� ID ��� �����������, ������� �� ����� ��������� �� ����� ��������
	byte  Texture[]={IDB_STONE_SIDE, IDB_STONE_TOP, IDB_STONE_DOWN};
	
	glGenTextures(sizeof(Texture), textures); // ������� 3 �������� (sizeof(Texture)=3 ID's)
	for (int i = 0; i < sizeof(Texture); i++)	// ���� �� ���� ID (�����������)
	{
		// �������� ��������
		hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(Texture[i]), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	
		if (hBMP)				// ���������� �� �����������?
		{						// ���� �� �

			GetObject(hBMP,sizeof(BMP), &BMP);

			// ����� ���������� �������� (�������� �� �������� ����� / 4 Bytes)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
			glBindTexture(GL_TEXTURE_2D, textures[i]);  // ����������� � ����� ���������

			// �������� ����������
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

			// �������� ���������� Mipmap GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			
			// ��������� Mipmapped �������� (3 �����, ������, ������ � ������ �� BMP)
			//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BMP.bmWidth, BMP.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
			
			DeleteObject(hBMP);              // �������� ������� �����������
		}
	}
}

int GLWindow::AddTile(signed short x, signed short y, signed short z, char mat)
{
	unsigned long bin = Hash(x, y, z);
	Tiles::iterator it = tTiles[bin].begin();
	
	while(it != tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it != tTiles[bin].end()) return 0;
	
	tTiles[bin].push_front(Tile(x, y, z, mat));
	
	if (!building)
	{
		if(!FindTile(x, y + 1, z)) visible[0].push_back(&(*tTiles[bin].begin()));
		else FindTileN(x, y + 1, z, 1);
		if(!FindTile(x, y - 1, z)) visible[1].push_back(&(*tTiles[bin].begin()));
		else FindTileN(x, y - 1, z, 0);
		if(!FindTile(x + 1, y, z)) visible[2].push_back(&(*tTiles[bin].begin()));
		else FindTileN(x + 1, y, z, 3);
		if(!FindTile(x - 1, y, z)) visible[3].push_back(&(*tTiles[bin].begin()));
		else FindTileN(x - 1, y, z, 2);
		if(!FindTile(x, y, z + 1)) visible[4].push_back(&(*tTiles[bin].begin()));
		else FindTileN(x, y, z + 1, 5);
		if(!FindTile(x, y, z - 1)) visible[5].push_back(&(*tTiles[bin].begin()));
		else FindTileN(x, y, z - 1, 4);
	}

	return 1;
}

void GLWindow::FindTileN(signed short x, signed short y, signed short z, char N)
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

Tile* GLWindow::FindTile(signed short x, signed short y, signed short z)
{
	unsigned long bin = Hash(x, y, z);
	Tiles::iterator it = tTiles[bin].begin();

	while(it != tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it == tTiles[bin].end()) return NULL;

	return &(*it);
}

int GLWindow::RmTile(signed short x, signed short y, signed short z)
{
	unsigned long bin = Hash(x, y, z);
	Tiles::iterator it = tTiles[bin].begin();

	while(it != tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it == tTiles[bin].end()) return 0;


	
	Tile *temp;
	temp = FindTile(x, y + 1, z); 
	if(!temp) FindTileN(x, y, z, 0);
	else visible[1].push_back(temp);
	
	temp = FindTile(x, y - 1, z); 
	if(!temp) FindTileN(x, y, z, 1);
	else visible[0].push_back(temp);
	temp = FindTile(x + 1, y, z); 
	if(!temp) FindTileN(x, y, z, 2);
	else visible[3].push_back(temp);
	temp = FindTile(x - 1, y, z); 
	if(!temp) FindTileN(x, y, z, 3);
	else visible[2].push_back(temp);
	temp = FindTile(x, y, z + 1); 
	if(!temp) FindTileN(x, y, z, 4);
	else visible[5].push_back(temp);
	temp = FindTile(x, y, z - 1); 
	if(!temp) FindTileN(x, y, z, 5);
	else visible[4].push_back(temp);
	/**/

	tTiles[bin].erase(it);

	return 1;
}

unsigned long GLWindow::Hash(signed short x, signed short y, signed short z)
{
	return (x & 0xff) + ((z & 0xff)<<8) + ((y & 0x0f)<<16);
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
	glClearColor(FOG_COLOR);								// ������� ������ � ������ ����
	glClearDepth( 1.0f );										// ��������� ������� ������ �������

	glEnable( GL_DEPTH_TEST );									// ��������� ���� �������

	glDepthFunc( GL_LEQUAL );									// ��� ����� �������
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );		// ��������� � ���������� �����������

	//glClearColor (10.3, 0.3, 0.3, 1.0);

	// ������� ��������� 
	//glEnable(GL_LIGHTING);

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
	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, TILE_SIZE + MAX_VIEV_DIST);
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
	double currentTime = (double)timeGetTime() * 0.025;

    static double frameTime = currentTime;  // ����� ���������� �����

    // �������� �������, ���������� � �������� �����
    g_FrameInterval = currentTime - frameTime;

    frameTime = currentTime;
	//g_FrameInterval = 0.1;
	//0.04;
}

void GLWindow::Control()
{
	GLdouble step = WALK_SPEED;
	if(keys[VK_SHIFT]) step *= SPRINT_KOEF;


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
	if(ko > step*step)
	{
		ko = pow(ko, 0.5);
		player.gfVelX = player.gfVelX*step/ko;
		player.gfVelZ = player.gfVelZ*step/ko;
	}
	
	if(keys[VK_SPACE])
	{
		if(!player.falling)
		{
			player.gfVelY = -JUMP_STR;
			player.falling = true;
			player.gfPosY += g_FrameInterval;
		}
	}

	//��������� ����
	GLdouble wx,wy,wz;			// ������������ ������� ����������.
	GetCenterCoords(&wx, &wy, &wz);

	GLdouble yerr, xerr, zerr;
	GetPlane(wx, wy, wz, &xerr, &yerr, &zerr);

	signed short xx, yy, zz;

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

	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3d(0.1, 0.1, 0.1);
	glLineWidth (2.0);
	glBegin(GL_QUADS);
	if (FindTile(xx,yy,zz))
		for(int i = 0; i < 6; i++)
			GlTile(xx,yy,zz,i);
	glEnd();
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	

	if(keys['E']) 
	{
		RmTile(xx,yy,zz);
	}
	
	if(keys['Q']) 
	{
		if((zerr < xerr)&&(zerr < yerr))
		{
			if(player.gfPosZ < wz) zz -= 1;
			if(player.gfPosZ > wz) zz += 1;
		}
		if((xerr < zerr)&&(xerr < yerr))
		{
			if(player.gfPosX < wx) xx -= 1;
			if(player.gfPosX > wx) xx += 1;
		}
		if((yerr < xerr)&&(yerr < zerr))
		{
			if(player.gfPosY < wy) yy -= 1;
			if(player.gfPosY > wy) yy += 1;
		}

		AddTile(xx,yy,zz,MATERIAL_YES);
	}

	{
		signed short xx, yy, zz;
		GLdouble wx = player.gfPosX + player.gfVelX;
		GLdouble wy = player.gfPosY - PLAYER_HEIGHT + 0.1;
		GLdouble wz = player.gfPosZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if((FindTile(xx, yy, zz) == NULL)&&(FindTile(xx, yy + 1, zz) == NULL))
			player.gfPosX += g_FrameInterval*player.gfVelX;
		else player.gfVelX = 0;
	}
	{
		signed short xx, yy, zz;
		GLdouble wx = player.gfPosX;
		GLdouble wy = player.gfPosY - PLAYER_HEIGHT + 0.1;
		GLdouble wz = player.gfPosZ + player.gfVelZ;

		xx = floor(wx/TILE_SIZE + 0.5);
		zz = floor(wz/TILE_SIZE + 0.5);
		yy = floor(wy/TILE_SIZE);

		if((FindTile(xx, yy, zz) == NULL)&&(FindTile(xx, yy + 1, zz) == NULL))
			player.gfPosZ += g_FrameInterval*player.gfVelZ;
		else player.gfVelZ = 0;
	}

	if(player.falling)
	{
		player.gfPosY -= g_FrameInterval*player.gfVelY;
		if(player.gfVelY < MAX_DOWNSTEP)
			player.gfVelY += g_FrameInterval*STEP_DOWNSTEP;
	}
	
	{
		signed short xx, yy, zz;
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


	glEnable(GL_FOG);						// �������� ����� (GL_FOG)
	glFogi(GL_FOG_MODE,  GL_LINEAR);			// �������� ��� ������
	glFogfv(GL_FOG_COLOR, fogColor);		// ������������� ���� ������
	glFogf(GL_FOG_DENSITY, FOG_DENSITY);	// ��������� ������ ����� �����
	glHint(GL_FOG_HINT, GL_DONT_CARE);		// ��������������� ��������� ������
	glFogf(GL_FOG_START, FOG_START);		// �������, � ������� ���������� �����
	glFogf(GL_FOG_END, MAX_VIEV_DIST);		// �������, ��� ����� �������������.

	GLdouble dBrightness = 1.0;
	glColor3d(dBrightness, dBrightness, dBrightness);
	
	glNormal3d(0.0, 1.0, 0.0);
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
			if((abs(it[0]->x*TILE_SIZE - player.gfPosX) < MAX_VIEV_DIST + 10*TILE_SIZE) && (abs(it[0]->y*TILE_SIZE - player.gfPosY) < MAX_VIEV_DIST + 10*TILE_SIZE) && (abs(it[0]->z*TILE_SIZE - player.gfPosZ) < MAX_VIEV_DIST + 10*TILE_SIZE))
				GlTile(it[0]->x,it[0]->y,it[0]->z, i);
			it++;
		}
	}

	glEnd();

	glDisable(GL_LIGHT2);

	return true;
}

void GLWindow::DrawInterface()
{
	glLoadIdentity();											// �������� ������� �������

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

void GLWindow::GlTile(signed short  X, signed short Y, signed short Z, char N)
{
	GLdouble dXcoord = X*TILE_SIZE, dYcoord = Y*TILE_SIZE, dZcoord = Z*TILE_SIZE;

	dXcoord -= TILE_SIZE/2;
	dZcoord -= TILE_SIZE/2;

	switch(N)
	{
	case 0:
		{	
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case 1:
		{
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
		}break;
	case 2:
		{
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case 3:
		{
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
		}break;
	case 4:
		{
			glTexCoord2f(0.0f, 0.0f);
			glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
		}break;
	case 5:
		{
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
