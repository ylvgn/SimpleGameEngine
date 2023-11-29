#pragma once

#include <sge_core.h>

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdint.h>

// H      -> Handle
// LPSTR  -> Long Pointer String is an artifact of 16-bit Windows, long means 32-bit int, so long pointer is 32-bit pointer; PSTR means 16-bit pointer
// WINAPI -> __stdcall function calling convention -> involves how machine code is generated to place function call arguments on the stack
// HINSTANCE -> instance handle, is simply a number, to uniquely identifies the program.
// hPrevInstance is always NULL(0) in Win32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow);
// Win16: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace sge {

template<class T> inline constexpr
void my_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

inline
void testOut(HDC hdc, int x, int y, wchar_t* text) {
	if (!text) return;
	TextOut(hdc, x, y, text, lstrlenW(text)); // wcslen(text)
}

inline
void drawText(HDC hdc, wchar_t* text, RECT& rect) {
	if (!text) return;
	DrawText(hdc, text, lstrlenW(text)/*-1*/, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

}