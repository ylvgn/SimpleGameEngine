#include "AppBase.h"

namespace sge {

String AppBase::getExecutableFilename() {
#if SGE_OS_WINDOWS
	wchar_t tmp[MAX_PATH + 1];
	if (!GetModuleFileName(nullptr, tmp, MAX_PATH))
		throw SGE_ERROR("");

	tmp[MAX_PATH] = L'\0';

	String o = UtfUtil::toString(tmp);
	return o;
#endif
}

}