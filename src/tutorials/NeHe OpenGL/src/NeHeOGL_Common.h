#pragma once

#include "sge_nehe_ogl-config.h"
#include <sge_core.h>
#include <sge_core/native_ui/NativeUI.h>

#if SGE_OS_WINDOWS
	#include <windowsx.h>

	#include <GL/gl.h>
	#pragma comment(lib, "Opengl32.lib")

	#include <GL/glu.h>
	#pragma comment(lib, "GLu32.lib")

//	Note: glaux has been deprecated for a long time
//	#include <Gl/glaux.h>
//	#pragma comment(lib, "GLaux.lib")

	#undef max
	#undef min
#endif

namespace sge {

#define NeHe_BeginMode_ENUM_LIST(E) \
	E(Points,			= GL_POINTS) \
	E(Lines,			= GL_LINES) \
	E(LineLoop,			= GL_LINE_LOOP) \
	E(LineStrip,		= GL_LINE_STRIP) \
	E(Triangles,		= GL_TRIANGLES) \
	E(TriangleStrip,	= GL_TRIANGLE_STRIP) \
	E(TriangleFan,		= GL_TRIANGLE_FAN) \
	E(Quads,			= GL_QUADS) \
	E(QuadStrip,		= GL_QUAD_STRIP) \
	E(Polygon,			= GL_POLYGON) \
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
		SGE_LOG("errCode = (0x{:X}) {}", static_cast<u32>(errCode), str);
	}
}

struct MyImage {
	int width = 0;
	int height = 0;

	Vector<u8> pixelData;

	void clean() {
		width = 0;
		height = 0;
		pixelData.clear();
	}

	void loadFile(StrView filename) {
		auto ext = FilePath::extension(filename);
		if (0 == StringUtil::ignoreCaseCompare(ext, "bmp")) {
			return loadBmpFile(filename);
		}

		throw SGE_ERROR("unsupported image file format {}", ext);
	}

	void loadBmpFile(StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		loadBmpMem(mm);
	}

	void loadBmpMem(ByteSpan data);
};

} // namespace sge

namespace sge {
namespace OGL {

	static constexpr Color4f kWhite		{ 1.0f, 1.0f, 1.0f, 1.0f };
	static constexpr Color4f kBlack		{ 0.0f, 0.0f, 0.0f, 1.0f };
	static constexpr Color4f kRed		{ 1.0f, 0.0f, 0.0f, 1.0f };
	static constexpr Color4f kGreen		{ 0.0f, 1.0f, 0.0f, 1.0f };
	static constexpr Color4f kBlue		{ 0.0f, 0.0f, 1.0f, 1.0f };
	static constexpr Color4f kYellow	{ 1.0f, 1.0f, 0.0f, 1.0f };
	static constexpr Color4f kViolet	{ 1.0f, 0.0f, 1.0f, 1.0f };
	static constexpr Color4f kCyan		{ 0.0f, 1.0f, 1.0f, 1.0f };
	static constexpr Color4f kOrange	{ 1.0f, 0.5f, 0.0f, 1.0f };

	inline void color4f(const Color4f& c)					{ ::glColor4f(c.r, c.g, c.b, c.a); }
	inline void texCoord2f(const Tuple2f& uv)				{ ::glTexCoord2f(uv.x, uv.y); }
	inline void vertex3f(const Tuple3f& v)					{ ::glVertex3f(v.x, v.y, v.z); }
	inline void normal3f(const Tuple3f& nl)					{ ::glNormal3f(nl.x, nl.y, nl.z); }
	inline void translatef(const Tuple3f& t)				{ ::glTranslatef(t.x, t.y, t.z); }
	inline void rotatef(float degrees, const Tuple3f& axis)	{ ::glRotatef(degrees, axis.x, axis.y, axis.z); }
	inline void scalef(const Tuple3f& s)					{ ::glScalef(s.x, s.y, s.z); }

	class ScopedBegin {
	public:
		ScopedBegin(NeHe_BeginMode mode) { glBegin(static_cast<GLenum>(mode)); }
		~ScopedBegin() { glEnd(); }
	};

} // namespace OGL
} // namespace sge