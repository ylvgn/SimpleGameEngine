#include <stdio.h>
#include <windows.h>

#include <windowsx.h>
#include <stdint.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

#if _DEBUG
#pragma comment( linker, "/subsystem:console" )
int main(int argc, const char** argv) {
	return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), SW_SHOWDEFAULT);
}
#else
#pragma comment( linker, "/subsystem:windows" )
#endif


// LRESULT -> L means long -> 32 bit int
// Win16: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam)

// Byte  (1 byte)
// WORD  (2 bytes) <--- 16 bit, 'W' PARAM
// DWORD (4 bytes) DW --> double WORD
// Long  (4 bytes)

// LPARAM (4 bytes) 32 bit
	// LWORD (16 bit) L low-order,  [0~15] in LPARAM
	// HWORD (16 bit) H high-order, [16~31] in LPARAM
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_LBUTTONDOWN: { // add scope cuz ctro a variable inside.
//		printf("button down\n");
#if 1
		auto x = (int16_t)(lParam & 0xffff);         // low-order word specifies the x-coordinate  (16 bit)
		auto y = (int16_t)((lParam >> 16) & 0xffff); // high-order word specifies the y-coordinate (16 bit)
#else
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);
#endif

		printf("Left Button Down %d %d\n", x, y);
		break;
	}
	case WM_DESTROY: // WM -> window message
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


// H      -> Handle
// LPSTR  -> Long Pointer String is an artifact of 16-bit Windows
// WINAPI -> __stdcall function calling convention

// Win16: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,  int nCmdShow) {

	const LPSTR szWindowClass = "My Window";

	// Window Class Extend
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		return 1;
	}

	HWND hWnd = CreateWindow (  szWindowClass,
								szWindowClass,
								WS_OVERLAPPEDWINDOW, // WS -> window style
//								10, 10, 320, 200, // xywh
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // CW -> create window
								nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, nCmdShow);
//	ShowWindow(hWnd, SW_MINIMIZE); // SW -> show window
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); // dispatch msg will call WndProc
	}

	return (int)msg.wParam;
}