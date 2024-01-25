#if SGE_OS_WINDOWS

#include "PW5_MyMessageBox.h"

namespace sge {

void PW5_MyMessageBox::onCreate(CreateDesc& desc) {
	// 'MB' -> message box
	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
	int res = MessageBox(nullptr, L"My message", L"", MB_APPLMODAL | MB_RIGHT | MB_ICONEXCLAMATION | MB_YESNOCANCEL);
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