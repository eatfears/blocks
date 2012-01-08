#include <vector>
#include <deque>
#include <list>
#include <time.h>

#include "blocks.h"
#include "gsl/gsl_randist.h"
gsl_rng *randNumGen;

#define MOUSE_SENSIVITY 1.5


#pragma comment( lib, "opengl32.lib" ) // ������ OpenGL32.lib ��� ��������
#pragma comment( lib, "glu32.lib" )    // ������ GLu32.lib ��� ��������
#pragma comment( lib, "Glaux.lib" )    // ������ Glaux.lib ��� ��������

using namespace std;

GLWindow glwWnd;

LRESULT CALLBACK WndProc(  HWND  hWnd,      // ���������� ������� ����
	UINT  uMsg,								// ��������� ��� ����� ����
	WPARAM  wParam,							// �������������� ����������
	LPARAM  lParam)							// �������������� ����������
{
	static POINT ptLastMousePosit;
	static POINT ptCurrentMousePosit;
	static RECT rcClient; 

	switch (uMsg)							// �������� ��������� ��� ����
	{
	case WM_ACTIVATE:						// �������� ��������� ���������� ����
		{
			if( !HIWORD( wParam ) )			// ��������� ��������� �����������
			{
				glwWnd.active = true;				// ��������� �������
			}
			else
			{
				glwWnd.active = false;				// ��������� ������ �� �������
			}

			return 0;						// ������������ � ���� ��������� ���������
		}

	case WM_SYSCOMMAND:						// ������������� ��������� �������
		{
			switch ( wParam )				// ������������� ��������� �����
			{
			case SC_SCREENSAVE:				// �������� �� ���������� �����������?
			case SC_MONITORPOWER:			// �������� �� ������� ������� � ����� ���������� �������?
				return 0;					// ������������� ���
			}
			break;							// �����
		}

	case WM_CLOSE:							// �� �������� ��������� � ��������?
		{
			PostQuitMessage( 0 );			// ��������� ��������� � ������
			return 0;						// ��������� �����
		}

	case WM_KEYDOWN:						// ���� �� ������ ������?
		{
			glwWnd.player.keys[wParam] = true;			// ���� ���, �� ����������� ���� ������ true
			return 0;						// ������������
		}

	case WM_KEYUP:							// ���� �� �������� �������?
		{
			glwWnd.player.keys[wParam] = false;			//  ���� ���, �� ����������� ���� ������ false
			return 0;						// ������������
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
					glwWnd.player.gfSpinY -= (ptCurrentMousePosit.x - ptLastMousePosit.x)/MOUSE_SENSIVITY;
					glwWnd.player.gfSpinX -= (ptCurrentMousePosit.y - ptLastMousePosit.y)/MOUSE_SENSIVITY;
				}

				while(glwWnd.player.gfSpinY >= 360.0)
					glwWnd.player.gfSpinY -= 360.0;

				while(glwWnd.player.gfSpinY < 0.0)
					glwWnd.player.gfSpinY += 360.0;

				if(glwWnd.player.gfSpinX < -90.0) glwWnd.player.gfSpinX = -90.0;
				if(glwWnd.player.gfSpinX > 90.0) glwWnd.player.gfSpinX = 90.0;

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

			return 0;						// ������������
		}

	case WM_SIZE:							// �������� ������� OpenGL ����
		{
			glwWnd.ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );	// ������� �����=Width, ������� �����=Height
			return 0;						// ������������
		}

	case WM_MOUSEHOVER:
		{
			ShowCursor(FALSE);
		}

	case WM_MOUSELEAVE:
		{
			ShowCursor(TRUE);
		}
		break;
	}

	// ���������� ��� �������������� ��������� DefWindowProc
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


int WINAPI WinMain(  HINSTANCE  hInstance,
	HINSTANCE  hPrevInstance,
	LPSTR    lpCmdLine,						// ��������� ��������� ������
	int    nCmdShow )						// ��������� ����������� ����
{
	MSG  msg;
	BOOL  done = false;
	ShowCursor(FALSE);

	// ������� ���� OpenGL ����
	if(!glwWnd.CreateGLWindow("Blocks", RESX, RESY, BITS))
	{
		return 0;
	}

	srand(time(NULL));
	randNumGen = gsl_rng_alloc(gsl_rng_mt19937);

	glwWnd.player.gfPosY = 20.0;

	while( !done )							// ���� ������������, ���� done �� ����� true
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )	// ���� �� � ������� �����-������ ���������?
		{
			if( msg.message == WM_QUIT )    // �� ������� ��������� � ������?
			{
				done = true;				// ���� ���, done=true
			}
			else							// ���� ���, ������������ ���������
			{
				TranslateMessage( &msg );   // ��������� ���������
				DispatchMessage( &msg );	// �������� ���������
			}
		}
		else								// ���� ��� ���������
		{
			done = glwWnd.Loop();
		}
	}

	glwWnd.KillGLWindow();						// ��������� ����
	return ( msg.wParam );					// ������� �� ���������
}

