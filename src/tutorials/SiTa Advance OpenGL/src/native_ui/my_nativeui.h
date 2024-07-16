#pragma once 

#if SGE_OS_WINDOWS

#include "MyOpenGLNativeUIWindow_Win32.h"

namespace sge {
	using MyOpenGLNativeUIWindow_Impl = MyOpenGLNativeUIWindow_Win32;
}

#else
	#error "unsupported platform"
#endif

namespace sge {

class MyOpenGLNativeUIWindow : public MyOpenGLNativeUIWindow_Impl {
};

}