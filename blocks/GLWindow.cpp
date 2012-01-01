#include "GLWindow.h"

LRESULT  CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

GLWindow::GLWindow(void)
{
	active = true;
	bMousing = false;

	tTiles = new Tiles[0x100000];
	
	for (int i = 0 ; i< 200 ; i++)
		for (int k = 0 ; k < 2 ; k++)
	{
		AddTile(rand()%200-100,k,rand()%200-100,MATERIAL_YES);
	}

	for (int i = 0; i < 200; i++)
		for (int k = 0; k < 200; k++)
		{
			AddTile(i-100,-1,k-100,MATERIAL_YES);
		}

	/**/
}

void GLWindow::AddTile(unsigned long x, unsigned long y, unsigned long z, char mat)
{
	tTiles[Hash(x, y, z)].push_front(Tile(x, y, z, mat));
}

int GLWindow::RmTile(unsigned long x, unsigned long y, unsigned long z)
{
	int bin = Hash(x, y, z);
	auto it = begin(tTiles[bin]);

	while(it < tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}

	if (it == tTiles[bin].end()) return 0;

	tTiles[bin].erase(it);

	return 1;
}

int GLWindow::Hash(unsigned long x, unsigned long y, unsigned long z)
{

	return (x & 0xff) + ((y & 0xff)<<8) + ((z & 0xf)<<16);
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

	// ������������� ������ ��������� 

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// �������������� ���������� �������� �

	// ��������� �����

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

#define TILE_SIZE 10

// ����� ����� ����������� ��� ����������
int GLWindow::DrawGLScene()   
{

	glEnable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// �������� ����� � ����� �������
	glLoadIdentity();											// �������� ������� �������


	glRotatef( -player.gfSpinX, 1.0f, 0.0f, 0.0f );
	glRotatef( -player.gfSpinY, 0.0f, 1.0f, 0.0f );
	glTranslatef(-player.gfPosX, -player.gfPosY, player.gfPosZ);


	GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);

	glTranslatef(0, 30, 0);
	GLfloat light2_diffuse[] = {0.2, 0.2, 0.2};
	GLfloat light2_position[] = {0.0, 0.0, 0.0, 1.0};
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0);
	glTranslatef(0, -30, 0);

	GLfloat fXcoord = 0, fYcoord = 0, fZcoord = 0;
	GLfloat fBrightness = 0.6;

	glNormal3f(0.0, 1.0, 0.0);
	glColor3f(fBrightness, fBrightness, fBrightness);


	for( int i = 0; i < 0x100000; i++)
	{
		auto it = begin(tTiles[i]);

		while(it < tTiles[i].end())
		{
			if(it->mat == MATERIAL_YES)
				GlTile(it->x,it->y,it->z);
			it++;
		}
	}
	
	glDisable(GL_LIGHT2);
	/*
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
	{
		GlTile(j,-1,-i);
	}

	*/
	/*
	for (int i = 0; i < 100; i++)
			for (int k = 0; k < 100; k++)
		{
			GlTile(rand()%100,k,-rand()%100);
		}
*/



	GLint    viewport[4];    // ��������� viewport-a.
	GLdouble projection[16]; // ������� ��������.
	GLdouble modelview[16];  // ������� �������.
	GLfloat vx,vy,vz;       // ���������� ������� ���� � ������� ��������� viewport-a.
	GLdouble wx,wy,wz;       // ������������ ������� ����������.

	glGetIntegerv(GL_VIEWPORT,viewport);           // ����� ��������� viewport-a.
	glGetDoublev(GL_PROJECTION_MATRIX,projection); // ����� ������� ��������.
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);   // ����� ������� �������.


		glReadPixels(width/2, height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
		gluUnProject((double) width/2,(double) height/2,(double) vz, modelview, projection, viewport, &wx, &wy, &wz);

		glBegin(GL_QUADS);

		double sz = 1;
		glVertex3f (wx - sz, wy - sz, wz + sz);
		glVertex3f (wx - sz, wy + sz, wz + sz);
		glVertex3f (wx + sz, wy + sz, wz + sz);
		glVertex3f (wx + sz, wy - sz, wz + sz);

		glVertex3f (wx - sz, wy - sz, wz - sz);
		glVertex3f (wx - sz, wy + sz, wz - sz);
		glVertex3f (wx + sz, wy + sz, wz - sz);
		glVertex3f (wx + sz, wy - sz, wz - sz);

		glVertex3f (wx - sz, wy + sz, wz - sz);
		glVertex3f (wx - sz, wy + sz, wz + sz);
		glVertex3f (wx + sz, wy + sz, wz + sz);
		glVertex3f (wx + sz, wy + sz, wz - sz);

		glVertex3f (wx - sz, wy - sz, wz - sz);
		glVertex3f (wx - sz, wy - sz, wz + sz);
		glVertex3f (wx + sz, wy - sz, wz + sz);
		glVertex3f (wx + sz, wy - sz, wz - sz);

		glVertex3f (wx + sz, wy - sz, wz - sz);
		glVertex3f (wx + sz, wy - sz, wz + sz);
		glVertex3f (wx + sz, wy + sz, wz + sz);
		glVertex3f (wx + sz, wy + sz, wz - sz);

		glVertex3f (wx - sz, wy - sz, wz - sz);
		glVertex3f (wx - sz, wy - sz, wz + sz);
		glVertex3f (wx - sz, wy + sz, wz + sz);
		glVertex3f (wx - sz, wy + sz, wz - sz);
		
		glEnd();
		/**/
	return true;
}

void GLWindow::GlTile(int X, int Y, int Z)
{
	glBegin(GL_QUADS);

	GLfloat fXcoord = X*TILE_SIZE, fYcoord = Y*TILE_SIZE, fZcoord = Z*TILE_SIZE;

	fXcoord -= TILE_SIZE/2;
	fZcoord -= TILE_SIZE/2;

	//glNormal3f(0.0, 0.0, 1.0);

	glVertex3f (fXcoord, fYcoord, fZcoord);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord);

	glVertex3f (fXcoord, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord + TILE_SIZE);

	//glNormal3f(1.0, 0.0, 0.0);

	glVertex3f (fXcoord, fYcoord, fZcoord);
	glVertex3f (fXcoord, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord);

	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord);

	//glNormal3f(0.0, 1.0, 0.0);

	glVertex3f (fXcoord, fYcoord, fZcoord);
	glVertex3f (fXcoord, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord);

	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord);

	glEnd();
}
