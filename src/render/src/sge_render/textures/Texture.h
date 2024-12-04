#pragma once

#include "Image.h"
#include "sge_render/RenderDataType.h"

namespace sge {

#define TextureFilter_ENUM_LIST(E) \
	E(None,) \
	E(Point,)      /* 1 texels on closest mipmap level */ \
	E(Linear,)     /* 1 texels on 2 mipmap levels */ \
	E(Bilinear,)   /* 4 texels on closest mipmap level */  \
	E(Trilinear,)  /* 4 texels on 2 mipmap levels */\
	E(Anisotropic,) \
//----
SGE_ENUM_CLASS(TextureFilter, u8)

#define TextureWrap_ENUM_LIST(E) \
	E(None,) \
	E(Repeat,) \
	E(Clamp,) \
	E(Mirror,) \
	E(MirrorOnce,) \
//----
SGE_ENUM_CLASS(TextureWrap, u8)


#if 0
#pragma mark ========= SamplerState ============
#endif
struct SamplerState {
public:
	using Filter	= TextureFilter;
	using Wrap		= TextureWrap;

	Filter	filter	= Filter::Bilinear;
	Wrap	wrapU	= Wrap::Repeat;
	Wrap	wrapV	= Wrap::Repeat;
	Wrap	wrapW	= Wrap::Repeat;

	float	minLOD			= 0;
	float	maxLOD			= Math::inf<float>();
	float	maxAnisotrory	= 1.f;
}; // SamplerState


#if 0
#pragma mark ========= Texture_CreateDesc ============
#endif
struct Texture_CreateDesc {
public:
	String		name;
	ColorType	colorType = ColorType::RGBAb;
	int			mipmapCount = 1;
}; // Texture_CreateDesc


#if 0
#pragma mark ========= Texture ============
#endif
class Texture : public RefCountBase {
	using CreateDesc = Texture_CreateDesc;
	using DataType = RenderDataType;

	DataType type() const { return _type; }

protected:
	Texture() = default;
	DataType _type = DataType::None;
}; // Texture


#if 0
#pragma mark ========= Texture2D_UploadRequest ============
#endif
class Texture2D_UploadRequest : public NonCopyable {
public:
	Vec2i		offset{ 0,0 };
	ImageInfo	imageInfo;
	ByteSpan	pixelData;

	void assign(const Image& imageToUpload) {
		pixelData = imageToUpload.pixelData();
		imageInfo = imageToUpload.info();
	}
}; // Texture2D_UploadRequest


#if 0
#pragma mark ========= Texture2D_CreateDesc ============
#endif
struct Texture2D_CreateDesc : public Texture_CreateDesc {
	using UploadRequest = Texture2D_UploadRequest;

	Vec2i			size {0,0};
	SamplerState	samplerState;
	UploadRequest*  uploadRequest = nullptr;
}; // Texture2D_CreateDesc


#if 0
#pragma mark ========= Texture2D ============
#endif
class Texture2D : public Texture {
public:
	using CreateDesc	= Texture2D_CreateDesc;
	using UploadRequest = Texture2D_UploadRequest;

	ColorType		colorType		() const	{ return _colorType; }
	int				width			() const	{ return _size.x; }
	int				height			() const	{ return _size.y; }
	Vec2i			size			() const	{ return _size; }
	int				mipmapCount		() const	{ return _mipmapCount; }
	SamplerState	samplerState	() const	{ return _samplerState; }

protected:
	Texture2D(CreateDesc& desc); // please create from 'Renderer::createTexture2D'

	SamplerState	_samplerState;
	Vec2i			_size {0,0};
	int				_mipmapCount = 0;
	ColorType		_colorType = ColorType::None;
}; // Texture2D

} // namespace sge
