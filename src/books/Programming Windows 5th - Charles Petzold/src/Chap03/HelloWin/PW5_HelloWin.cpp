#include "PW5_HelloWin.h"

#if SGE_OS_WINDOWS

#pragma comment(lib, "Winmm.lib") // Windows multimedia

// WINAPI/CALLBACK -> __stdcall function calling convention
	// involves how machine code is generated to place function call arguments on the stack

// window main entry
	// Win32: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow);
		// HINSTANCE -> instance handle, is simply a number, to uniquely identifies the program.
		// hPrevInstance is always NULL in 32 bit versions of Windows
	// Win16: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
		// LPSTR == PSTR -> char* (LPSTR is an artifact of 16 bit Windows),
			// 'NP' -> near pointer, 'LP' -> long pointer
			// There is no differentiation between near and long pointers in Win32, but two different sizes in Win16.

// window procedure
	// Win32: LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		// WPARAM is UINT, LPARAM is LONG, and both are 32 bit value
		// LRESULT is LONG
	// Win16: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam)
		// WPARAM is WORD, unsigned short int (16 bit) 
		// LPARAM is LONG, signed long int (32 bit)

namespace sge {

// BYTE  (1 byte)
// WORD  (2 bytes) 16 bit
// DWORD (4 bytes) 32 bit, 'DW' -> double WORD
// LONG  (4 bytes) 32 bit
void PW5_HelloWin::onCreate(CreateDesc& desc) {

	// static const LPWSTR   clsName = TEXT("PW5_HelloWin");
	// static const wchar_t* clsName = TEXT("PW5_HelloWin");
	// static const TCHAR*	 clsName = TEXT("PW5_HelloWin");
	static const TCHAR		 clsName[] = TEXT ("PW5_HelloWin");

    WNDCLASS wndclass;
	g_bzero(wndclass);

	auto hInstance			= GetModuleHandle(nullptr); // an instance of the program itself
    wndclass.style			= CS_HREDRAW | CS_VREDRAW;	// 'CS' -> class style
    wndclass.cbClsExtra		= 0; // 'cb' -> count of byte, and 'c' -> count
    wndclass.cbWndExtra		= 0;
    wndclass.hIcon			= LoadIcon (NULL, IDI_APPLICATION);	// 'IDI' -> ID of icon
    wndclass.hCursor		= LoadCursor (NULL, IDC_ARROW);		// 'IDC' -> ID of cursor
    wndclass.hbrBackground	= static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)); // 'hbr' -> a handle of brush
    wndclass.lpszMenuName	= NULL;
	// in WNDCLASS two most important fields
    wndclass.hInstance		= hInstance;
    wndclass.lpfnWndProc	= s_wndProc;
    wndclass.lpszClassName	= clsName;

    if (!RegisterClass (&wndclass)) {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"), clsName, MB_ICONERROR);
		throw SGE_ERROR("error RegisterClass");
    }

	DWORD dwStyle = WS_OVERLAPPEDWINDOW /*& ~WS_THICKFRAME*/; // 'WS' -> window style

	// HWND -> Handle to a window object
	HWND hwnd = CreateWindow(clsName,				// window class name
							 TEXT ("PW5 HelloWin"),	// window caption title
							 dwStyle,				// window style
							 CW_USEDEFAULT,			// initial x position, 'CW' -> create window
							 CW_USEDEFAULT,			// initial y position
							 CW_USEDEFAULT,			// initial x size
							 CW_USEDEFAULT,			// initial y size
							 NULL,					// parent window handle
							 NULL,					// window menu handle
							 hInstance,				// program instance handle
							 NULL);					// creation parameters

    ::ShowWindow (hwnd, SW_SHOWDEFAULT); // 'SW' -> show window, The ShowWindow function puts the window on the display
    ::UpdateWindow (hwnd); // Directs the window to paint itself -> WM_PAINT
}

LRESULT CALLBACK PW5_HelloWin::s_wndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		// 'WM' -> window message
		case WM_CREATE:
			// the window procedure of the new window receives this message after the window is created,
			// but before the window becomes visible.
			PlaySound (TEXT ("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC); // 'SND' -> Sound
			return 0;
		case WM_PAINT: {
			::HDC		  hdc;
			::PAINTSTRUCT ps;
			::RECT		  rect;

			hdc = ::BeginPaint (hwnd, &ps); // Initiates the beginning of window painting
			{
				::GetClientRect (hwnd, &rect); // Obtains the dimensions of the window's client area
				DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER); // 'DT' -> draw text
			}
			::EndPaint (hwnd, &ps); // Ends window painting
		} return 0;
		case WM_DESTROY:
			::PostQuitMessage(0); // Inserts a "quit" message into the message queue
			return 0;
	} // switch
    return DefWindowProc (hwnd, message, wParam, lParam); // Performs default processing of messages
}

}

#endif // SGE_OS_WINDOWS