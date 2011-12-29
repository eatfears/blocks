#include <vector>
#include <deque>
#include <list>
#include <time.h>

#include "blocks.h"
#include "gsl/gsl_randist.h"
gsl_rng *randNumGen;


#pragma comment( lib, "opengl32.lib" ) // Искать OpenGL32.lib при линковке
#pragma comment( lib, "glu32.lib" )    // Искать GLu32.lib при линковке

using namespace std;

GLWindow glwWnd;

LRESULT CALLBACK WndProc(  HWND  hWnd,      // Дескриптор нужного окна
	UINT  uMsg,								// Сообщение для этого окна
	WPARAM  wParam,							// Дополнительная информация
	LPARAM  lParam)							// Дополнительная информация
{

	srand(time(NULL));
	randNumGen = gsl_rng_alloc(gsl_rng_mt19937);


	switch (uMsg)							// Проверка сообщения для окна
	{
	case WM_ACTIVATE:						// Проверка сообщения активности окна
		{
			if( !HIWORD( wParam ) )			// Проверить состояние минимизации
			{
				glwWnd.active = true;				// Программа активна
			}
			else
			{
				glwWnd.active = false;				// Программа теперь не активна
			}

			return 0;						// Возвращаемся в цикл обработки сообщений
		}

	case WM_SYSCOMMAND:						// Перехватываем системную команду
		{
			switch ( wParam )				// Останавливаем системный вызов
			{
			case SC_SCREENSAVE:				// Пытается ли запустится скринсейвер?
			case SC_MONITORPOWER:			// Пытается ли монитор перейти в режим сбережения энергии?
				return 0;					// Предотвращаем это
			}
			break;							// Выход
		}

	case WM_CLOSE:							// Мы получили сообщение о закрытие?
		{
			PostQuitMessage( 0 );			// Отправить сообщение о выходе
			return 0;						// Вернуться назад
		}

	case WM_KEYDOWN:						// Была ли нажата кнопка?
		{
			glwWnd.keys[wParam] = true;			// Если так, мы присваиваем этой ячейке true
			return 0;						// Возвращаемся
		}

	case WM_KEYUP:							// Была ли отпущена клавиша?
		{
			glwWnd.keys[wParam] = false;			//  Если так, мы присваиваем этой ячейке false
			return 0;						// Возвращаемся
		}

	case WM_SIZE:							// Изменены размеры OpenGL окна
		{
			glwWnd.ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );	// Младшее слово=Width, старшее слово=Height
			return 0;						// Возвращаемся
		}
	}

	// пересылаем все необработанные сообщения DefWindowProc
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


int WINAPI WinMain(  HINSTANCE  hInstance,  // Дескриптор приложения
	HINSTANCE  hPrevInstance,				// Дескриптор родительского приложения
	LPSTR    lpCmdLine,						// Параметры командной строки
	int    nCmdShow )						// Состояние отображения окна
	{
		MSG  msg;								// Структура для хранения сообщения Windows
		BOOL  done = false;						// Логическая переменная для выхода из цикла

		// Создать наше OpenGL окно
		if( !glwWnd.CreateGLWindow("Blocks", 400, 400, 32) )
		{
			return 0;							// Выйти, если окно не может быть создано
		}

		srand(time(NULL));
		randNumGen = gsl_rng_alloc(gsl_rng_mt19937);

		while( !done )							// Цикл продолжается, пока done не равно true
		{
			
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )	// Есть ли в очереди какое-нибудь сообщение?
			{
				if( msg.message == WM_QUIT )    // Мы поучили сообщение о выходе?
				{
					done = true;				// Если так, done=true
				}
				else							// Если нет, обрабатывает сообщения
				{
					TranslateMessage( &msg );   // Переводим сообщение
					DispatchMessage( &msg );	// Отсылаем сообщение
				}
			}
			else								// Если нет сообщений
			{
				// Прорисовываем сцену.
				//if( glwWnd.active )					// Активна ли программа?
				{
					if(glwWnd.keys[VK_ESCAPE])			// Было ли нажата клавиша ESC?
					{
						done = true;			// ESC говорит об останове выполнения программы
					}
					else						// Не время для выхода, обновим экран.
					{
						glwWnd.DrawGLScene();

						SwapBuffers( glwWnd.hDC );		// Меняем буфер (двойная буферизация)
					}
				}
			}
		}

		glwWnd.KillGLWindow();						// Разрушаем окно
		return ( msg.wParam );					// Выходим из программы
}

