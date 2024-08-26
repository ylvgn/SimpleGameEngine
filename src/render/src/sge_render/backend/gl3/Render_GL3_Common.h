#pragma once

#if SGE_RENDER_HAS_GL3

// GLEW --------
#define GLEW_STATIC 1
#include <GL/glew.h>

#if SGE_OS_WINDOWS
	#include <GL/wglew.h>
#endif
// GLEW --------

#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "GLu32.lib")

#include "../../Render_Common.h"
#include "../../RenderDataType.h"
#include "../../buffer/RenderGpuBuffer.h"
#include <sge_render/shader/Shader.h>

namespace sge {

class Renderer_GL3;

struct GL3Util {
	GL3Util() = delete;

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
	static void getShaderInfoLog(GLuint shader, String& outMsg);
	static void getProgramInfoLog(GLuint program, String& outMsg);

	static GLenum getGlPrimitiveTopology(RenderPrimitiveType v);
	static GLenum getGlFormat(RenderDataType v);
	static GLenum getGlBufferBindingTarget(RenderGpuBufferType v);
	static const char* getGlStageProfile(ShaderStageMask s);
	static GLenum getGlShaderType(ShaderStageMask s);
	static VertexSemantic parseGlSemanticName(StrView vkName);

private:
	static bool _checkError(GLenum errCode) {
		return errCode != GL_NO_ERROR; // if got error, return true
	}
};

inline
GLenum GL3Util::getGlPrimitiveTopology(RenderPrimitiveType v) {
	using SRC = RenderPrimitiveType;
	switch (v) {
		case SRC::Points:		return GL_POINTS;
		case SRC::Lines:		return GL_LINES;
		case SRC::Triangles:	return GL_TRIANGLES;
		default: throw SGE_ERROR("unsupported RenderPrimitiveType");
	}
}

inline
GLenum GL3Util::getGlBufferBindingTarget(RenderGpuBufferType v) {
	using SRC = RenderGpuBufferType;
	switch (v) {
		case SRC::Vertex:	return GL_ARRAY_BUFFER;
		case SRC::Index:	return GL_ELEMENT_ARRAY_BUFFER;
		case SRC::Const:	return GL_UNIFORM_BUFFER;
		default: throw SGE_ERROR("unsupported RenderGpuBufferType");
	}
}

inline
GLenum GL3Util::getGlFormat(RenderDataType v) {
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
const char* GL3Util::getGlStageProfile(ShaderStageMask s) {
	switch (s) {
		case ShaderStageMask::Vertex:	return "330";
		case ShaderStageMask::Pixel:	return "330";
		default: return "";
	}
}

inline
GLenum GL3Util::getGlShaderType(ShaderStageMask s) {
	switch (s) {
		case ShaderStageMask::Vertex:	return GL_VERTEX_SHADER;
		case ShaderStageMask::Pixel:	return GL_FRAGMENT_SHADER;
		default: throw SGE_ERROR("unsupported ShaderStageMask");
	}
}

//GL_VERTEX_SHADER
}

#endif