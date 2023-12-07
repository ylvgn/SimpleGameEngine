#include "PW5_HelloWin.h"

#if SGE_OS_WINDOWS

#pragma comment(lib, "Winmm.lib") // Windows multimedia

namespace sge {

// Byte  (1 byte)
// WORD  (2 bytes) 16 bit
// DWORD (4 bytes) 32 bit, 'DW' -> double WORD
// Long  (4 bytes)
void PW5_HelloWin::onCreate(CreateDesc& desc) {

	// static const LPWSTR   clsName = TEXT("PW5_HelloWin");
	// static const wchar_t* clsName = TEXT("PW5_HelloWin");
	// static const TCHAR*	 clsName = TEXT("PW5_HelloWin");
	static const TCHAR		 clsName[] = TEXT ("PW5_HelloWin");

    WNDCLASS wndclass;
	g_bzero(wndclass);

	auto hInstance			= GetModuleHandle(nullptr);
    wndclass.style			= CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc	= s_WndProc;
    wndclass.cbClsExtra		= 0;
    wndclass.cbWndExtra		= 0;
    wndclass.hInstance		= hInstance;
    wndclass.hIcon			= LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor		= LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground	= (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName	= NULL;

    wndclass.lpszClassName	= clsName;

    if (!RegisterClass (&wndclass)) {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"), clsName, MB_ICONERROR);
		throw SGE_ERROR("error RegisterClassEx");
    }

	DWORD dwStyle = WS_OVERLAPPEDWINDOW /*& ~WS_THICKFRAME*/; // 'WS' -> window style

	HWND hwnd = CreateWindow(clsName,				// window class name
							 TEXT ("PW5 HelloWin"),	// window caption title
							 dwStyle,				// window style
							 CW_USEDEFAULT,			// initial x position
							 CW_USEDEFAULT,			// initial y position
							 CW_USEDEFAULT,			// initial x size
							 CW_USEDEFAULT,			// initial y size
							 NULL,					// parent window handle
							 NULL,					// window menu handle
							 hInstance,				// program instance handle
							 NULL);					// creation parameters

    ShowWindow (hwnd, SW_SHOWDEFAULT); // 'SW' -> show window
    UpdateWindow (hwnd); // Directs the window to paint itself -> WM_PAINT
}

// LRESULT same as int (32 bit), 'L' -> long
LRESULT CALLBACK PW5_HelloWin::s_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message)
    {
	// 'WM' -> window message
    case WM_CREATE:
		// the window procedure of the new window receives this message after the window is created,
		// but before the window becomes visible.
        PlaySound (TEXT ("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps); // Initiates the beginning of window painting
		{
			GetClientRect (hwnd, &rect); // Obtains the dimensions of the window's client area
			DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER); // Displays a text string
		}
        EndPaint (hwnd, &ps); // Ends window painting
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0); // Inserts a "quit" message into the message queue
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam); // Performs default processing of messages
}

}

#endif // SGE_OS_WINDOWS