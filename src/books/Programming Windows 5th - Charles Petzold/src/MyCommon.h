#pragma once

#include <sge_core.h>

#if SGE_OS_WINDOWS

// main entry
	// Win32: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow);
		// HINSTANCE -> instance handle, is simply a number, to uniquely identifies the program.
		// hPrevInstance is always NULL(0) in Win32
	// Win16: int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
		// LPSTR == PSTR -> char* (LPSTR is an artifact of 16-bit Windows),
			// 'NP' -> near pointer
			// 'LP' -> long pointer
				// There is no differentiation between near and long pointers in Win32, but two different sizes in Win16.

// Window message handler
	// Win32: LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	// Win16: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam)
		// WINAPI/CALLBACK -> __stdcall function calling convention
			// involves how machine code is generated to place function call arguments on the stack

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdint.h>
#include <tchar.h>

namespace sge {

template<class T> constexpr
void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

}

#endif // SGE_OS_WINDOWS