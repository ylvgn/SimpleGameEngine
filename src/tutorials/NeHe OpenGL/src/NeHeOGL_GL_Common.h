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

struct OGLUtil {
	OGLUtil() = delete;

	static void reportError(GLenum errCode = _getErrorCode()) {
		auto* errStr = gluErrorStringWIN(errCode);
		TempString str;
		UtfUtil::convert(str, errStr);
		SGE_LOG("glGetError = (0x{:X}) {}", static_cast<u32>(errCode), str);
	}

	static void throwIfError(GLenum errCode = _getErrorCode()) {
		if (_checkError(errCode)) {
			reportError(errCode);
			throw SGE_ERROR("glGetError = (0x{:0X})", errCode);
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
	static GLenum getGlCullMode(NeHeOGL_RenderState::Cull v);
	static GLenum getGlDepthTestOp(NeHeOGL_RenderState::DepthTestOp v);

private:
	static GLenum _getErrorCode() { return glGetError(); }

	static bool _checkError(GLenum errCode = _getErrorCode()) {
		return errCode != GL_NO_ERROR; // if got error, return true
	}
};

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
		default:				throw  SGE_ERROR("unsupported NeHe_RenderDataType");
	}
}

inline
GLenum OGLUtil::getGlCullMode(NeHeOGL_RenderState::Cull v) {
	using SRC = NeHeOGL_RenderState::Cull;
	switch (v) {
		case SRC::None:			return GL_FRONT_AND_BACK;
		case SRC::Back:			return GL_BACK;
		case SRC::Front:		return GL_FRONT;
		default:				throw  SGE_ERROR("unsupported NeHeOGL_RenderState::Cull");
	}
}

inline
GLenum OGLUtil::getGlDepthTestOp(NeHeOGL_RenderState::DepthTestOp v) {
	using SRC = NeHeOGL_RenderState::DepthTestOp;
	switch (v) {
		case SRC::Less:			return GL_LESS;
		case SRC::LessEqual:	return GL_LEQUAL;
		case SRC::Equal:		return GL_EQUAL;
		case SRC::Grater:		return GL_GREATER;
		case SRC::GraterEqual:	return GL_GEQUAL;
		case SRC::NotEqual:		return GL_NOTEQUAL;
		case SRC::Always:		return GL_ALWAYS;
		case SRC::Never:		return GL_NEVER;
		default:				throw  SGE_ERROR("unsupported NeHeOGL_RenderState::DepthTestOp");
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
		static constexpr auto type = OGLUtil::getGlFormat(NeHe_RenderDataTypeUtil::get<DataType>()); \
		GL_FUNC_NAME(SRC::kElementCount, type, static_cast<GLsizei>(stride), p); \
	} \
//-----
	SGE_DECLEAR_GLXXXPOINTER(glVertexPointer,	vertexPointer,		Tuple3f)
	SGE_DECLEAR_GLXXXPOINTER(glVertexPointer,	vertexPointer,		Vec3f)
	SGE_DECLEAR_GLXXXPOINTER(glColorPointer,	colorPointer,		Color4b)
	SGE_DECLEAR_GLXXXPOINTER(glTexCoordPointer, texCoordPointer,	Tuple2f)
	SGE_DECLEAR_GLXXXPOINTER(glTexCoordPointer, texCoordPointer,	Vec2f)
#undef SGE_DECLEAR_GLXXXPOINTER


	class ScopedBegin : public NonCopyable {
	public:
		ScopedBegin(NeHe_BeginMode mode) { glBegin(static_cast<GLenum>(mode)); }
		~ScopedBegin() { glEnd(); }
	};

	class ScopedBindTexture2D : public NonCopyable {
	public:
		ScopedBindTexture2D(GLuint target) { glBindTexture(GL_TEXTURE_2D, target); }
		~ScopedBindTexture2D() { glBindTexture(GL_TEXTURE_2D, 0); }
	};

	class ScopedglPushMatrix : public NonCopyable {
	public:
		ScopedglPushMatrix()  { glPushMatrix(); }
		~ScopedglPushMatrix() { glPopMatrix(); }
	};

} // namespace OGL
} // namespace sge