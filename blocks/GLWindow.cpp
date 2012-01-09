#include "GLWindow.h"
#include "Blocks_Definitions.h"

LRESULT  CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

GLfloat fogColor[4]= {FOG_COLOR};

GLWindow::GLWindow(void)
{
	active = true;
	bMousing = false;
	fullscreen = false;

	MaterialLib.InitMaterials();
	wWorld.Build();
}

GLWindow::~GLWindow(void)			// ���������� ���������� ����
{
	//delete[] tTiles;
}

void GLWindow::KillGLWindow(void)
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

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

int GLWindow::InitGL(void)										// ��� ��������� ������� OpenGL ���������� �����
{
	glShadeModel(GL_SMOOTH);									// ��������� ������� �������� �����������
	glClearColor(FOG_COLOR);									// ������� ������ � ������ ����
	glClearDepth(1.0f);											// ��������� ������� ������ �������

	glEnable(GL_DEPTH_TEST);									// ��������� ���� �������

	glDepthFunc(GL_LEQUAL);										// ��� ����� �������
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// ��������� � ���������� �����������

	//������� ��������� 
	//glEnable(GL_LIGHTING);

	//������� �������
	glEnable(GL_TEXTURE_2D);
	
	//������������� ������ ��������� 
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	//�������������� ���������� �������� � ��������� �����
	glEnable(GL_NORMALIZE);

	MaterialLib.LoadGLTextures();

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

	//���������� ����������� �������������� �������� ��� ����
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

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle  =   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// ����������� ����� ����
		dwStyle    =   WS_OVERLAPPEDWINDOW;				// ������� ����� ����
	}
	
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

	if(!InitGL())							// ������������� ������ ��� ���������� ����
	{
		KillGLWindow();						// ������������ �����

		MessageBox( NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// ������� false
	}

	return true;							// �� � �������!
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

void GLWindow::GetFrameTime()
{
	double currentTime = (double)timeGetTime() * 0.025;

    static double frameTime = currentTime;  // ����� ���������� �����

    //�������� �������, ���������� � �������� �����
    g_FrameInterval = currentTime - frameTime;

    frameTime = currentTime;
	//g_FrameInterval = 0.1;
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


	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);		//��� ������
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, FOG_DENSITY);	//��������� ������ ����� �����
	glHint(GL_FOG_HINT, GL_DONT_CARE);		//��������������� ��������� ������
	glFogf(GL_FOG_START, FOG_START);		//�������, � ������� ���������� �����
	glFogf(GL_FOG_END, MAX_VIEV_DIST);		//�������, ��� ����� �������������

	GLdouble dBrightness = 1.0;
	glColor3d(dBrightness, dBrightness, dBrightness);


	int iCurrTex = 0;
	glBegin(GL_QUADS);

	for( int i = 0; i < 6; i++)
	{
		auto it = begin(wWorld.visible[i]);
		GLuint *tex = MaterialLib.textures;

		while(it < wWorld.visible[i].end())
		{

			if((abs(it[0]->x*TILE_SIZE - player.gfPosX) < MAX_VIEV_DIST + 10*TILE_SIZE) && (abs(it[0]->y*TILE_SIZE - player.gfPosY) < MAX_VIEV_DIST + 10*TILE_SIZE) && (abs(it[0]->z*TILE_SIZE - player.gfPosZ) < MAX_VIEV_DIST + 10*TILE_SIZE))
			{
				if(iCurrTex != MaterialLib.mMater[it[0]->mat].iTex[i])
				{
					iCurrTex = MaterialLib.mMater[it[0]->mat].iTex[i];
					glEnd();
					glBindTexture(GL_TEXTURE_2D, tex[iCurrTex]);
					glBegin(GL_QUADS);
				}
				GlTile(it[0], i);
			}
			it++;
		}
	}

	glEnd();

	glDisable(GL_LIGHT2);

	return true;
}

void GLWindow::DrawInterface()
{
	//��������� ����
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3d(0.1, 0.1, 0.1);
	glLineWidth (2.0);
	glBegin(GL_QUADS);
	if (Tile *temp = wWorld.FindTile(player.xx,player.yy,player.zz))
		for(int i = 0; i < 6; i++)
			GlTile(temp,i);
	glEnd();
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	//�������� ������� �������
	glLoadIdentity();

	//��������� �������
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

void GLWindow::GlTile(Tile *tTile, char N)
{
	GLdouble dXcoord = tTile->x*TILE_SIZE, dYcoord = tTile->y*TILE_SIZE, dZcoord = tTile->z*TILE_SIZE;

	dXcoord -= TILE_SIZE/2;
	dZcoord -= TILE_SIZE/2;

	switch(N)
	{
	case TOP:
		{	
			//������� �����
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
			//������ �����
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
			//������ �����
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
			//����� �����
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
			//������ �����
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
			//�������� �����
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

bool GLWindow::Loop()
{
	// ������������� �����
	if(active)					// ������� �� ���������?
	{
		if(player.keys[VK_ESCAPE])			// ���� �� ������ ������� ESC?
		{
			return true;			// ESC ������� �� �������� ���������� ���������
		}
		else						// �� ����� ��� ������, ������� �����.
		{
			DrawGLScene();
			GetCenterCoords(&player.wx, &player.wy, &player.wz);
			
			player.Control(g_FrameInterval, wWorld);
			DrawInterface();

			GetFrameTime();

			if(player.keys[VK_F1])						// Is F1 Being Pressed?
			{
				player.keys[VK_F1] = false;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
				bMousing = false;
				// Recreate Our OpenGL Window
				if(!CreateGLWindow("Blocks", RESX, RESY, BITS))
				{
					return true;						// Quit If Window Was Not Created
				}
			}

			SwapBuffers(hDC);		// ������ ����� (������� �����������)
		}
	}
	return false;
}