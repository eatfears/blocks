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
GLfloat fogColor[4]= {FOG_COLOR}; // Цвет тумана

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
	HBITMAP hBMP;                   // Указатель на изображение
	BITMAP  BMP;                    // структура изображения

	// Три ID для изображений, которые мы хотим загрузить из файла ресурсов
	byte  Texture[]={IDB_STONE_SIDE, IDB_STONE_TOP, IDB_STONE_DOWN};
	
	glGenTextures(sizeof(Texture), textures); // создаем 3 текстуры (sizeof(Texture)=3 ID's)
	for (int i = 0; i < sizeof(Texture); i++)	// цикл по всем ID (изображений)
	{
		// Создание текстуры
		hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(Texture[i]), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	
		if (hBMP)				// существует ли изображение?
		{						// если да …

			GetObject(hBMP,sizeof(BMP), &BMP);

			// режим сохранения пикселей (равнение по двойному слову / 4 Bytes)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
			glBindTexture(GL_TEXTURE_2D, textures[i]);  // связываемся с нашей текстурой

			// линейная фильтрация
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

			// линейная фильтрация Mipmap GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			
			// генерация Mipmapped текстуры (3 байта, ширина, высота и данные из BMP)
			//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BMP.bmWidth, BMP.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
			
			DeleteObject(hBMP);              // удаление объекта изображения
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
	glClearColor(FOG_COLOR);								// Очистка экрана в черный цвет
	glClearDepth( 1.0f );										// Разрешить очистку буфера глубины

	glEnable( GL_DEPTH_TEST );									// Разрешить тест глубины

	glDepthFunc( GL_LEQUAL );									// Тип теста глубины
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );		// Улучшение в вычислении перспективы

	//glClearColor (10.3, 0.3, 0.3, 1.0);

	// рассчет освещения 
	//glEnable(GL_LIGHTING);

	// рассчет текстур
	glEnable(GL_TEXTURE_2D);
	LoadGLTextures();
	
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
	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, TILE_SIZE + MAX_VIEV_DIST);
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

void GLWindow::GetFrameTime()
{
	double currentTime = (double)timeGetTime() * 0.025;

    static double frameTime = currentTime;  // Время последнего кадра

    //Интервал времени, прошедшего с прошлого кадра
    g_FrameInterval = currentTime - frameTime;

    frameTime = currentTime;
	//g_FrameInterval = 0.1;
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
	//Выделение куба
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor3d(0.1, 0.1, 0.1);
	glLineWidth (2.0);
	glBegin(GL_QUADS);
	if (FindTile(player.xx,player.yy,player.zz))
		for(int i = 0; i < 6; i++)
			GlTile(player.xx,player.yy,player.zz,i);
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

void GLWindow::GlTile(signed short  X, signed short Y, signed short Z, char N)
{
	GLdouble dXcoord = X*TILE_SIZE, dYcoord = Y*TILE_SIZE, dZcoord = Z*TILE_SIZE;

	dXcoord -= TILE_SIZE/2;
	dZcoord -= TILE_SIZE/2;

	switch(N)
	{
	case 0:
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
	case 1:
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
	case 2:
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
	case 3:
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
	case 4:
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
	case 5:
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
