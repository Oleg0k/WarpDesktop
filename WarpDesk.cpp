#include <windows.h>	// Header File For Windows
#include "resource.h"
#include "Action.h"
#include "Mathematics.h"

extern GLvoid InitGL(GLsizei Width, GLsizei Height, HBITMAP hBitmapDesk );
extern GLvoid DrawGLScene(GLvoid);
extern GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height);
extern void KeysControl(BOOL* keys);

static	HGLRC hRC;		// Permanent Rendering Context
static	HDC hDC, hdc, hdcMem; // Private GDI Device Context

int Width, Height;
// long FPSCounter;

extern HWND hWndMain;

HBITMAP hBitmapDesk ;

BOOL	keys[256];		// Array Used For The Keyboard Routine


LRESULT CALLBACK WndProc(	HWND	hWnd,
				UINT	message,
				WPARAM	wParam,
				LPARAM	lParam)
{
	RECT	Screen;							// Used Later On To Get The Size Of The Window
	GLuint	PixelFormat;
// 	HDC hdc;

	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),		// Size Of This Pixel Format Descriptor
		1,									// Version Number (?)
		PFD_DRAW_TO_WINDOW |				// Format Must Support Window
		PFD_SUPPORT_OPENGL |				// Format Must Support OpenGL
        PFD_GENERIC_ACCELERATED |
		PFD_DOUBLEBUFFER,					// Must Support Double Buffering
		PFD_TYPE_RGBA,						// Request An RGBA Format
		32,									// Select A 16Bit Color Depth
		0, 0, 0, 0, 0, 0,					// Color Bits Ignored (?)
		0,									// No Alpha Buffer
		0,									// Shift Bit Ignored (?)
		0,									// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored (?)
		32,									// 16Bit Z-Buffer (Depth Buffer)  
		0,									// No Stencil Buffer
		0,									// No Auxiliary Buffer (?)
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,									// Reserved (?)
		0, 0, 0								// Layer Masks Ignored (?)
	};

	switch (message)						// Tells Windows We Want To Check The Message
	{
		case WM_CREATE:
			hDC = GetDC(hWnd);				// Gets A Device Context For The Window

			PixelFormat = ChoosePixelFormat(hDC, &pfd);		// Finds The Closest Match To The Pixel Format We Set Above

			if (!PixelFormat)
			{
				MessageBox(0,"Can't Find A Suitable PixelFormat.","Error",MB_OK|MB_ICONERROR);
				PostQuitMessage(0);			// This Sends A 'Message' Telling The Program To Quit
				break;						// Prevents The Rest Of The Code From Running
			}

			if(!SetPixelFormat(hDC,PixelFormat,&pfd))
			{
				MessageBox(0,"Can't Set The PixelFormat.","Error",MB_OK|MB_ICONERROR);
				PostQuitMessage(0);
				break;
			}

			hRC = wglCreateContext(hDC);
			if(!hRC)
			{
				MessageBox(0,"Can't Create A GL Rendering Context.","Error",MB_OK|MB_ICONERROR);
				PostQuitMessage(0);
				break;
			}

			if(!wglMakeCurrent(hDC, hRC))
			{
				MessageBox(0,"Can't activate GLRC.","Error",MB_OK|MB_ICONERROR);
				PostQuitMessage(0);
				break;
			}

			GetClientRect(hWnd, &Screen);
			InitGL(Screen.right, Screen.bottom, hBitmapDesk);
			hWndMain = hWnd;

			break;
		case WM_PAINT:
  		DrawGLScene();
	  	SwapBuffers(hDC);

		case WM_TIMER:

			break;
		case WM_DESTROY:
			DeleteObject(hBitmapDesk);
			break;
		case WM_CLOSE:
			ChangeDisplaySettings(NULL, 0);
 			wglMakeCurrent(hDC,NULL);
			wglDeleteContext(hRC);
			ReleaseDC(hWnd,hDC);
//			KillTimer(hWnd,1);
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
			keys[wParam] = TRUE;
			KeysControl(keys);
			break;
		case WM_KEYUP:
			keys[wParam] = FALSE;
			break;

		case WM_SIZE:
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
return (0);
}

int WINAPI WinMain(	HINSTANCE	hInstance, 
			HINSTANCE	hPrevInstance, 
			LPSTR		lpCmdLine, 
			int			nCmdShow)
{
	MSG			msg;		// Windows Message Structure
	WNDCLASS	wc;			// Windows Class Structure Used To Set Up The Type Of Window
	HWND		hWnd;		// Storage For Window Handle

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL WinClass";

	if(!RegisterClass(&wc))
	{
		MessageBox(0,"Failed To Register The Window Class.","Error",MB_OK|MB_ICONERROR);
		return FALSE;
	};

	Width=GetSystemMetrics(SM_CXSCREEN);
	Height=GetSystemMetrics(SM_CYSCREEN);

	hdc=GetWindowDC(GetDesktopWindow());
	hdcMem=CreateCompatibleDC(hdc);
	hBitmapDesk=CreateCompatibleBitmap(hdc, Width, Height);
	SelectObject(hdcMem,hBitmapDesk);
	BitBlt(hdcMem, 0, 0, Width, Height, hdc , 0, 0, SRCCOPY) ;

	hWnd = CreateWindow(
	"OpenGL WinClass",
	"Warped Desktop",		// Title Appearing At The Top Of The Window

	WS_POPUP |
	WS_CLIPCHILDREN |
	WS_CLIPSIBLINGS,

	0, 0,												// The Position Of The Window On The Screen
	Width, Height,											// The Width And Height Of The WIndow

	NULL,
	NULL,
	hInstance,
	NULL);

	if(!hWnd)
	{
		MessageBox(0,"Window Creation Error.","Error",MB_OK|MB_ICONERROR);
		return FALSE;
	}
/*
	DEVMODE dmScreenSettings ;
	dmScreenSettings.dmSize				= sizeof(DEVMODE);
	dmScreenSettings.dmPelsWidth        = 640;								// Width
	dmScreenSettings.dmPelsHeight       = 480;								// Height
	dmScreenSettings.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT;		// Color Depth
	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);				// Switch To Fullscreen Mode
*/
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

//	SetTimer(hWnd,1,1000,NULL);

	SetFocus(hWnd);
	wglMakeCurrent(hDC,hRC);

	while (1)
	{
		// Process All Messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				return TRUE;
			}
		}

	}// while

}
