#pragma once

#include "NeHeOGL_GL_Common.h"
#include "NeHeOGL_Image.h"

namespace sge {

#define NeHe_TextureMagFilter_ENUM_LIST(E) \
	E(None,					) \
	E(Nearest,	= GL_NEAREST) \
	E(Linear,	= GL_LINEAR	) \
//----
SGE_ENUM_CLASS(NeHe_TextureMagFilter, GLint)

#define NeHe_TextureMinFilter_ENUM_LIST(E) \
	E(None,					) \
	E(Nearest,	= GL_NEAREST) \
	E(Linear,	= GL_LINEAR	) \
	\
	E(NearestMipNearest, = GL_NEAREST_MIPMAP_NEAREST) \
	E(LinearNearestMipNearest, = GL_LINEAR_MIPMAP_NEAREST) \
	E(NearestMipLinear, = GL_NEAREST_MIPMAP_LINEAR) \
	E(LinearNearestMipLinear, = GL_LINEAR_MIPMAP_LINEAR) \
//----
SGE_ENUM_CLASS(NeHe_TextureMinFilter, GLint)

enum class NeHe_TextureFilter : GLint;
constexpr NeHe_TextureFilter NeHe_TextureFilter_make(NeHe_TextureMagFilter v) {
	return static_cast<NeHe_TextureFilter>(enumInt(v));
}
constexpr NeHe_TextureFilter NeHe_TextureFilter_make(NeHe_TextureMinFilter v) {
	return static_cast<NeHe_TextureFilter>(enumInt(v));
}
#define NeHe_TextureFilter_ENUM_LIST(E) \
	/* TextureMagFilter */ \
	E(Nearest, = NeHe_TextureFilter_make(NeHe_TextureMagFilter::Nearest)) \
	E(Linear, = NeHe_TextureFilter_make(NeHe_TextureMagFilter::Linear)) \
	/* TextureMinFilter */ \
	E(NearestMipNearest, = NeHe_TextureFilter_make(NeHe_TextureMinFilter::NearestMipNearest)) \
	E(LinearNearestMipNearest, = NeHe_TextureFilter_make(NeHe_TextureMinFilter::LinearNearestMipNearest)) \
	E(NearestMipLinear, = NeHe_TextureFilter_make(NeHe_TextureMinFilter::NearestMipLinear)) \
	E(LinearNearestMipLinear, = NeHe_TextureFilter_make(NeHe_TextureMinFilter::LinearNearestMipLinear)) \
//----
SGE_ENUM_DECLARE(NeHe_TextureFilter, GLint)
SGE_ENUM_ALL_OPERATOR(NeHe_TextureFilter)

#define NeHe_TextureWrap_ENUM_LIST(E) \
	E(None,					) \
	E(Repeat,	= GL_REPEAT	) \
	E(Clamp,	= GL_CLAMP	) \
//----
SGE_ENUM_CLASS(NeHe_TextureWrap, GLint)

struct NeHeOGL_SamplerState {
	using Filter	= NeHe_TextureFilter;
	using Wrap		= NeHe_TextureWrap;

	Filter	minFilter = Filter::Linear;
	Filter	magFilter = Filter::Linear;

	Wrap	wrapU	= Wrap::Repeat; // aka wrapS
	Wrap	wrapV	= Wrap::Repeat; // aka wrapT

	float	minLOD = 0;
	float	maxLOD = 0;
};

struct NeHeOGL_Texture_CreateDesc {
};

class NeHeOGL_Texture : public NonCopyable {
public:
	using SamplerState		= NeHeOGL_SamplerState;
	using TextureFilter		= SamplerState::Filter;
	using RenderDataType	= NeHe_RenderDataType;

	RenderDataType type() const { return _type; }

protected:
	RenderDataType _type = RenderDataType::None;
};

struct NeHeOGL_Texture2D_CreateDesc : public NeHeOGL_Texture_CreateDesc {
public:
	using Image			= NeHeOGL_Image;
	using SamplerState	= NeHeOGL_SamplerState;

	SamplerState		samplerState;
	Image				imageToUpload;
};

class NeHeOGL_Texture2D : public NeHeOGL_Texture {
public:
	using CreateDesc	= NeHeOGL_Texture2D_CreateDesc;
	using Image			= CreateDesc::Image;

	~NeHeOGL_Texture2D() { destroy(); }

	void create(CreateDesc& desc);
	void destroy();

	void createTest();
	void createByLoadFile(StrView filename);

	void bind();
	void unbind();
private:
	GLenum _colorType2InternalFormat(ColorType v);

	GLuint _tex = 0;
};

}