#pragma once

#include "NeHeOGL_Image.h"
#include "NeHeOGL_RenderDataType.h"

namespace sge {

#define NeHeOGL_TextureFilter_ENUM_LIST(E) \
	E(None,) \
	E(Nearest,) \
	E(Linear,) \
//----
SGE_ENUM_DECLARE(NeHeOGL_TextureFilter, int)
SGE_ENUM_ALL_OPERATOR(NeHeOGL_TextureFilter)

#define NeHeOGL_TextureWrap_ENUM_LIST(E) \
	E(None,) \
	E(Repeat,) \
	E(Clamp,) \
//----
SGE_ENUM_CLASS(NeHeOGL_TextureWrap, int)

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
public:
	ColorType colorType = ColorType::RGBAb;
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

	Vec2i			size{ 0,0 };
	SamplerState	samplerState;
	Image			imageToUpload;
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

	ColorType	colorType	() const	{ return _colorType; }
	int			width		() const	{ return _size.x; }
	int			height		() const	{ return _size.y; }
	Vec2i		size		() const	{ return _size; }

private:
	u32			_tex = 0;
	ColorType	_colorType = ColorType::None;
	Vec2i		_size{ 0,0 };
};

}