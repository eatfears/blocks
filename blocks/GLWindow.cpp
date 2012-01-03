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
	
	//AddTile(0,-1,0,MATERIAL_YES);
	//AddTile(3,-1,0,MATERIAL_YES);
	//AddTile(0,-1,3,MATERIAL_YES);

	/*
	AddTile(10,0,0,MATERIAL_YES);
	AddTile(0,1,0,MATERIAL_YES);
	AddTile(0,-3,0,MATERIAL_YES);
	AddTile(1,0,0,MATERIAL_YES);
	AddTile(-1,0,0,MATERIAL_YES);
	/**/
	for (int i = 0 ; i< 10 ; i++)
		for (int k = 0 ; k < 1 ; k++)
	{
		AddTile(rand()%100-50,k,rand()%100-50,MATERIAL_YES);
	}

	for (int j = 1; j <= 10; j++)
	for (int i = 0; i < 20; i++)
		for (int k = 0; k < 20; k++)
		{
			AddTile(i-10,-j,k-10,MATERIAL_YES);
		}

	/**/
}

GLuint textures[2];

GLvoid LoadGLTextures()
{
	/*
	glGenTextures(1, textures);


	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 16, 16, 0, GLenum format, GLenum type, const GLvoid *pixels);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLenum p2);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLenum p2);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);*/

}

int GLWindow::AddTile(signed long x, signed long y, signed long z, char mat)
{
	int bin = Hash(x, y, z);
	auto it = begin(tTiles[bin]);
	
	while(it < tTiles[bin].end())
	{
		if ((it->z == z)&&(it->x == x)&&(it->y == y)) break;
		it++;
	}
	if (it != tTiles[bin].end()) return 0;
	
	tTiles[bin].push_front(Tile(x, y, z, mat));
	return 1;
}

Tile* GLWindow::FindTile(signed long x, signed long y, signed long z)
{
	int bin = Hash(x, y, z);
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

int GLWindow::Hash(signed long x, signed long y, signed long z)
{
	return (x & 0xff) + ((y & 0xff)<<8) + ((z & 0xf)<<16);
}

GLWindow::~GLWindow(void)			// Корректное разрушение окна
{

}

void GLWindow::KillGLWindow(void)
{
	if( hRC )										// Существует ли Контекст Рендеринга?
	{
		if( !wglMakeCurrent( NULL, NULL ) )			// Возможно ли освободить RC и DC?
		{ 
			MessageBox( NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		if( !wglDeleteContext( hRC ) )				// Возможно ли удалить RC?
		{
			MessageBox( NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		hRC = NULL;									// Установить RC в NULL
	}

	if( hDC && !ReleaseDC( hWnd, hDC ) )			// Возможно ли уничтожить DC?
	{
		MessageBox( NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hDC=NULL;									// Установить DC в NULL
	}

	if(hWnd && !DestroyWindow(hWnd))				// Возможно ли уничтожить окно?
	{
		MessageBox( NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hWnd = NULL;								// Установить hWnd в NULL
	}

	if( !UnregisterClass( "OpenGL", hInstance ) )	// Возможно ли разрегистрировать класс
	{
		MessageBox( NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;							// Установить hInstance в NULL
	}
}

int GLWindow::InitGL(void)											// Все установки касаемо OpenGL происходят здесь
{
	glShadeModel( GL_SMOOTH );									// Разрешить плавное цветовое сглаживание
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);						// Очистка экрана в черный цвет
	glClearDepth( 1.0f );										// Разрешить очистку буфера глубины

	glEnable( GL_DEPTH_TEST );									// Разрешить тест глубины

	glDepthFunc( GL_LEQUAL );									// Тип теста глубины
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );		// Улучшение в вычислении перспективы

	//glClearColor (10.3, 0.3, 0.3, 1.0);

	// рассчет освещения 
	glEnable(GL_LIGHTING);

	// рассчет текстур
	glEnable(GL_TEXTURE_2D);

	// двухсторонний расчет освещения 
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// автоматическое приведение нормалей к единичной длине
	glEnable(GL_NORMALIZE);

	return true;												// Инициализация прошла успешно
}

void GLWindow::ReSizeGLScene( GLsizei width, GLsizei height )	// Изменить размер и инициализировать окно GL 
{
	if( height == 0 )											// Предотвращение деления на ноль 
	{
		height = 1;
	}

	glViewport( 0, 0, width, height );							// Сброс текущей области вывода
	glMatrixMode( GL_PROJECTION );								// Выбор матрицы проекций
	glLoadIdentity();											// Сброс матрицы проекции

	// Вычисление соотношения геометрических размеров для окна
	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 10000.0f );
	glMatrixMode( GL_MODELVIEW );								// Выбор матрицы вида модели
	glLoadIdentity();											// Сброс матрицы вида модели

	this->width = width;
	this->height = height;
}

bool GLWindow::CreateGLWindow( LPCSTR title, GLsizei width, GLsizei height, int bits)
{
	GLuint    PixelFormat;								// Хранит результат после поиска
	WNDCLASS  wc;										// Структура класса окна
	DWORD    dwExStyle;									// Расширенный стиль окна

	DWORD    dwStyle;									// Обычный стиль окна
	RECT WindowRect;									// Grabs Rectangle Upper Left / Lower Right Values

	WindowRect.left=(long)0;							// Установить левую составляющую в 0
	WindowRect.right=(long)width;						// Установить правую составляющую в Width
	WindowRect.top=(long)0;								// Установить верхнюю составляющую в 0
	WindowRect.bottom=(long)height;						// Установить нижнюю составляющую в Height

	this->width = width;
	this->height = height;

	hInstance    = GetModuleHandle(NULL);				// Считаем дескриптор нашего приложения
	wc.style    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Перерисуем при перемещении и создаём скрытый DC
	wc.lpfnWndProc    =  (WNDPROC) WndProc;						// Процедура обработки сообщений
	wc.cbClsExtra    = 0;								// Нет дополнительной информации для окна
	wc.cbWndExtra    = 0;								// Нет дополнительной информации для окна
	wc.hInstance    = hInstance;						// Устанавливаем дескриптор
	wc.hIcon    = LoadIcon(NULL, IDI_WINLOGO);			// Загружаем иконку по умолчанию
	wc.hCursor    = LoadCursor(NULL, IDC_ARROW);		// Загружаем указатель мышки
	wc.hbrBackground  = NULL;							// Фон не требуется для GL
	wc.lpszMenuName    = NULL;							// Меню в окне не будет
	wc.lpszClassName  = "OpenGL";						// Устанавливаем имя классу

	if( !RegisterClass( &wc ) )							// Пытаемся зарегистрировать класс окна
	{
		MessageBox( NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;									// Выход и возвращение функцией значения false
	}

	dwExStyle  =   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// Расширенный стиль окна
	dwStyle    =   WS_OVERLAPPEDWINDOW;				// Обычный стиль окна
	
	AdjustWindowRectEx( &WindowRect, dwStyle, false, dwExStyle );   // Подбирает окну подходящие размеры

	if( !( hWnd = CreateWindowEx(  dwExStyle,			// Расширенный стиль для окна
		"OpenGL",							// Имя класса
		title,								// Заголовок окна
		WS_CLIPSIBLINGS |					// Требуемый стиль для окна
		WS_CLIPCHILDREN |					// Требуемый стиль для окна
		dwStyle,							// Выбираемые стили для окна
		0, 0,								// Позиция окна
		WindowRect.right-WindowRect.left,   // Вычисление подходящей ширины
		WindowRect.bottom-WindowRect.top,   // Вычисление подходящей высоты
		NULL,								// Нет родительского
		NULL,								// Нет меню
		hInstance,							// Дескриптор приложения
		NULL ) ) )							// Не передаём ничего до WM_CREATE (???)
	{
		KillGLWindow();						// Восстановить экран
		MessageBox( NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// Вернуть false
	}

	static  PIXELFORMATDESCRIPTOR pfd=      // pfd сообщает Windows каким будет вывод на экран каждого пикселя
	{
		sizeof(PIXELFORMATDESCRIPTOR),      // Размер дескриптора данного формата пикселей
		1,									// Номер версии
		PFD_DRAW_TO_WINDOW |				// Формат для Окна
		PFD_SUPPORT_OPENGL |				// Формат для OpenGL
		PFD_DOUBLEBUFFER,					// Формат для двойного буфера
		PFD_TYPE_RGBA,						// Требуется RGBA формат
		bits,								// Выбирается бит глубины цвета
		0, 0, 0, 0, 0, 0,					// Игнорирование цветовых битов
		0,									// Нет буфера прозрачности
		0,									// Сдвиговый бит игнорируется
		0,									// Нет буфера накопления
		0, 0, 0, 0,							// Биты накопления игнорируются
		32,									// 32 битный Z-буфер (буфер глубины)
		0,									// Нет буфера трафарета
		0,									// Нет вспомогательных буферов
		PFD_MAIN_PLANE,						// Главный слой рисования
		0,									// Зарезервировано
		0, 0, 0								// Маски слоя игнорируются
	};

	if( !( hDC = GetDC( hWnd ) ) )          // Можем ли мы получить Контекст Устройства?
	{
		KillGLWindow();						// Восстановить экран
		MessageBox( NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// Вернуть false
	}

	if( !( PixelFormat = ChoosePixelFormat( hDC, &pfd ) ) )     // Найден ли подходящий формат пикселя?
	{
		KillGLWindow();						// Восстановить экран
		MessageBox( NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// Вернуть false
	}

	if( !SetPixelFormat( hDC, PixelFormat, &pfd ) )     // Возможно ли установить Формат Пикселя?
	{
		KillGLWindow();						// Восстановить экран
		MessageBox( NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );

		return false;						// Вернуть false
	}

	if( !( hRC = wglCreateContext( hDC ) ) )			// Возможно ли установить Контекст Рендеринга?
	{
		KillGLWindow();						// Восстановить экран
		MessageBox( NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;						// Вернуть false
	}

	if( !wglMakeCurrent( hDC, hRC ) )					// Попробовать активировать Контекст Рендеринга
	{
		KillGLWindow();						// Восстановить экран
		MessageBox( NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// Вернуть false
	}

	ShowWindow( hWnd, SW_SHOW );            // Показать окно
	SetForegroundWindow( hWnd );            // Слегка повысим приоритет
	SetFocus( hWnd );						// Установить фокус клавиатуры на наше окно
	ReSizeGLScene( width, height );         // Настроим перспективу для нашего OpenGL экрана.

	if( !InitGL() )							// Инициализация только что созданного окна
	{
		KillGLWindow();						// Восстановить экран

		MessageBox( NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// Вернуть false
	}

	return true;							// Всё в порядке!
}

double round(double x)
{
	if ( x - floor(x) >= 0.5) return ceil(x);
	return floor(x);
}

void GLWindow::GetCenterCoords(GLdouble *wx, GLdouble *wy, GLdouble *wz)
{
	GLint    viewport[4];		// параметры viewport-a.
	GLdouble projection[16];	// матрица проекции.
	GLdouble modelview[16];		// видовая матрица.
	GLfloat vz;					// координаты курсора мыши в системе координат viewport-a.

	glGetIntegerv(GL_VIEWPORT,viewport);           // узнаём параметры viewport-a.
	glGetDoublev(GL_PROJECTION_MATRIX,projection); // узнаём матрицу проекции.
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);   // узнаём видовую матрицу.

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

    static double frameTime = currentTime;  // Время последнего кадра

    // Интервал времени, прошедшего с прошлого кадра
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
			player.gfVelX += AIR_ACCEL*step*cos(TORAD(player.gfSpinY));
			player.gfVelZ -= AIR_ACCEL*step*sin(TORAD(player.gfSpinY));
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
			player.gfVelX -= AIR_ACCEL*step*cos(TORAD(player.gfSpinY));
			player.gfVelZ += AIR_ACCEL*step*sin(TORAD(player.gfSpinY));
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
		GLdouble wx,wy,wz;			// возвращаемые мировые координаты.
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
		GLdouble wx,wy,wz;			// возвращаемые мировые координаты.
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
	
}
// Здесь будет происходить вся прорисовка
int GLWindow::DrawGLScene()   
{
	glEnable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		// Очистить экран и буфер глубины
	glLoadIdentity();											// Сбросить текущую матрицу

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

	glNormal3f(0.0, 1.0, 0.0);
	glColor3d(dBrightness, dBrightness, dBrightness);

	for( int i = 0; i < 0x100000; i++)
//	for( int i = 0; i < 0x100000; i++)
	{
		auto it = begin(tTiles[i]);

		while(it < tTiles[i].end())
		{
			GlTile(it->x,it->y,it->z);
			it++;
		}
	}
	
	glDisable(GL_LIGHT2);

	return true;
}

void GLWindow::GlTile(int X, int Y, int Z)
{
	glBegin(GL_QUADS);

	GLdouble dXcoord = X*TILE_SIZE, dYcoord = Y*TILE_SIZE, dZcoord = Z*TILE_SIZE;

	dXcoord -= TILE_SIZE/2;
	dZcoord -= TILE_SIZE/2;

	//glNormal3f(0.0, 0.0, 1.0);

	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);

	glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);

	//glNormal3f(1.0, 0.0, 0.0);

	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);

	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);

	//glNormal3f(0.0, 1.0, 0.0);

	glVertex3d (dXcoord, dYcoord, dZcoord);
	glVertex3d (dXcoord, dYcoord, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord, dZcoord);

	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord);
	glVertex3d (dXcoord, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord + TILE_SIZE);
	glVertex3d (dXcoord + TILE_SIZE, dYcoord + TILE_SIZE, dZcoord);

	glEnd();
}
