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

// RenderDataType ---------------------------------------- start
#define NeHe_RenderDataType_ENUM_LIST(E) \
	E(None,) \
	\
	E(Int8,)	\
	E(Int16,)	\
	E(Int32,)	\
	E(Int64,)	\
	\
	E(UInt8,)	\
	E(UInt16,)	\
	E(UInt32,)	\
	E(UInt64,)	\
	\
	E(Float32,)	\
	E(Float64,)	\

//----
SGE_ENUM_CLASS(NeHe_RenderDataType, u8)

struct NeHe_RenderDataTypeUtil {
	NeHe_RenderDataTypeUtil() = delete;
	
	using Type = NeHe_RenderDataType;

	template<class T> static constexpr Type get();

	template<> static constexpr Type get<void>() { return Type::None;  }

	template<> static constexpr Type get<i8 >() { return Type::Int8;  }
	template<> static constexpr Type get<i16>() { return Type::Int16; }
	template<> static constexpr Type get<i32>() { return Type::Int32; }
	template<> static constexpr Type get<i64>() { return Type::Int64; }

	template<> static constexpr Type get<u8 >() { return Type::UInt8;  }
	template<> static constexpr Type get<u16>() { return Type::UInt16; }
	template<> static constexpr Type get<u32>() { return Type::UInt32; }
	template<> static constexpr Type get<u64>() { return Type::UInt64; }

	template<> static constexpr Type get<f32>() { return Type::Float32; }
	template<> static constexpr Type get<f64>() { return Type::Float64; }
};
// RenderDataType ---------------------------------------- end

template<class T> inline void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

inline float g_rad(float degree) {
	return degree / 360.f * (2.f * Math::PI<float>());
}

inline u64 g_to_uint64(LARGE_INTEGER v) {
	return static_cast<u64>(v.HighPart) << 32 | v.LowPart;
}


// Timer ---------------------------------------- start
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
		return ::GetTickCount64(); // 10~16 ms, resolution of GetTickCount64 is limited by OS
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
		bool ok = ::QueryPerformanceCounter(&v); // <1 us
		if (!ok) {
			DWORD errCode = GetLastError();
			throw SGE_ERROR("QueryPerformanceCounter errCode = {}", errCode);
		}
		return g_to_uint64(v);
	}

	u64 _freq;
	u64 _start;
};
// Timer ---------------------------------------- end


// OGLUtil ---------------------------------------- start
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

	static constexpr GLenum getGlFormat(NeHe_RenderDataType v);

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

constexpr
GLenum OGLUtil::getGlFormat(NeHe_RenderDataType v) {
	using SRC = NeHe_RenderDataType;
	switch (v) {
		case SRC::Int8:			return GL_BYTE;
		case SRC::UInt8:		return GL_UNSIGNED_BYTE;
		case SRC::Int16:		return GL_SHORT;
		case SRC::UInt16:		return GL_UNSIGNED_SHORT;
		case SRC::Int32:		return GL_INT;
		case SRC::UInt32:		return GL_UNSIGNED_INT;
		case SRC::Float32:		return GL_FLOAT;
		default:				throw SGE_ERROR("unsupported NeHe_RenderDataType");
	}
}
// OGLUtil ---------------------------------------- end


// MyImage ---------------------------------------- start
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
// MyImage ---------------------------------------- end


// MyVertex ---------------------------------------- start
class MyVertex_PosColor {
public:
	Vec3f pos;
	Color4b color;
};

// MyVertex ---------------------------------------- end


// MyMesh ---------------------------------------- start
class MyMesh {
public:
	Vector<MyVertex_PosColor> vertices;
	Vector<u16> indices;
};
// MyMesh ---------------------------------------- end

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

	static constexpr Color4b kbWhite	{ 255,255,255,255 };
	static constexpr Color4b kbBlack	{ 0,  0,  0,  255 };
	static constexpr Color4b kbRed		{ 255,0,  0,  255 };
	static constexpr Color4b kbGreen	{ 0,  255,0,  255 };
	static constexpr Color4b kbBlue		{ 0,  0,  255,255 };
	static constexpr Color4b kbYellow	{ 255,255,0,  255 };
	static constexpr Color4b kbViolet	{ 255,0,  255,255 };
	static constexpr Color4b kbCyan		{ 0,  255,255,255 };
	static constexpr Color4b kbOrange	{ 255,128,0,  255 };

	inline void color4f(const Color4f& c)					{ ::glColor4f(c.r, c.g, c.b, c.a); }
	inline void texCoord2f(const Tuple2f& uv)				{ ::glTexCoord2f(uv.x, uv.y); }
	inline void vertex3f(const Tuple3f& v)					{ ::glVertex3f(v.x, v.y, v.z); }
	inline void normal3f(const Tuple3f& nl)					{ ::glNormal3f(nl.x, nl.y, nl.z); }
	inline void translatef(const Tuple3f& t)				{ ::glTranslatef(t.x, t.y, t.z); }
	inline void rotatef(float degrees, const Tuple3f& axis)	{ ::glRotatef(degrees, axis.x, axis.y, axis.z); }
	inline void scalef(const Tuple3f& s)					{ ::glScalef(s.x, s.y, s.z); }


	inline void color4fv(const Color4f* c)		{ ::glColor4fv(c->data); }
	inline void texCoord2fv(const Tuple2f& uv)	{ ::glTexCoord2fv(uv.data); }
	inline void vertex3fv(const Tuple3f* v)		{ ::glVertex3fv(v->data); }
	inline void normal3fv(const Tuple3f* nl)	{ ::glNormal3fv(nl->data); }

	class ScopedBegin : public NonCopyable {
	public:
		ScopedBegin(NeHe_BeginMode mode) { glBegin(static_cast<GLenum>(mode)); }
		~ScopedBegin() { glEnd(); }
	};

} // namespace OGL
} // namespace sge