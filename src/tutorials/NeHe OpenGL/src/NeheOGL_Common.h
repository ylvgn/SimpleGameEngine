#pragma once

#include "sge_nehe_ogl-config.h"
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

#define NeHe_BeginMode_ENUM_LIST(E) \
	E(None,) \
	E(Points,) \
	E(Lines,) \
	E(LineLoop,) \
	E(LineStrip,) \
	E(Triangles,) \
	E(TriangleStrip,) \
	E(TriangleFan,) \
	E(Quads,) \
	E(QuadStrip,) \
	E(Polygon,) \
//----
SGE_ENUM_CLASS(NeHe_BeginMode, u8)

template<class T> inline void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

inline float g_rad(float degree) {
	return degree / 360.f * (2.f * Math::PI<float>());
}

inline u64 g_to_uint64(LARGE_INTEGER v) {
	return static_cast<u64>(v.HighPart) << 32 | v.LowPart;
}

class MyTimer : public NonCopyable {
public:
	MyTimer() {
		reset();
	}

	void reset() {
		_start = _getTick();
	}

	double get() {
		auto c = _getTick();
		return (c - _start) / 1000.0f;
	}

private:
	u64 _getTick() {
		return ::GetTickCount64(); // 10~16 ms, resolution of GetTickCount64 limit by os
	}
	u64 _start;
};

class MyHiResTimer : public NonCopyable {
public:
	MyHiResTimer() {
		LARGE_INTEGER f;
		bool ok = ::QueryPerformanceFrequency(&f);
		if (!ok) {
			DWORD errCode = GetLastError();
			throw SGE_ERROR("QueryPerformanceFrequency errCode = {}", errCode);
		}
		_freq = g_to_uint64(f);
		reset();
	}

	void reset() {
		_start = _getTick();
	}

	double get() {
		auto c = _getTick();
		return static_cast<double>(c - _start) / _freq;
	}

private:
	u64 _getTick() {
		LARGE_INTEGER v;
		bool ok = ::QueryPerformanceCounter(&v); // <1us
		if (!ok) {
			DWORD errCode = GetLastError();
			throw SGE_ERROR("QueryPerformanceCounter errCode = {}", errCode);
		}
		return g_to_uint64(v);
	}

	u64 _freq;
	u64 _start;
};

}


namespace sge {

struct OGLUtil {
	OGLUtil() = delete;

	static void reportError(GLenum errCode = _getErrorCode());

	static void throwIfError(GLenum errCode = _getErrorCode()) {
		if (_checkError(errCode)) {
			reportError(errCode);
			throw SGE_ERROR("errCode = (0x{:0X})", errCode);
		}
	}

	static bool assertIfError(GLenum errCode = _getErrorCode()) {
		if (_checkError(errCode)) {
			reportError(errCode);
			SGE_ASSERT(false);
			return false;
		}
		return true;
	}

	static GLenum getOGLBeginMode(NeHe_BeginMode v);

private:
	static GLenum _getErrorCode() { return glGetError(); }

	static bool _checkError(GLenum errCode = _getErrorCode()) {
		return errCode != GL_NO_ERROR; // if got error, return true
	}
	
};

inline
void OGLUtil::reportError(GLenum errCode) {
	if (_checkError(errCode)) {
		auto* errStr = gluErrorStringWIN(errCode);
		TempString str;
		UtfUtil::convert(str, errStr);
		SGE_LOG("errCode = (0x{:0X}) {}", static_cast<u32>(errCode), str);
	}
}

inline
GLenum OGLUtil::getOGLBeginMode(NeHe_BeginMode v) {
	using SRC = NeHe_BeginMode;
	switch (v)
	{
		case SRC::Points:			return GL_POINTS;
		case SRC::Lines:			return GL_LINES;
		case SRC::LineLoop:			return GL_LINE_LOOP;
		case SRC::LineStrip:		return GL_LINE_STRIP;
		case SRC::Triangles:		return GL_TRIANGLES;
		case SRC::TriangleStrip:	return GL_TRIANGLE_STRIP;
		case SRC::TriangleFan:		return GL_TRIANGLE_FAN;
		case SRC::Quads:			return GL_QUADS;
		case SRC::QuadStrip:		return GL_QUAD_STRIP;
		case SRC::Polygon:			return GL_POLYGON;
		default:					throw  SGE_ERROR("unsupported OGLBeginMode");
	}
}

} // namespace sge

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

	class ScopedGLBegin {
	public:
		ScopedGLBegin(NeHe_BeginMode mode)	{ glBegin(OGLUtil::getOGLBeginMode(mode)); }
		~ScopedGLBegin() { glEnd(); }
	};

} // namespace OGL
} // namespace sge