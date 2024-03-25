#pragma once
#if SGE_OS_WINDOWS

#include "PW5_GDI.h"

namespace sge {

struct PW5_Win32Util {
	PW5_Win32Util() = delete;

	static WNDCLASSEX createWndClass(HMODULE hInstance, const wchar_t* clsName, WNDPROC lpfnWndProc, UINT style = 0);
	static bool isRegisterWndClass(HMODULE hInstance, const wchar_t* clsName);
	static void registerWndClass(WNDCLASSEX& wc);
};

inline
WNDCLASSEX PW5_Win32Util::createWndClass(HMODULE hInstance, const wchar_t* clsName, WNDPROC lpfnWndProc, UINT csStyle /*= 0*/) {
	WNDCLASSEX wc;
	g_bzero(wc);

	wc.cbSize			= sizeof(WNDCLASSEX);
    wc.hInstance		= hInstance;
    wc.lpfnWndProc		= lpfnWndProc;
    wc.lpszClassName	= clsName;
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
bool PW5_Win32Util::isRegisterWndClass(HMODULE hInstance, const wchar_t* clsName) {
	WNDCLASSEX tmpWc;
	return 0 != ::GetClassInfoEx(hInstance, clsName, &tmpWc);
}

inline
void PW5_Win32Util::registerWndClass(WNDCLASSEX& wc) {
	if (PW5_Win32Util::isRegisterWndClass(wc.hInstance, wc.lpszClassName))
		return;

	if (!::RegisterClassEx(&wc)) {
		DWORD e = GetLastError();
		switch (e) {
			case ERROR_CALL_NOT_IMPLEMENTED: throw SGE_ERROR("calling RegisterClassW in Windows 98");
			default: throw SGE_ERROR("error g_registerWndClass");
		}
	}
}

}

#endif