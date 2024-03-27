#pragma once
#if SGE_OS_WINDOWS

#include "PW5_GDI.h"

namespace sge {

struct PW5_Win32Util {
	PW5_Win32Util() = delete;

	static ::WNDCLASSEXA createWndClass(::HMODULE hInstance, StrViewA clsName, ::WNDPROC lpfnWndProc, UINT style = 0);
	static ::WNDCLASSEXW createWndClass(::HMODULE hInstance, StrViewW clsName, ::WNDPROC lpfnWndProc, UINT style = 0);

	static void registerWndClass(::WNDCLASSEX& wc);
};

inline
WNDCLASSEXA PW5_Win32Util::createWndClass(::HMODULE hInstance, StrViewA clsName, ::WNDPROC lpfnWndProc, UINT csStyle /*= 0*/) {
	WNDCLASSEXA wc;
	g_bzero(wc);

	wc.cbSize			= sizeof(wc);
    wc.hInstance		= hInstance;
    wc.lpfnWndProc		= lpfnWndProc;
    wc.lpszClassName	= clsName.data();
    wc.style			= CS_HREDRAW | CS_VREDRAW | csStyle;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(hInstance, IDC_ARROW);
    wc.hbrBackground	= nullptr;
    wc.lpszMenuName		= nullptr;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	return wc;
}

inline
WNDCLASSEXW PW5_Win32Util::createWndClass(::HMODULE hInstance, StrViewW clsName, ::WNDPROC lpfnWndProc, UINT csStyle /*= 0*/) {
	WNDCLASSEXW wc;
	g_bzero(wc);

	wc.cbSize			= sizeof(wc);
    wc.hInstance		= hInstance;
    wc.lpfnWndProc		= lpfnWndProc;
    wc.lpszClassName	= clsName.data();
    wc.style			= CS_HREDRAW | CS_VREDRAW | csStyle;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(hInstance, IDC_ARROW);
    wc.hbrBackground	= nullptr;
    wc.lpszMenuName		= nullptr;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	return wc;
}

inline
void PW5_Win32Util::registerWndClass(::WNDCLASSEX& wc) {
	if (0 != ::GetClassInfoEx(wc.hInstance, wc.lpszClassName, nullptr))
		return;

	if (!::RegisterClassEx(&wc)) {
		DWORD e = ::GetLastError();
		switch (e) {
			case ERROR_CALL_NOT_IMPLEMENTED: throw SGE_ERROR("calling RegisterClassW in Windows 98");
			default: throw SGE_ERROR("error g_registerWndClass");
		}
	}
}

}

#endif