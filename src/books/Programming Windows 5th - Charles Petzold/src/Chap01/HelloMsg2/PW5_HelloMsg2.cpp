#include "PW5_HelloMsg2.h"

#if SGE_OS_WINDOWS

namespace sge {

void PW5_HelloMsg2::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	// 'MB' -> message box
	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
	int res = MessageBox(_hwnd, L"my message", L"my caption", MB_APPLMODAL | MB_RIGHT | MB_ICONEXCLAMATION | MB_YESNOCANCEL);
	if (res == IDYES) {
		SGE_LOG("Click Yes!");
	} else if (res == IDNO) {
		SGE_LOG("Click No!");
	} else if (res == IDCANCEL) {
		SGE_LOG("Click Cancel!");
	}
}

}

#endif // SGE_OS_WINDOWS