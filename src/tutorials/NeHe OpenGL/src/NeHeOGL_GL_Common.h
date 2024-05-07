#pragma once

#include "NeHeOGL_Common.h"

#include <GL/gl.h>
#pragma comment(lib, "Opengl32.lib")

#include <GL/glu.h>
#pragma comment(lib, "GLu32.lib")

//	Note: glaux has been deprecated for a long time
//	#include <Gl/glaux.h>
//	#pragma comment(lib, "GLaux.lib")

#include "NeHeOGL_RenderState.h"
#include "NeHeOGL_Vertex.h"
#include "NeHeOGL_Texture2D.h"
#include "NeHeOGL_Mesh.h"

namespace sge {

#define NeHeOGL_BeginMode_ENUM_LIST(E) \
	E(Points,			= GL_POINTS			) \
	E(Lines,			= GL_LINES			) \
	E(LineLoop,			= GL_LINE_LOOP		) \
	E(LineStrip,		= GL_LINE_STRIP		) \
	E(Triangles,		= GL_TRIANGLES		) \
	E(TriangleStrip,	= GL_TRIANGLE_STRIP	) \
	E(TriangleFan,		= GL_TRIANGLE_FAN	) \
	E(Quads,			= GL_QUADS			) \
	E(QuadStrip,		= GL_QUAD_STRIP		) \
	E(Polygon,			= GL_POLYGON		) \
//----
SGE_ENUM_CLASS(NeHeOGL_BeginMode, GLenum)

using Mesh						= NeHeOGL_Mesh;
using RenderDataType			= NeHeOGL_RenderDataType;
using RenderDataTypeUtil		= NeHeOGL_RenderDataTypeUtil;
using RenderState				= NeHeOGL_RenderState;
using RenderState_Cull			= NeHeOGL_RenderState_Cull;
using RenderState_DepthTestFunc = NeHeOGL_RenderState_DepthTestFunc;
using Texture					= NeHeOGL_Texture;
using Texture2D					= NeHeOGL_Texture2D;
using Image						= NeHeOGL_Image;
using SamplerState				= NeHeOGL_SamplerState;
using TextureFilter				= NeHeOGL_TextureFilter;
using TextureWrap				= NeHeOGL_TextureWrap;
using RenderPrimitiveType		= NeHeOGL_RenderPrimitiveType;

struct OGLUtil {
	OGLUtil() = delete;

	static void reportError(GLenum errCode = _getErrorCode());
	static void throwIfError(GLenum errCode = _getErrorCode());
	static bool assertIfError(GLenum errCode = _getErrorCode());

	static GLenum getGlPrimitiveTopology(RenderPrimitiveType v);
	static constexpr GLenum getGlFormat(RenderDataType v);
	static GLenum getGlColorType(ColorType v);

	static GLenum getGlCullMode(RenderState::Cull v);
	static GLenum getGlDepthTestOp(RenderState::DepthTestOp v);

private:
	static GLenum _getErrorCode() { return glGetError(); }

	static bool _checkError(GLenum errCode = _getErrorCode()) {
		return errCode != GL_NO_ERROR; // if got error, return true
	}
};

inline
void OGLUtil::reportError(GLenum errCode /*= _getErrorCode()*/) {
	// https://learn.microsoft.com/en-us/windows/win32/opengl/gluerrorstring
	
#if 0 && SGE_OS_WINDOWS
	const CHAR* errStr = gluErrorStringWIN(errCode); // why not work???
	TempString str;
	UtfUtil::convert(str, errStr);
#else
	const GLubyte* errStr = gluErrorString(errCode); // in ANSI only
	TempString str(reinterpret_cast<const char*>(errStr));
#endif

	SGE_LOG("glGetError = (0x{:X}) {}", static_cast<u32>(errCode), str);
}

inline
void OGLUtil::throwIfError(GLenum errCode /*= _getErrorCode()*/) {
	if (_checkError(errCode)) {
		reportError(errCode);
		throw SGE_ERROR("glGetError = (0x{:0X})", errCode);
	}
}

inline
bool OGLUtil::assertIfError(GLenum errCode /*= _getErrorCode()*/) {
	if (_checkError(errCode)) {
		reportError(errCode);
		SGE_ASSERT(false);
		return false;
	}
	return true;
}

inline
GLenum OGLUtil::getGlPrimitiveTopology(RenderPrimitiveType v) {
	using SRC = RenderPrimitiveType;
	switch (v) {
		case SRC::Points:		return GL_POINTS;
		case SRC::Lines:		return GL_LINES;
		case SRC::Triangles:	return GL_TRIANGLES;
		default: throw SGE_ERROR("unsupported RenderPrimitiveType");
	}
}

constexpr
GLenum OGLUtil::getGlFormat(RenderDataType v) {
	using SRC = RenderDataType;
	switch (v) {
		case SRC::Int8:			return GL_BYTE;
		case SRC::UInt8:		return GL_UNSIGNED_BYTE;
		case SRC::Int16:		return GL_SHORT;
		case SRC::UInt16:		return GL_UNSIGNED_SHORT;
		case SRC::Int32:		return GL_INT;
		case SRC::UInt32:		return GL_UNSIGNED_INT;
		case SRC::Float32:		return GL_FLOAT;
		default:				throw  SGE_ERROR("unsupported RenderDataType");
	}
}

inline
GLenum OGLUtil::getGlColorType(ColorType v) {
	using SRC = ColorType;

	switch (v) {
		case SRC::Lb:		return GL_RED;	// GL_R8;
		case SRC::Ls:		return GL_RED;	// GL_R16F;
		case SRC::Lf:		return GL_RED;	// GL_R32F;

		case SRC::Rb:		return GL_RED;	// GL_R8;
		case SRC::Rs:		return GL_RED;	// GL_R16F;
		case SRC::Rf:		return GL_RED;	// GL_R32F;

//		case SRC::RGb:		return GL_RG;	// GL_RG;
//		case SRC::RGs:		return GL_RG;	// GL_RG16F;
//		case SRC::RGf:		return GL_RG;	// GL_RG32F;

		case SRC::RGBb:		return GL_RGB;	// GL_RGB;
		case SRC::RGBs:		return GL_RGB;	// GL_RGB16F;
		case SRC::RGBf:		return GL_RGB;	// GL_RGB32F;

		case SRC::RGBAb:	return GL_RGBA; // GL_RGBA8;
		case SRC::RGBAs:	return GL_RGBA; // GL_RGBA16F;
		case SRC::RGBAf:	return GL_RGBA; // GL_RGBA32F;

		default: throw SGE_ERROR("unsupported ColorType");
	}
}

inline
GLenum OGLUtil::getGlCullMode(RenderState::Cull v) {
	using SRC = RenderState::Cull;
	switch (v) {
		case SRC::None:			return GL_FRONT_AND_BACK;
		case SRC::Back:			return GL_BACK;
		case SRC::Front:		return GL_FRONT;
		default:				throw  SGE_ERROR("unsupported RenderState::Cull");
	}
}

inline
GLenum OGLUtil::getGlDepthTestOp(RenderState::DepthTestOp v) {
	using SRC = RenderState::DepthTestOp;
	switch (v) {
		case SRC::Less:			return GL_LESS;
		case SRC::LessEqual:	return GL_LEQUAL;
		case SRC::Equal:		return GL_EQUAL;
		case SRC::Grater:		return GL_GREATER;
		case SRC::GraterEqual:	return GL_GEQUAL;
		case SRC::NotEqual:		return GL_NOTEQUAL;
		case SRC::Always:		return GL_ALWAYS;
		case SRC::Never:		return GL_NEVER;
		default:				throw  SGE_ERROR("unsupported RenderState::DepthTestOp");
	}
}

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

	inline void color4b(const Color4b& c) {
		::glColor4b(
			static_cast<GLbyte>(c.r - 128),
			static_cast<GLbyte>(c.g - 128),
			static_cast<GLbyte>(c.b - 128),
			static_cast<GLbyte>(c.a - 128)
		);
	}
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

#define SGE_DECLEAR_GLXXXPOINTER(GL_FUNC_NAME, FUNC_NAME, IN_SRC) \
	inline void FUNC_NAME(const IN_SRC* p, size_t stride) { \
		using SRC = IN_SRC; \
		using DataType = SRC::ElementType; \
		static constexpr auto type = OGLUtil::getGlFormat(RenderDataTypeUtil::get<DataType>()); \
		GL_FUNC_NAME(SRC::kElementCount, type, static_cast<GLsizei>(stride), p); \
	} \
//-----
	SGE_DECLEAR_GLXXXPOINTER(glVertexPointer,	vertexPointer,		Tuple3f)
	SGE_DECLEAR_GLXXXPOINTER(glVertexPointer,	vertexPointer,		Vec3f)
	SGE_DECLEAR_GLXXXPOINTER(glColorPointer,	colorPointer,		Color4b)
	SGE_DECLEAR_GLXXXPOINTER(glTexCoordPointer, texCoordPointer,	Tuple2f)
	SGE_DECLEAR_GLXXXPOINTER(glTexCoordPointer, texCoordPointer,	Vec2f)
#undef SGE_DECLEAR_GLXXXPOINTER


	class Scoped_glBegin : public NonCopyable {
	public:
		using Mode = NeHeOGL_BeginMode;

		Scoped_glBegin(NeHeOGL_BeginMode mode) { glBegin(static_cast<GLenum>(mode)); }
		~Scoped_glBegin() { glEnd(); }
	};

	class ScopedBindTexture2D : public NonCopyable {
	public:
		ScopedBindTexture2D(GLuint target) { glBindTexture(GL_TEXTURE_2D, target); }
		~ScopedBindTexture2D() { glBindTexture(GL_TEXTURE_2D, 0); }
	};

	class Scoped_glPushMatrix : public NonCopyable {
	public:
		Scoped_glPushMatrix()  { glPushMatrix(); }
		~Scoped_glPushMatrix() { glPopMatrix(); }
	};


	inline void drawGridAndCoordinate(Mesh& grid, Mesh& coordinate) {
		grid.draw();
		coordinate.drawVertexArrays();
	}

} // namespace OGL
} // namespace sge