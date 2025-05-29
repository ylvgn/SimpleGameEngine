#pragma once

#include <sge_render/Render_Common.h>

#if SGE_RENDER_HAS_OPENGL

#include <sge_render/RenderDataType.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/shader/Shader.h>
#include <sge_render/textures/Texture.h>

namespace sge {

struct GLUtil : public RenderCommonBase {
	GLUtil() = delete;

	static void			reportError(GLenum errCode);
	static void			throwIfError();
	static bool			assertIfError();

	static void			compileShader(GLuint& shader, GLenum shaderStageType, StrView filename);
	static void			getShaderInfoLog(GLuint& shader, String& outMsg);
	static void			getProgramInfoLog(GLuint& program, String& outMsg);

	static GLenum		getGlPrimitiveTopology(RenderPrimitiveType v);
	static GLenum		getGlFormat(RenderDataType v);
	static GLenum		getGlBaseFormat(RenderDataType v);
	static GLenum		getGlBufferBindingTarget(RenderGpuBufferType v);
	static GLenum		getGlShaderType(ShaderStageMask s);
	static const char*	getGlStageProfile(ShaderStageMask s);

	static GLenum		getGlCullMode(RenderState_Cull c);
	static GLenum		getGlDepthTestOp(RenderState_DepthTestOp o);
	static GLenum		getGlStencilTestOp(RenderState_StencilTestOp o);
	static GLenum		getGlBlendOp(RenderState_BlendOp o);
	static GLenum		getGlBlendFactor(RenderState_BlendFactor f);

	static GLenum		getGLTextureMinFilter(TextureFilter t, int mipmapCount = 1);
	static GLenum		getGLTextureMagFilter(TextureFilter t);
	static GLenum		getGlTextureWrap(TextureWrap t);
	static GLenum		getGlTextureCubeFaceOrder(TextureCubeFaceOrder t);

	static GLsizei		castGLsizei(int v)	{ return static_cast<GLsizei>(v); }
	static GLuint		castGLuint(int v)	{ return static_cast<GLuint>(v); }
	static GLint		castGLint(int v)	{ return static_cast<GLint>(v); }

	static const char*	getGlSemanticName(VertexSemanticType v);
	static int			getComponentCount(RenderDataType v);

	static void			convert(VertexSemantic& o, StrView i);
	static void			convert(String& o, VertexSemantic i);

	static void			dumpActiveAttrib(GLint program);
	static void			dumpActiveUniforms(GLint program);
	static void			dumpActiveUniformBlocks(GLint program);

private:
	static bool			_checkError(GLenum errCode);
};
SGE_STATIC_ASSERT_NO_MEMBER_CLASS(GLUtil);

SGE_INLINE
bool GLUtil::_checkError(GLenum errCode) {
	return errCode != GL_NO_ERROR; // if got error, return true
}

SGE_INLINE
void GLUtil::reportError(GLenum errCode) {
	auto* sz = reinterpret_cast<const char*>(gluErrorString(errCode));
	TempString str = UtfUtil::toString(sz);
	SGE_LOG("glGetError = (0x{:X}) {}", static_cast<u32>(errCode), str);
}

SGE_INLINE
void GLUtil::throwIfError() {
	auto errCode = glGetError();
	if (_checkError(errCode)) {
		reportError(errCode);
		throw SGE_ERROR("glGetError = (0x{:0X})", errCode);
	}
}

SGE_INLINE
bool GLUtil::assertIfError() {
	auto errCode = glGetError();
	if (_checkError(errCode)) {
		reportError(errCode);
		SGE_ASSERT(false);
		return false;
	}
	return true;
}

SGE_INLINE
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

SGE_INLINE
GLenum GLUtil::getGlBufferBindingTarget(RenderGpuBufferType v) {
	using SRC = RenderGpuBufferType;
	switch (v) {
		case SRC::Vertex:	return GL_ARRAY_BUFFER;
		case SRC::Index:	return GL_ELEMENT_ARRAY_BUFFER;
		case SRC::Const:	return GL_UNIFORM_BUFFER;
		case SRC::Storage:	return GL_SHADER_STORAGE_BUFFER;
	//---
		default: throw SGE_ERROR("unsupported RenderGpuBufferType '{}'", v);
	}
}

SGE_INLINE
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
		default: throw SGE_ERROR("unsupported RenderDataType '{}'", v);
	}
}

SGE_INLINE
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
		default: throw SGE_ERROR("unsupported RenderDataType '{}'", v);
	}
}

SGE_INLINE
GLenum GLUtil::getGlShaderType(ShaderStageMask s) {
	using SRC = ShaderStageMask;
	switch (s) {
		case SRC::Vertex:	return GL_VERTEX_SHADER;
		case SRC::Pixel:	return GL_FRAGMENT_SHADER;
		case SRC::Compute:	return GL_COMPUTE_SHADER;
	//---
		default: throw SGE_ERROR("unsupported ShaderStageMask '{}'", s);
	}
}

SGE_INLINE
const char* GLUtil::getGlStageProfile(ShaderStageMask s) {
	using SRC		= ShaderStageMask;
	using Profile	= ShaderStageProfile;

	static constexpr const char* vs = StringUtil::extractFromPrefix(Profile::GLSL_VS, "vs_");
	static constexpr const char* ps = StringUtil::extractFromPrefix(Profile::GLSL_PS, "ps_");
	static constexpr const char* cs = StringUtil::extractFromPrefix(Profile::GLSL_CS, "cs_");

	switch (s) {
		case SRC::Vertex:	return vs;
		case SRC::Pixel:	return ps;
		case SRC::Compute:	return cs;
	//---
		default: throw SGE_ERROR("unsupported ShaderStageMask '{}'", s);
	}
}

SGE_INLINE
GLenum GLUtil::getGlCullMode(RenderState_Cull c) {
	using SRC = RenderState_Cull;
	switch (c) {
		case SRC::None:	 return GL_FRONT_AND_BACK;
		case SRC::Back:	 return GL_BACK;
		case SRC::Front: return GL_FRONT;
	//---
		default:		 throw  SGE_ERROR("unsupported RenderState_Cull '{}'", c);
	}
}

SGE_INLINE
GLenum GLUtil::getGlDepthTestOp(RenderState_DepthTestOp o) {
	using SRC = RenderState_DepthTestOp;
	switch (o) {
		case SRC::Less:			return GL_LESS;
		case SRC::LessEqual:	return GL_LEQUAL;
		case SRC::Equal:		return GL_EQUAL;
		case SRC::Greater:		return GL_GREATER;
		case SRC::GreaterEqual:	return GL_GEQUAL;
		case SRC::NotEqual:		return GL_NOTEQUAL;
		case SRC::Always:		return GL_ALWAYS;
		case SRC::Never:		return GL_NEVER;
	//---
		default:				throw  SGE_ERROR("unsupported RenderState DepthTestOp '{}'", o);
	}
}

SGE_INLINE
GLenum GLUtil::getGlStencilTestOp(RenderState_StencilTestOp o) {
	using SRC = RenderState_StencilTestOp;
	switch (o) {
		case SRC::Zero:				return GL_ZERO;
		case SRC::Keep:				return GL_KEEP;
		case SRC::Replace:			return GL_REPLACE;
		case SRC::Increment:		return GL_INCR;
		case SRC::IncrementWrap:	return GL_INCR_WRAP;
		case SRC::Decrement:		return GL_DECR;
		case SRC::DecrementWrap:	return GL_DECR_WRAP;
		case SRC::Invert:			return GL_INVERT;
	//---
		default:					throw  SGE_ERROR("unsupported RenderState StencilTestOp '{}'", o);
	}
}

SGE_INLINE
GLenum GLUtil::getGlBlendOp(RenderState_BlendOp o) {
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBlendEquation.xhtml
	// https://learnopengl.com/Advanced-OpenGL/Blending
	using SRC = RenderState_BlendOp;
	switch (o) {
		case SRC::Add:		return GL_FUNC_ADD;
		case SRC::Min:		return GL_MIN;
		case SRC::Max:		return GL_MAX;
		case SRC::Sub:		return GL_FUNC_SUBTRACT;
		case SRC::RevSub:	return GL_FUNC_REVERSE_SUBTRACT;
	//---
		default: throw SGE_ERROR("unsupported RenderState BlendOp '{}'", o);
	}
}

SGE_INLINE
GLenum GLUtil::getGlBlendFactor(RenderState_BlendFactor f) {
	using SRC = RenderState_BlendFactor;
	switch (f) {
		case SRC::Zero:					return GL_ZERO;
		case SRC::One:					return GL_ONE;
		case SRC::SrcAlpha:				return GL_SRC_ALPHA;
		case SRC::DstAlpha:				return GL_DST_ALPHA;
		case SRC::SrcColor:				return GL_SRC_COLOR;
		case SRC::DstColor:				return GL_DST_COLOR;
		case SRC::ConstColor:			return GL_CONSTANT_COLOR;
//		case SRC::ConstAlpha:			return GL_CONSTANT_ALPHA;
		case SRC::OneMinusSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
		case SRC::OneMinusSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
		case SRC::OneMinusDstAlpha:		return GL_ONE_MINUS_DST_ALPHA;
		case SRC::OneMinusDstColor:		return GL_ONE_MINUS_DST_COLOR;
		case SRC::OneMinusConstColor:	return GL_ONE_MINUS_CONSTANT_COLOR;
//		case SRC::OneMinusConstAlpha:	return GL_ONE_MINUS_CONSTANT_ALPHA;
	//---
		default: throw SGE_ERROR("unsupported RenderState BlendFactor '{}'", f);
	}
}

SGE_INLINE
GLenum GLUtil::getGLTextureMinFilter(TextureFilter t, int mipmapCount /*= 1*/) {
	bool m = mipmapCount > 1;

	using SRC = TextureFilter;
	switch (t) {
		case SRC::Point:		return m ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
		case SRC::Linear:		return m ? GL_NEAREST_MIPMAP_LINEAR  : GL_LINEAR;
		case SRC::Bilinear:		return m ? GL_LINEAR_MIPMAP_NEAREST  : GL_LINEAR;
		case SRC::Trilinear:	return m ? GL_LINEAR_MIPMAP_LINEAR   : GL_LINEAR;
#if GL_ARB_texture_filter_anisotropic
		case SRC::Anisotropic:	return GLEW_ARB_texture_filter_anisotropic;
#else
		case SRC::Anisotropic:	return GL_LINEAR;
#endif
	//---
		default: throw SGE_ERROR("unsupported TextureFilter '{}'", t);
	}
}

SGE_INLINE
GLenum GLUtil::getGLTextureMagFilter(TextureFilter t) {
	using SRC = TextureFilter;
	switch (t) {
		case SRC::Point:		return GL_NEAREST;
		case SRC::Linear:		return GL_LINEAR;
		case SRC::Bilinear:		return GL_LINEAR;
		case SRC::Trilinear:	return GL_LINEAR;
#if GL_ARB_texture_filter_anisotropic
		case SRC::Anisotropic:	return GLEW_ARB_texture_filter_anisotropic;
#else
		case SRC::Anisotropic:	return GL_LINEAR;
#endif
	//---
		default: throw SGE_ERROR("unsupported TextureFilter '{}'", t);
	}
}

SGE_INLINE
GLenum GLUtil::getGlTextureWrap(TextureWrap t) {
	using SRC = TextureWrap;
	switch (t) {
		case SRC::Repeat:		return GL_REPEAT;
		case SRC::Clamp:		return GL_CLAMP_TO_EDGE;
		case SRC::Mirror:		return GL_MIRRORED_REPEAT;
		case SRC::MirrorOnce:	return GL_MIRROR_CLAMP_TO_EDGE;
	//---
		default: throw SGE_ERROR("unsupported TextureWrap '{}'", t);
	}
}

SGE_INLINE
GLenum GLUtil::getGlTextureCubeFaceOrder(TextureCubeFaceOrder t) {
	using SRC = TextureCubeFaceOrder;
	switch (t) {
		case SRC::Right:	return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		case SRC::Left:		return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case SRC::Top:		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case SRC::Bottom:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case SRC::Front:	return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case SRC::Back:		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	//---
		default: throw SGE_ERROR("unsupported TextureCubeFaceOrder '{}'", t);
	}
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL