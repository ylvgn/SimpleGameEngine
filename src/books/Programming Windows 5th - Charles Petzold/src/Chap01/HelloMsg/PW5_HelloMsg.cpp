#if SGE_OS_WINDOWS

#include "PW5_HelloMsg.h"

namespace sge {

void PW5_HelloMsg::onCreate(CreateDesc& desc) {
	MessageBox(NULL, TEXT("Hello, Windows 98!"), TEXT("PW5_HelloMsg"), 0);
}

}

#endif // SGE_OS_WINDOWS
