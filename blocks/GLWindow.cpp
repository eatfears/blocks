#include "GLWindow.h"
#include "Blocks_Definitions.h"

LRESULT  CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

GLWindow::GLWindow()
{
	active = true;
	bMousing = false;
	fullscreen = false;
}

GLWindow::~GLWindow()			// Корректное разрушение окна
{
}

void GLWindow::KillGLWindow()
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

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

int GLWindow::InitGL()										// Все установки касаемо OpenGL происходят здесь
{
	glShadeModel(GL_SMOOTH);									// Разрешить плавное цветовое сглаживание
	glClearColor(FOG_COLOR);									// Очистка экрана в черный цвет
	glClearDepth(1.0f);											// Разрешить очистку буфера глубины

	glEnable(GL_DEPTH_TEST);									// Разрешить тест глубины

	glDepthFunc(GL_LEQUAL);										// Тип теста глубины
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Улучшение в вычислении перспективы

	//рассчет освещения 
	//glEnable(GL_LIGHTING);

	//рассчет текстур
	glEnable(GL_TEXTURE_2D);
	
	//двухсторонний расчет освещения 
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	//автоматическое приведение нормалей к единичной длине
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

	//Вычисление соотношения геометрических размеров для окна
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
		dwExStyle  =   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// Расширенный стиль окна
		dwStyle    =   WS_OVERLAPPEDWINDOW;				// Обычный стиль окна
	}
	
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

	if(!InitGL())							// Инициализация только что созданного окна
	{
		KillGLWindow();						// Восстановить экран

		MessageBox( NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;						// Вернуть false
	}

	return true;							// Всё в порядке!
}
