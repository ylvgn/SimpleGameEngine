#pragma once

#include "AppBase.h"

namespace sge {

#if 0
#pragma mark ========= ConsoleApp_Arguments ============
#endif
class ConsoleApp_Arguments : public NonCopyable {
public:
	ConsoleApp_Arguments(int argc, const char* argv[]);
	~ConsoleApp_Arguments();

	static ConsoleApp_Arguments* instance();

	Span<const StrView> args() const { return _argsView.span(); }

private:
	static ConsoleApp_Arguments* s_instance;

	Vector<StrView, 8>	_argsView;
	Vector<String, 8>	_argsStr;
};

#if 0
#pragma mark ========= ConsoleApp ============
#endif
class ConsoleApp : public AppBase {
public:
	using Arguments = ConsoleApp_Arguments;

	Span<const StrView> commandArguments() const;

	void	setAppName(StrView s) { _appName = s; }
	StrView	appName();

	void run() { onRun(); }

protected:
	virtual void onRun() {}
private:
	String	_appName;
};

} // namespace sge


//----------
template<class T> inline
int sgeMain(int argc, const char* argv[]) {
	SGE_STATIC_ASSERT(sge::TypeTraits::isBaseOf<sge::ConsoleApp, T>::value);

	sge::ConsoleApp_Arguments arg(argc, argv);
	T app;
	app.run();
	return 0;
}

#if SGE_OS_WINDOWS
	#define SGE_Main(T) \
		int main(int argc, const char* argv[]) { \
			try { \
				return sgeMain<T>(argc, argv); \
			} \
			catch (...) { \
				SGE_ASSERT(false); \
				throw; \
			} \
		} \
	//--------
#else
	#define SGE_WinMain(T) \
		int main(int argc, const char* argv[]) { return sgeMain<T>(argc, argv); } \
	//--------
#endif // SGE_OS_WINDOWS