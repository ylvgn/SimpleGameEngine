#include <sge_prog_win5.h>

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#pragma comment(lib, "Winmm.lib")

#include <stdio.h>
#include <windows.h>

#include <windowsx.h>
#include <stdint.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

#if _DEBUG
#pragma comment( linker, "/subsystem:console" )
int main(int argc, const TCHAR** argv) {
	return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLineA(), SW_SHOWDEFAULT);
}
#else
#pragma comment( linker, "/subsystem:windows" )
#endif


// LRESULT -> L means long -> 32-bit int
// Win16: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam)

// Byte  (1 byte)
// WORD  (2 bytes) <--- 16 bit, 'W' PARAM
// DWORD (4 bytes) DW --> double WORD : 32 bit
// Long  (4 bytes)

// LPARAM (4 bytes) 32 bit
	// LWORD (16 bit) L low-order,  [0~15] in LPARAM
	// HWORD (16 bit) H high-order, [16~31] in LPARAM

void testOut(HDC hdc, int x, int y, wchar_t* text) {
	if (!text) return;
	TextOut(hdc, x, y, text, lstrlenW(text)); // wcslen(text)
}

void drawText(HDC hdc, wchar_t* text, RECT& rect) {
	if (!text) return;
	DrawText(hdc, text, lstrlenW(text)/*-1*/, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	// "WM" -> window message
	switch (message)
	{
	case WM_CREATE: {
		PlaySound(TEXT("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC);
		return 0;
	} break;
	case WM_CLOSE: {
		int ret = MessageBox(hWnd, L"Quit?", L"My Title", MB_YESNO);
		if (ret == IDYES) {
			DestroyWindow(hWnd); // or DefWindowProc
		}
		return 0;
	} break;
	case WM_SIZE: {

		int width  = GET_X_LPARAM(lParam); // int width  = lParam & 0xffff;
		int height = GET_Y_LPARAM(lParam); // int height = lParam >> 16 & 0xffff;
		printf("SIZE %d %d\n", width, height);

	} break;

	case WM_LBUTTONDOWN: { // add scope cuz ctro a variable inside.
		printf("button down\n");
		auto x = GET_X_LPARAM(lParam); // int x = (int16_t)(lParam & 0xffff);  // low-order word specifies the x-coordinate  (16 bit)
		auto y = GET_Y_LPARAM(lParam); // int y = (int16_t)((lParam >> 16) & 0xffff); // high-order word specifies the y-coordinate (16 bit)

		HDC hdc = GetDC(hWnd);
		{
			Rectangle(hdc, x, y, x + 10, y + 10);
			testOut(hdc, x, y, L"Hello");

			POINT pt;
			MoveToEx(hdc, x, y, &pt);
			LineTo(hdc, x + 100, y);
		}
		ReleaseDC(hWnd, hdc);

		printf("Left Button Down %d %d\n", x, y);
		break;
	}
	case WM_PAINT: {
		RECT rect;
		PAINTSTRUCT ps;
		GetClientRect(hWnd, &rect);
		HDC hdc = BeginPaint(hWnd, &ps);
		{
			int sx = GetSystemMetrics(SM_CXSCREEN); // SM -> System Metrics
			int sy = GetSystemMetrics(SM_CYSCREEN); // The height of the screen primary display monitor in pixels (main display monitor)

			/*
				MessageBoxPrintf(L"ScrnSize",
								 L"The screen is %i pixels wide by %i pixels high.",
								 sx, sy);
			*/
			TCHAR   szBuffer[1024];
			swprintf_s(szBuffer, 1024, L"The display monitor is %i pixels wide by %i pixels high.", sx, sy);
			drawText(hdc, szBuffer, rect);
		}
		EndPaint(hWnd, &ps);
	} break;
	case WM_RBUTTONDOWN: {
#if 0
		MessageBox(hWnd, L"My message", L"My Title", MB_OK | MB_ICONWARNING); // modal
#else
		int ret = MessageBox(hWnd, L"My message", L"My Title", MB_YESNO);
		if (ret == IDNO) {
			printf("NO !");
		}
#endif
	}break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int CDECL MessageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...) {
	TCHAR   szBuffer[1024];
	va_list pArgList;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	// pArgList = (char *) &szFormat + sizeof (szFormat) ;

	va_start(pArgList, szFormat);

	// The last argument to wvsprintf points to the arguments
	 
//	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList); // ProgWin5-Chap02 : _vsntprintf is deprecated
	_vsnwprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList); // corrects this warning using safe string manipulation: https://learn.microsoft.com/en-us/cpp/code-quality/c6053?view=msvc-170

	// The va_end macro just zeroes out pArgList for no good reason

	va_end(pArgList);

	return MessageBox(NULL, szBuffer, szCaption, 0); //  prefix "MB" -> message box
}

void g_setCurrentDir() {
	using namespace sge;

	wchar_t tmp[MAX_PATH + 1];
	if (::GetModuleFileName(nullptr, tmp, MAX_PATH))
	{
		String exeFilePath = UtfUtil::toString(tmp);
		sge::String workingDir = sge::FilePath::dirname(exeFilePath);
		workingDir.append("/../../../../../../../examples/Test103");

		sge::Directory::setCurrent(workingDir);
	}

	SGE_LOG("current dir={}", Directory::getCurrent());
}

// H      -> Handle
// LPSTR  -> Long Pointer String is an artifact of 16-bit Windows, long means 32-bit int, so long pointer is 32-bit pointer; PSTR means 16-bit pointer
// WINAPI -> __stdcall function calling convention -> involves how machine code is generated to place function call arguments on the stack

// Win16: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
// HINSTANCE -> instance handle, is simply a number, to uniquely identifies the program.
// hPrevInstance is always NULL(0) in Win32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow) {

	g_setCurrentDir();

	HWND        hWnd;
	MSG			msg;

	int cy = GetSystemMetrics(SM_CYCAPTION); // The height of a caption area, in pixels.
	printf("caption = %d\n", cy);

	const LPWSTR szWindowClass = L"My Window";

#if 0
	WNDCLASS	wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));

	wc.style         = CS_HREDRAW | CS_VREDRAW ;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName  = NULL ;
    wc.lpszClassName = szWindowClass;

	if (!RegisterClass(&wc)) {
		return 1;
	}

	hWnd = CreateWindow (szWindowClass,			// window class name
						 szWindowClass,			// window caption
						 WS_OVERLAPPEDWINDOW,	// window style, "WS" -> window style
						 CW_USEDEFAULT,			// initial x position
						 CW_USEDEFAULT,			// initial y position
						 640,					// initial x size
						 488,					// initial y size, "CW" -> create window
						 nullptr,				// parent window handle
						 nullptr,				// window menu handle
						 hInstance,				// program instance handle
						 nullptr);				// creation parameters

#else
	WNDCLASSEX	wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

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

	// dwExStyle -> double word extend style
	// "WS" -> window style
	hWnd = CreateWindowEx(0,
						  szWindowClass,
						  szWindowClass,
						  WS_OVERLAPPEDWINDOW /* & ~WS_THICKFRAME*/, // WS -> window style
//						  10, 10, 320, 200, // xywh
						  CW_USEDEFAULT, CW_USEDEFAULT, 640, 488, // CW -> create window
						  nullptr, nullptr, hInstance, nullptr);
#endif

	ShowWindow(hWnd, iCmdShow); // ShowWindow(hWnd, SW_MINIMIZE); // SW -> show window
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); // dispatch msg will call WndProc
	}

	return (int)msg.wParam;
}