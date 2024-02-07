#pragma once

#include <sge_core.h>
#include <sge_core/native_ui/NativeUI.h>

#if SGE_OS_WINDOWS
	#include <windowsx.h>

	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <Gl/glaux.h>

	#pragma comment(lib, "Opengl32.lib")
	#pragma comment(lib, "GLu32.lib")
	#pragma comment(lib, "GLaux.lib")

	#undef max
	#undef min
#endif

namespace sge {

template<class T> constexpr void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

inline float g_rad(float degree) {
	return degree / 360.f * (2.f * Math::PI<float>());
}

}