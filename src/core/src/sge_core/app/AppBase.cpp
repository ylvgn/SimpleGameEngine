#include "AppBase.h"

namespace sge {
#if 0
#pragma mark ========= AppArguments ============
#endif
AppArguments::AppArguments(int argc, const char* argv[]) {
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

#if 0
#pragma mark ========= AppBase ============
#endif
void AppBase::setCommandArguments(int argc, const char* argv[]) {
	_args = UPtr_make<AppArguments>(argc, argv);
}

Span<const StrView> AppBase::commandArguments() const {
	return _args ? _args->args() : Span<const StrView>();
}

StrView AppBase::appName() {
	if (_appName.empty()) {
		_appName = getExecutableFilename();
	}
	return _appName;
}

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

int AppBase::_run() {
	onRun();
	return 0;
}

} // namespace sge