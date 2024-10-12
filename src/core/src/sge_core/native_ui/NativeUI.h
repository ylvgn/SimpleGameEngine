#pragma once

#if SGE_OS_WINDOWS

#include "Win32/NativeUIApp_Win32.h"
#include "Win32/NativeUIWindow_Win32.h"

namespace sge {
	using NativeUIWindow_Impl	= NativeUIWindow_Win32;
	using NativeUIApp_Impl		= NativeUIApp_Win32;
}

#else
	#error "unsupported platform"

#endif


namespace sge {

class NativeUIWindow : public NativeUIWindow_Impl {
};

class NativeUIApp : public NativeUIApp_Impl {
public:
	NativeUIApp() noexcept;
	~NativeUIApp() noexcept;

	static NativeUIApp* current();
private:
	static NativeUIApp* _current;
}; // NativeUIApp

} // namespace sge


//------
template<class T, class ENABLE = void>
struct sgeMain {
	sgeMain() = delete;

	static int run(T& app) {
		SGE_STATIC_ASSERT(sge::TypeTraits::isBaseOf<sge::AppBase, T>::value);
		return app._run();
	}
};

template<class T>
struct sgeMain<T, std::enable_if_t< sge::TypeTraits::isBaseOf<sge::NativeUIApp, T>::value> > {
	sgeMain() = delete;

	static int run(T& app) {
		T::CreateDesc desc;
		app.run(desc);
		return app.exitCode();
	}
};

#if SGE_OS_WINDOWS
#define SGE_MAIN(T) \
	int main(int argc, const char* argv[]) { \
		try { \
			T app; \
			app.setCommandArguments(argc, argv); \
			return sgeMain<T>::run(app); \
		} \
		catch (...) { \
			SGE_ASSERT(false); \
			throw; \
		} \
	} \
	//------
#else
	#define SGE_MAIN(T) \
		int main(int argc, const char* argv[]) { \
			T app; \
			app.setCommandArguments(argc, argv); \
			return sgeMain<T>::run(app); \
		} \
	//------
#endif // SGE_OS_WINDOWS