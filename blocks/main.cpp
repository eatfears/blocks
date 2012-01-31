#include <list>

#include "blocks.h"


#pragma comment( lib, "opengl32.lib" ) // Искать OpenGL32.lib при линковке
#pragma comment( lib, "glu32.lib" )    // Искать GLu32.lib при линковке
#pragma comment( lib, "Glaux.lib" )    // Искать Glaux.lib при линковке

using namespace std;

#include "gsl/gsl_randist.h"
gsl_rng *randNumGen;

GLWindow glwWnd;
Game gGame;

LRESULT CALLBACK WndProc(  HWND  hWnd,      // Дескриптор нужного окна
	UINT  uMsg,								// Сообщение для этого окна
	WPARAM  wParam,							// Дополнительная информация
	LPARAM  lParam)							// Дополнительная информация
{
	static POINT ptLastMousePosit;
	static POINT ptCurrentMousePosit;
	static RECT rcClient; 

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
			gGame.player.bKeyboard[wParam] = true;			// Если так, мы присваиваем этой ячейке true
			return 0;						// Возвращаемся
		}

	case WM_KEYUP:							// Была ли отпущена клавиша?
		{
			gGame.player.bKeyboard[wParam] = false;			//  Если так, мы присваиваем этой ячейке false
			gGame.player.bKeyboardDown[wParam] = true;			//  Если так, мы присваиваем этой ячейке false
			return 0;						// Возвращаемся
		}
		
/*	case WM_LBUTTONDOWN:
		{

			ptLastMousePosit.x = ptCurrentMousePosit.x = LOWORD (lParam);
			ptLastMousePosit.y = ptCurrentMousePosit.y = HIWORD (lParam);
			bMousing = true;
		}
		break;*/
/*
	case WM_LBUTTONUP:
		{
			bMousing = false;
		}
		break;
		*/
	case WM_MOUSEMOVE:						
		{
			if(glwWnd.bMousing)
			{
				glwWnd.bMousing = false;
				ptCurrentMousePosit.x = LOWORD (lParam);
				ptCurrentMousePosit.y = HIWORD (lParam);

				{
					gGame.player.dSpinY -= (ptCurrentMousePosit.x - ptLastMousePosit.x)/MOUSE_SENSIVITY;
					gGame.player.dSpinX -= (ptCurrentMousePosit.y - ptLastMousePosit.y)/MOUSE_SENSIVITY;
				}

				while(gGame.player.dSpinY >= 360.0)
					gGame.player.dSpinY -= 360.0;

				while(gGame.player.dSpinY < 0.0)
					gGame.player.dSpinY += 360.0;

				if(gGame.player.dSpinX < -90.0) gGame.player.dSpinX = -90.0;
				if(gGame.player.dSpinX > 90.0) gGame.player.dSpinX = 90.0;

				ptLastMousePosit.x = ptCurrentMousePosit.x;
				ptLastMousePosit.y = ptCurrentMousePosit.y;

				GetWindowRect(hWnd, &rcClient); 
				SetCursorPos((rcClient.left+rcClient.right)/2,(rcClient.top + rcClient.bottom)/2);
			}
			else 
			{
				glwWnd.bMousing = true;
				ptLastMousePosit.x = LOWORD (lParam);
				ptLastMousePosit.y = HIWORD (lParam);
			}

			return 0;						// Возвращаемся
		}

	case WM_SIZE:							// Изменены размеры OpenGL окна
		{
			glwWnd.ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );	// Младшее слово=Width, старшее слово=Height
			return 0;						// Возвращаемся
		}

		break;
	}

	// пересылаем все необработанные сообщения DefWindowProc
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


int WINAPI WinMain(  HINSTANCE  hInstance,
	HINSTANCE  hPrevInstance,
	LPSTR    lpCmdLine,						// Параметры командной строки
	int    nCmdShow )						// Состояние отображения окна
{
	MSG  msg;
	BOOL  done = false;
	ShowCursor(FALSE);

	// Создать наше OpenGL окно
	if(!glwWnd.CreateGLWindow("Blocks", RESX, RESY, BITS))
	{
		return 0;
	}

// 	gGame.player.lnwPositionX = 0;
// 	gGame.player.lnwPositionZ = 0;
	gGame.player.dPositionY = 100*BLOCK_SIZE+00.0;
	gGame.player.dSpinY = -90 - 45;
	gGame.InitGame(&glwWnd);

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
			// Прорисовываем сцену
			if(glwWnd.active)					// Активна ли программа?
			{
				done = gGame.Loop();
			}
		}
	}

	gsl_rng_free(randNumGen);
	glwWnd.KillGLWindow();						// Разрушаем окно
	return ( msg.wParam );					// Выходим из программы
}

