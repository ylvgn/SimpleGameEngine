#pragma once

#include "../Render_GL_Common.h"

#if SGE_RENDER_HAS_GL3

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
	static GLenum getGlBaseFormat(RenderDataType v);
	static GLenum getGlBufferBindingTarget(RenderGpuBufferType v);
	static const char* getGlStageProfile(ShaderStageMask s);
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
GLenum GL3Util::getGlPrimitiveTopology(RenderPrimitiveType v) {
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
GLenum GL3Util::getGlBufferBindingTarget(RenderGpuBufferType v) {
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
GLenum GL3Util::getGlFormat(RenderDataType v) {
	using SRC = RenderDataType;
	switch (v) {
	//---
		case SRC::Int8:			return GL_BYTE;
		case SRC::Int16:		return GL_SHORT;
	//---
		case SRC::SNorm8:		return GL_R8_SNORM;
		case SRC::SNorm8x2:		return GL_RG8_SNORM;
//		case SRC::SNorm8x3:		return GL_RGB8_SNORM; // does not support in GL3
		case SRC::SNorm8x4:		return GL_RGBA8_SNORM;
	//---
		case SRC::SNorm16:		return GL_R16_SNORM_EXT;
		case SRC::SNorm16x2:	return GL_RG16_SNORM_EXT;
//		case SRC::SNorm16x3:	return GL_RGB16_SNORM_EXT; // does not support in GL3
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
GLenum GL3Util::getGlBaseFormat(RenderDataType v) {
	using SRC = RenderDataType;
	switch (v) {
	//---
		case SRC::Int8:			return GL_BYTE;
		case SRC::Int16:		return GL_SHORT;
	//---
		case SRC::SNorm8:		return GL_R8_SNORM;
		case SRC::SNorm8x2:		return GL_R8_SNORM;
//		case SRC::SNorm8x3:		return GL_R8_SNORM; // does not support in GL3
		case SRC::SNorm8x4:		return GL_R8_SNORM;
	//---
		case SRC::SNorm16:		return GL_R16_SNORM_EXT;
		case SRC::SNorm16x2:	return GL_R16_SNORM_EXT;
//		case SRC::SNorm16x3:	return GL_R16_SNORM_EXT; // does not support in GL3
		case SRC::SNorm16x4:	return GL_R16_SNORM_EXT;
	//---
		case SRC::UNorm8:		return GL_UNSIGNED_BYTE;
		case SRC::UNorm8x2:		return GL_UNSIGNED_BYTE;
//		case SRC::UNorm8x3:		return GL_UNSIGNED_BYTE; // does not support in GL3
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
const char* GL3Util::getGlStageProfile(ShaderStageMask s) {
	switch (s) {
		case ShaderStageMask::Vertex:	return "330";
		case ShaderStageMask::Pixel:	return "330";
	//---
		default: return "";
	}
}

inline
GLenum GL3Util::getGlShaderType(ShaderStageMask s) {
	switch (s) {
		case ShaderStageMask::Vertex:	return GL_VERTEX_SHADER;
		case ShaderStageMask::Pixel:	return GL_FRAGMENT_SHADER;
	//---
		default: throw SGE_ERROR("unsupported ShaderStageMask");
	}
}

} // namespace

#endif // SGE_RENDER_HAS_GL3