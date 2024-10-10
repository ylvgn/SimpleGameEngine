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
	NativeUIApp();
	~NativeUIApp();

	static NativeUIApp* current();
private:
	static NativeUIApp* _current;
};

} // namespace sge


//----------
template<class T> inline
int sgeWinMain(int argc, const char* argv[]) {
	SGE_STATIC_ASSERT(sge::TypeTraits::isBaseOf<sge::NativeUIApp, T>::value);

	T app;
	T::CreateDesc desc;
	return app.run(desc);
}

#if SGE_OS_WINDOWS
	#define SGE_WinMain(T) \
		int main(int argc, const char* argv[]) { \
			try { \
				return sgeWinMain<T>(argc, argv); \
			} \
			catch (...) { \
				SGE_ASSERT(false); \
				throw; \
			} \
		} \
	//--------
#else
	#define SGE_WinMain(T) \
		int main(int argc, const char* argv[]) { return sgeWinMain<T>(argc, argv); } \
	//--------
#endif // SGE_OS_WINDOWS