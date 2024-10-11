#pragma once

#if SGE_RENDER_HAS_OPENGL

// GLEW --------
#define GLEW_STATIC 1
#include <GL/glew.h>

#if SGE_OS_WINDOWS
	#include <GL/wglew.h>
#endif
// GLEW --------

#include <GL/gl.h>
#include <GL/glu.h>

#if SGE_OS_WINDOWS
	#pragma comment(lib, "Opengl32.lib")
	#pragma comment(lib, "GLu32.lib")
#endif

#include <sge_render/Render_Common.h>
#include <sge_render/RenderDataType.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/shader/Shader.h>
#include <sge_render/textures/Texture.h>

namespace sge {

class Renderer_GL;

#if 0
#pragma mark ========= GLObject ============
#endif
class GLObject : public NonCopyable {
public:
	virtual ~GLObject() noexcept { SGE_ASSERT(_gl == 0); }

	bool isValid() { return _gl != 0; }

protected:
	GLuint _gl = 0;
};

#if 0
#pragma mark ========= GLVertexBuffer ============
#endif
class GLVertexBuffer : public GLObject {
public:
	~GLVertexBuffer() { destroy(); }

	template<class T> void create(Span<T> data);
	void destroy();

	void bind	() const { glBindBuffer(GL_ARRAY_BUFFER, _gl); }
	void unbind	() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

#if 0
#pragma mark ========= GLIndexBuffer ============
#endif
class GLIndexBuffer : public GLObject {
public:
	~GLIndexBuffer() { destroy(); }

	void create(const Span<const u16> data);
	void create(const Span<const u32> data);

	void destroy();

	void bind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gl); }
	void unbind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
};

#if 0
#pragma mark ========= GLUtil ============
#endif
struct GLUtil {
	GLUtil() = delete;

	static void reportError(GLenum errCode) {
		auto* sz = reinterpret_cast<const char*>(gluErrorString(errCode));
		TempString str = UtfUtil::toString(sz);
		SGE_LOG("glGetError = (0x{:X}) {}", static_cast<u32>(errCode), str);
	}

	static void throwIfError() {
		auto errCode = glGetError();
		if (_checkError(errCode)) {
			reportError(errCode);
			throw SGE_ERROR("glGetError = (0x{:0X})", errCode);
		}
	}

	static bool assertIfError() {
		auto errCode = glGetError();
		if (_checkError(errCode)) {
			reportError(errCode);
			SGE_ASSERT(false);
			return false;
		}
		return true;
	}

	static GLenum getErrorCode() { return glGetError(); }

	static void compileShader(GLuint& shader, GLenum type, StrView filename);
	static void compileShader(GLuint& shader, GLenum type, ByteSpan sourceCode, StrView filename = StrView());
	static GLuint compileShader(GLenum type, StrView source);
	static void getProgramInfoLog(GLuint program, String& outMsg);

	static GLenum getGlPrimitiveTopology(RenderPrimitiveType v);
	static GLenum getGlFormat(RenderDataType v);
	static GLenum getGlBaseFormat(RenderDataType v);
	static GLenum getGlBufferBindingTarget(RenderGpuBufferType v);
	static GLenum getGlShaderType(ShaderStageMask s);

	static const char* getGlSemanticName(VertexSemanticType v);
	static int getComponentCount(RenderDataType v);

	static void convert(VertexSemantic& o, StrView i);
	static void convert(String& o, VertexSemantic i);

	static void dumpActiveAttrib(GLint program);
	static void dumpActiveUniforms(GLint program);
	static void dumpActiveUniformBlocks(GLint program);

private:
	static bool _checkError(GLenum errCode) {
		return errCode != GL_NO_ERROR; // if got error, return true
	}
};

inline
GLenum GLUtil::getGlPrimitiveTopology(RenderPrimitiveType v) {
	using SRC = RenderPrimitiveType;
	switch (v) {
		case SRC::Points:		return GL_POINTS;
		case SRC::Lines:		return GL_LINES;
		case SRC::Triangles:	return GL_TRIANGLES;
	//---
		default: throw SGE_ERROR("unsupported RenderPrimitiveType");
	}
}

inline
GLenum GLUtil::getGlBufferBindingTarget(RenderGpuBufferType v) {
	using SRC = RenderGpuBufferType;
	switch (v) {
		case SRC::Vertex:	return GL_ARRAY_BUFFER;
		case SRC::Index:	return GL_ELEMENT_ARRAY_BUFFER;
		case SRC::Const:	return GL_UNIFORM_BUFFER;
	//---
		default: throw SGE_ERROR("unsupported RenderGpuBufferType");
	}
}

inline
GLenum GLUtil::getGlFormat(RenderDataType v) {
	using SRC = RenderDataType;
	switch (v) {
	//---
		case SRC::Int8:			return GL_BYTE;
		case SRC::Int16:		return GL_SHORT;
	//---
		case SRC::SNorm8:		return GL_R8_SNORM;
		case SRC::SNorm8x2:		return GL_RG8_SNORM;
//		case SRC::SNorm8x3:		return GL_RGB8_SNORM; // does not support in GL
		case SRC::SNorm8x4:		return GL_RGBA8_SNORM;
	//---
		case SRC::SNorm16:		return GL_R16_SNORM_EXT;
		case SRC::SNorm16x2:	return GL_RG16_SNORM_EXT;
//		case SRC::SNorm16x3:	return GL_RGB16_SNORM_EXT; // does not support in GL
		case SRC::SNorm16x4:	return GL_RGBA16_SNORM_EXT;
	//---
		case SRC::Int32:		return GL_INT;
		case SRC::Int32x2:		return GL_INT_VEC2;
		case SRC::Int32x3:		return GL_INT_VEC3;
		case SRC::Int32x4:		return GL_INT_VEC4; 
	//---
		case SRC::UInt8:		return GL_UNSIGNED_BYTE;
		case SRC::UInt16:		return GL_UNSIGNED_SHORT;
	//---
		case SRC::UInt32:		return GL_UNSIGNED_INT;
		case SRC::UInt32x2:		return GL_UNSIGNED_INT_VEC2;
		case SRC::UInt32x3:		return GL_UNSIGNED_INT_VEC3;
		case SRC::UInt32x4:		return GL_UNSIGNED_INT_VEC4;
	//---
		case SRC::Float32:		return GL_FLOAT;
		case SRC::Float32x2:	return GL_FLOAT_VEC2;
		case SRC::Float32x3:	return GL_FLOAT_VEC3;
		case SRC::Float32x4:	return GL_FLOAT_VEC4;
	//---
		case SRC::Float64:		return GL_DOUBLE;
	//---
		case SRC::Float32_4x4:	return GL_FLOAT_MAT4;
	//---
		default: throw SGE_ERROR("unsupported RenderDataType");
	}
}

inline
GLenum GLUtil::getGlBaseFormat(RenderDataType v) {
	using SRC = RenderDataType;
	switch (v) {
	//---
		case SRC::Int8:			return GL_BYTE;
		case SRC::Int16:		return GL_SHORT;
	//---
		case SRC::SNorm8:		return GL_R8_SNORM;
		case SRC::SNorm8x2:		return GL_R8_SNORM;
//		case SRC::SNorm8x3:		return GL_R8_SNORM; // does not support in GL
		case SRC::SNorm8x4:		return GL_R8_SNORM;
	//---
		case SRC::SNorm16:		return GL_R16_SNORM_EXT;
		case SRC::SNorm16x2:	return GL_R16_SNORM_EXT;
//		case SRC::SNorm16x3:	return GL_R16_SNORM_EXT; // does not support in GL
		case SRC::SNorm16x4:	return GL_R16_SNORM_EXT;
	//---
		case SRC::UNorm8:		return GL_UNSIGNED_BYTE;
		case SRC::UNorm8x2:		return GL_UNSIGNED_BYTE;
//		case SRC::UNorm8x3:		return GL_UNSIGNED_BYTE; // does not support in GL
		case SRC::UNorm8x4:		return GL_UNSIGNED_BYTE;
	//---
		case SRC::Int32:		return GL_INT;
		case SRC::Int32x2:		return GL_INT;
		case SRC::Int32x3:		return GL_INT;
		case SRC::Int32x4:		return GL_INT;
	//---
		case SRC::UInt8:		return GL_UNSIGNED_BYTE;
		case SRC::UInt16:		return GL_UNSIGNED_SHORT;
		case SRC::UInt32:		return GL_UNSIGNED_INT;
	//---
		case SRC::Float32:		return GL_FLOAT;
		case SRC::Float32x2:	return GL_FLOAT;
		case SRC::Float32x3:	return GL_FLOAT;
		case SRC::Float32x4:	return GL_FLOAT;
	//---
		case SRC::Float32_4x4:	return GL_FLOAT;
	//---
		default: throw SGE_ERROR("unsupported RenderDataType");
	}
}

inline
GLenum GLUtil::getGlShaderType(ShaderStageMask s) {
	switch (s) {
		case ShaderStageMask::Vertex:	return GL_VERTEX_SHADER;
		case ShaderStageMask::Pixel:	return GL_FRAGMENT_SHADER;
	//---
		default: throw SGE_ERROR("unsupported ShaderStageMask");
	}
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL