#include <vector>
#include <deque>
#include <list>
#include <time.h>

#include "blocks.h"
#include "gsl/gsl_randist.h"
gsl_rng *randNumGen;

#define MOUSE_SENSIVITY 1.5

#define TORAD(x) x*0.01745329251994329576923690768489
#define TODEG(x) x*57.295779513082320876798154814105

#pragma comment( lib, "opengl32.lib" ) // ������ OpenGL32.lib ��� ��������
#pragma comment( lib, "glu32.lib" )    // ������ GLu32.lib ��� ��������

using namespace std;

GLWindow glwWnd;

LRESULT CALLBACK WndProc(  HWND  hWnd,      // ���������� ������� ����
	UINT  uMsg,								// ��������� ��� ����� ����
	WPARAM  wParam,							// �������������� ����������
	LPARAM  lParam)							// �������������� ����������
{
	static POINT ptLastMousePosit;
	static POINT ptCurrentMousePosit;

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
			glwWnd.keys[wParam] = true;			// ���� ���, �� ����������� ���� ������ true
			return 0;						// ������������
		}

	case WM_KEYUP:							// ���� �� �������� �������?
		{
			glwWnd.keys[wParam] = false;			//  ���� ���, �� ����������� ���� ������ false
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
					glwWnd.g_fSpinZ -= (ptCurrentMousePosit.x - ptLastMousePosit.x)/MOUSE_SENSIVITY;
					glwWnd.g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y)/MOUSE_SENSIVITY;
				}

				while(glwWnd.g_fSpinZ >= 360.0)
					glwWnd.g_fSpinZ -= 360.0;

				while(glwWnd.g_fSpinZ > 0.0)
					glwWnd.g_fSpinZ += 360.0;

				if(glwWnd.g_fSpinY < -90.0) glwWnd.g_fSpinY = -90.0;
				if(glwWnd.g_fSpinY > 90.0) glwWnd.g_fSpinY = 90.0;

				ptLastMousePosit.x = ptCurrentMousePosit.x;
				ptLastMousePosit.y = ptCurrentMousePosit.y;


			
			RECT rcClient; 

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
	}

	// ���������� ��� �������������� ��������� DefWindowProc
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


int WINAPI WinMain(  HINSTANCE  hInstance,  // ���������� ����������
	HINSTANCE  hPrevInstance,				// ���������� ������������� ����������
	LPSTR    lpCmdLine,						// ��������� ��������� ������
	int    nCmdShow )						// ��������� ����������� ����
	{
		MSG  msg;								// ��������� ��� �������� ��������� Windows
		BOOL  done = false;						// ���������� ���������� ��� ������ �� �����

		// ������� ���� OpenGL ����
		if( !glwWnd.CreateGLWindow("Blocks", 400, 400, 32) )
		{
			return 0;							// �����, ���� ���� �� ����� ���� �������
		}

		srand(time(NULL));
		randNumGen = gsl_rng_alloc(gsl_rng_mt19937);


		glwWnd.y = 10.0;
		GLfloat step = 1.2;
		ShowCursor(FALSE);

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
				// ������������� �����
				if( glwWnd.active )					// ������� �� ���������?
				{
					if(glwWnd.keys[VK_ESCAPE])			// ���� �� ������ ������� ESC?
					{
						done = true;			// ESC ������� �� �������� ���������� ���������
					}
					else						// �� ����� ��� ������, ������� �����.
					{
						if(glwWnd.keys['W']) 
						{
							glwWnd.x -= step*sin(TORAD(glwWnd.g_fSpinZ));
							glwWnd.z += step*cos(TORAD(glwWnd.g_fSpinZ));
						}
						if(glwWnd.keys['S']) 
						{
							glwWnd.x += step*sin(TORAD(glwWnd.g_fSpinZ));
							glwWnd.z -= step*cos(TORAD(glwWnd.g_fSpinZ));
						}
						if(glwWnd.keys['D']) 
						{
							glwWnd.x += step*cos(TORAD(glwWnd.g_fSpinZ));
							glwWnd.z += step*sin(TORAD(glwWnd.g_fSpinZ));
						}
						if(glwWnd.keys['A']) 
						{
							glwWnd.x -= step*cos(TORAD(glwWnd.g_fSpinZ));
							glwWnd.z -= step*sin(TORAD(glwWnd.g_fSpinZ));
						}

						if(glwWnd.keys['R']) glwWnd.y += step; //r
						if(glwWnd.keys['F']) glwWnd.y -= step; //f

						glwWnd.DrawGLScene();

						SwapBuffers( glwWnd.hDC );		// ������ ����� (������� �����������)
					}
				}
			}
		}

		glwWnd.KillGLWindow();						// ��������� ����
		return ( msg.wParam );					// ������� �� ���������
}

