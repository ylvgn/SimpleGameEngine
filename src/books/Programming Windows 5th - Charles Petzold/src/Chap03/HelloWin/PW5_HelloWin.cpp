#include "PW5_HelloWin.h"

#pragma comment(lib, "Winmm.lib")

namespace sge {

// Byte  (1 byte)
// WORD  (2 bytes) 16 bit
// DWORD (4 bytes) 32 bit, DW means double WORD
// Long  (4 bytes)
void PW5_HelloWin::onCreate(CreateDesc& desc) {

	// static const LPWSTR   szAppName = L"PW5_HelloWin";
	// static const wchar_t* szAppName = L"PW5_HelloWin";
	// static const TCHAR*	 szAppName = TEXT("PW5_HelloWin");
	static const TCHAR		 szAppName[] = TEXT ("PW5_HelloWin");

    WNDCLASS wndclass;
	my_bzero(wndclass);

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
    wndclass.lpszClassName	= szAppName;

    if (!RegisterClass (&wndclass)) {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"),  szAppName, MB_ICONERROR);
		throw SGE_ERROR("error RegisterClassEx");
    }
     
	HWND hwnd = CreateWindow (szAppName,            // window class name
                        TEXT ("The Hello Program"), // window caption
                        WS_OVERLAPPEDWINDOW,        // window style
                        CW_USEDEFAULT,              // initial x position
                        CW_USEDEFAULT,              // initial y position
                        CW_USEDEFAULT,              // initial x size
                        CW_USEDEFAULT,              // initial y size
                        NULL,                       // parent window handle
                        NULL,                       // window menu handle
                        hInstance,                  // program instance handle
                        NULL);                      // creation parameters
     
    ShowWindow (hwnd, SW_SHOWDEFAULT);
    UpdateWindow (hwnd);
}

// LRESULT int (32 bit), L means long
// Win16: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam)
LRESULT CALLBACK PW5_HelloWin::s_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
// LPARAM (4 bytes) 32 bit
	// LWORD (16 bit) L low-order,  [ 0~15] in LPARAM
	// HWORD (16 bit) H high-order, [16~31] in LPARAM

    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;
     
    switch (message)
    {
    case WM_CREATE:
        PlaySound (TEXT ("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
		{
			GetClientRect (hwnd, &rect);
			DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
        EndPaint (hwnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage (0);
        return 0;
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

}
