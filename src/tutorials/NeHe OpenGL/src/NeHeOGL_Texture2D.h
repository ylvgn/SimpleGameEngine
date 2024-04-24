#pragma once

#include "NeHeOGL_GL_Common.h"
#include "NeHeOGL_Image.h"

namespace sge {

#define NeHeOGL_TextureFilter_ENUM_LIST(E) \
	E(Nearest,	= GL_NEAREST) \
	E(Linear,	= GL_LINEAR	) \
//----
SGE_ENUM_DECLARE(NeHeOGL_TextureFilter, GLint)
SGE_ENUM_ALL_OPERATOR(NeHeOGL_TextureFilter)

#define NeHeOGL_TextureWrap_ENUM_LIST(E) \
	E(None,					) \
	E(Repeat,	= GL_REPEAT	) \
	E(Clamp,	= GL_CLAMP	) \
//----
SGE_ENUM_CLASS(NeHeOGL_TextureWrap, GLint)

struct NeHeOGL_SamplerState {
	using Filter	= NeHeOGL_TextureFilter;
	using Wrap		= NeHeOGL_TextureWrap;

	Filter	filter = Filter::Linear;

	Wrap	wrapU	= Wrap::Repeat; // aka wrapS
	Wrap	wrapV	= Wrap::Repeat; // aka wrapT
	Wrap	wrapW	= Wrap::Repeat;

	float	minLOD = 0;
	float	maxLOD = 0;
};

struct NeHeOGL_Texture_CreateDesc {
};

class NeHeOGL_Texture : public NonCopyable {
public:
	using SamplerState		= NeHeOGL_SamplerState;
	using TextureFilter		= NeHeOGL_TextureFilter;
	using RenderDataType	= NeHeOGL_RenderDataType;

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
	using Image			= NeHeOGL_Image;

	~NeHeOGL_Texture2D() { destroy(); }

	void create(CreateDesc& desc);
	void destroy();

	void bind();
	void unbind();
private:
	GLenum _colorType2InternalFormat(ColorType v);

	GLuint _tex = 0;
};

}