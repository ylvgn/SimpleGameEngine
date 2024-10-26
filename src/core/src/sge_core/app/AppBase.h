#pragma once

#include <sge_core/string/UtfUtil.h>

namespace sge {
#if 0
#pragma mark ========= AppArguments ============
#endif
class AppArguments : public NonCopyable {
public:
	explicit AppArguments(int argc, const char* argv[]);

	Span<const StrView> args() const { return _argsView.span(); }

private:
	Vector<StrView, 8>	_argsView;
	Vector<String, 8>	_argsStr;
}; // AppArguments

#if 0
#pragma mark ========= AppBase ============
#endif
class AppBase : public NonCopyable {
public:
	AppBase() = default;
	virtual ~AppBase() = default;

	void setCommandArguments(int argc, const char* argv[]);

	template<size_t N>
	void setCommandArguments(const char* (&argv)[N]) { if(N) setCommandArguments(static_cast<int>(N), argv); }

	Span<const StrView> commandArguments() const;

	void	setAppName(StrView s) { _appName = s; }
	StrView	appName();

	String	getExecutableFilename();

	virtual void onRun() = 0;

private:
	UPtr<AppArguments>	_args;
	String				_appName;
}; // AppBase

} // namespace sge