#include "GLWindow.h"

LRESULT  CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

GLWindow::GLWindow(int iResolution)
{
	active = true;
}

GLWindow::GLWindow(void)
{
	active = true;
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
	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 200.0f );
	glMatrixMode( GL_MODELVIEW );								// ����� ������� ���� ������
	glLoadIdentity();											// ����� ������� ���� ������
}

bool GLWindow::CreateGLWindow( LPCSTR title, int width, int height, int bits)
{
	;
	GLuint    PixelFormat;								// ������ ��������� ����� ������
	WNDCLASS  wc;										// ��������� ������ ����
	DWORD    dwExStyle;									// ����������� ����� ����

	DWORD    dwStyle;									// ������� ����� ����
	RECT WindowRect;									// Grabs Rectangle Upper Left / Lower Right Values

	WindowRect.left=(long)0;							// ���������� ����� ������������ � 0
	WindowRect.right=(long)width;						// ���������� ������ ������������ � Width
	WindowRect.top=(long)0;								// ���������� ������� ������������ � 0
	WindowRect.bottom=(long)height;						// ���������� ������ ������������ � Height

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

	glTranslatef(10.0f, -20.0f,-55);	// ��������� ����� �� 1.5 ������� �


	GLfloat fXcoord = 0 , fYcoord = 0, fZcoord = 0;
	GLfloat fBrightness = 1;

	GlTile(0,0,0);
	GlTile(-1,0,0);
	GlTile(-1,1,0);
	GlTile(-1,2,0);
	GlTile(-2,2,1);


	
	return true;	// ���������� ������ �������
}

void GLWindow::GlTile(int X, int Y, int Z)
{
	glBegin(GL_QUADS);

	GLfloat fXcoord = X*TILE_SIZE, fYcoord = Y*TILE_SIZE, fZcoord = Z*TILE_SIZE;

	fXcoord -= TILE_SIZE/2.0;
	fYcoord -= TILE_SIZE/2.0;

	glColor3f((GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX);

	glVertex3f (fXcoord, fYcoord, fZcoord);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord);

	glColor3f((GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX);

	glVertex3f (fXcoord, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord + TILE_SIZE);

	glColor3f((GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX);

	glVertex3f (fXcoord, fYcoord, fZcoord);
	glVertex3f (fXcoord, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord);

	glColor3f((GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX);

	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord);

	glColor3f((GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX);

	glVertex3f (fXcoord, fYcoord, fZcoord);
	glVertex3f (fXcoord, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord, fZcoord);

	glColor3f((GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX, (GLfloat)rand()/(GLfloat)RAND_MAX);

	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord);
	glVertex3f (fXcoord, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord + TILE_SIZE);
	glVertex3f (fXcoord + TILE_SIZE, fYcoord + TILE_SIZE, fZcoord);

	glEnd();
}
