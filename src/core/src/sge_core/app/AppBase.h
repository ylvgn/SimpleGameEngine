#pragma once

#include <sge_core/string/UtfUtil.h>
#include <sge_core/file/FilePath.h>
#include <sge_core/file/Directory.h>

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
	Vector<String,  8>	_argsStr;
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
	void setCommandArguments(const char* (&argv)[N]) {
		if(N) setCommandArguments(static_cast<int>(N), argv);
	}

	Span<const StrView> commandArguments() const;

	void	setAppName(StrView s) { _appName = s; }
	StrView	appName();

	template<class STR> void executableFilenameTo(STR& out);
	String	executableFilename() { String o; executableFilenameTo(o); return o; }

	template<class STR> void executableDirPathTo(STR& out);
	String	executableDirPath() { String o; executableDirPathTo(o); return o; }

	void	setCurDirRelativeToExecutable(StrView relativePath);

	virtual void onRun() = 0;

private:
	UPtr<AppArguments>	_args;
	String				_appName;
}; // AppBase


#if 0
#pragma mark ========= Windows ============
#endif
#if SGE_OS_WINDOWS

template<class STR> inline
void AppBase::executableFilenameTo(STR& out) {
	StringW_<MAX_PATH> pathW;
	pathW.resizeToLocalBufSize();
	auto requiredSize = ::GetModuleFileName(nullptr, pathW.data(), MAX_PATH);
	if (!requiredSize)
		throw SGE_ERROR("::GetModuleFileName error: {}", ::WSAGetLastError());
	pathW.resize(requiredSize);
	UtfUtil::convert(out, pathW);
	out.replaceChars('\\', '/');
}

template<class STR> inline
void AppBase::executableDirPathTo(STR& out) {
	executableFilenameTo(out);
	auto dir = FilePath::dirname(out.view());
	out.resize(dir.size());
}
#endif  // SGE_OS_WINDOWS

} // namespace sge