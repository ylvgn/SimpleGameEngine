
#include "ConsoleApp.h"
#include <sge_core/log/Log.h>

namespace sge {

#if 0
#pragma mark ========= ConsoleApp_Arguments ============
#endif
ConsoleApp_Arguments* ConsoleApp_Arguments::s_instance = nullptr;

ConsoleApp_Arguments* ConsoleApp_Arguments::instance() {
	return s_instance;
}

ConsoleApp_Arguments::ConsoleApp_Arguments(int argc, const char* argv[]) {
	SGE_ASSERT(!s_instance);
	s_instance = this;

#if SGE_OS_WINDOWS
	int argCount = 0;
	auto* arr = CommandLineToArgvW(GetCommandLineW(), &argCount);
	_argsStr.resize(argCount);
	for (int i = 0; i < argCount; ++i) {
		_argsStr[i] = UtfUtil::toString(arr[i]);
	}
#else
	_argsStr.resize(argc);
	for (int i = 0; i < argc; ++i) {
		_argsStr[i] = argv[i];
	}
#endif

	_argsView.resize(_argsStr.size());
	for (int i = 0; i < _argsStr.size(); ++i) {
		_argsView[i] = _argsStr[i];
	}
}

ConsoleApp_Arguments::~ConsoleApp_Arguments() {
	SGE_ASSERT(s_instance == this);
	s_instance = nullptr;
}


#if 0
#pragma mark ========= ConsoleApp ============
#endif
Span<const StrView> ConsoleApp::commandArguments() const {
	auto* p = ConsoleApp_Arguments::instance();
	return p ? p->args() : Span<const StrView>();
}

StrView ConsoleApp::appName() {
	if (_appName.empty()) {
		_appName = getExecutableFilename();
	}
	return _appName;
}

}