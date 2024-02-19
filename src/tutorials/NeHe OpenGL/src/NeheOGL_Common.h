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

template<class T> inline void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

inline float g_rad(float degree) {
	return degree / 360.f * (2.f * Math::PI<float>());
}

}

namespace sge {
namespace OGL {

	static constexpr Color4f kWhite			{ 1.0f, 1.0f, 1.0f, 1.0f };
	static constexpr Color4f kBlack			{ 0.0f, 0.0f, 0.0f, 1.0f };
	static constexpr Color4f kRed			{ 1.0f, 0.0f, 0.0f, 1.0f };
	static constexpr Color4f kGreen			{ 0.0f, 1.0f, 0.0f, 1.0f };
	static constexpr Color4f kBlue			{ 0.0f, 0.0f, 1.0f, 1.0f };
	static constexpr Color4f kYellow		{ 1.0f, 1.0f, 0.0f, 1.0f };
	static constexpr Color4f kViolet		{ 1.0f, 0.0f, 1.0f, 1.0f };
	static constexpr Color4f kCyan			{ 0.0f, 1.0f, 1.0f, 1.0f };
	static constexpr Color4f kOrange		{ 1.0f, 0.5f, 0.0f, 1.0f };

	inline void glColor(const Color4f& c)	{ glColor4f(c.r, c.g, c.b, c.a); }

} // namespace OGL
} // namespace sge