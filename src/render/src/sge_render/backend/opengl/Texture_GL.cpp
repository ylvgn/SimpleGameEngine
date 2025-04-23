#include "Texture_GL.h"
#include "Renderer_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {

#if 0
#pragma mark ========= Texture2D_GL__Format ============
#endif
void Texture2D_GL__Format::set(ColorType colorType) {
	switch (colorType) {
		case ColorType::RGBAb:		{ internalFormat = GL_RGBA8;	sourceFormat = GL_RGBA;		elementType = GL_UNSIGNED_BYTE;	} break;
		case ColorType::RGBb:		{ internalFormat = GL_RGB8;		sourceFormat = GL_RGB;		elementType = GL_UNSIGNED_BYTE;	} break;
		case ColorType::RGb:		{ internalFormat = GL_RG8;		sourceFormat = GL_RG;		elementType = GL_UNSIGNED_BYTE;	} break;
		case ColorType::LAb:		{ internalFormat = GL_RG8;		sourceFormat = GL_RG;		elementType = GL_UNSIGNED_BYTE;	} break;
		case ColorType::Rb:			{ internalFormat = GL_R8;		sourceFormat = GL_RED;		elementType = GL_UNSIGNED_BYTE;	} break;
		case ColorType::Lb:			{ internalFormat = GL_R8;		sourceFormat = GL_RED;		elementType = GL_UNSIGNED_BYTE;	} break;
	//---
		case ColorType::RGBAs:		{ internalFormat = GL_RGBA16;	sourceFormat = GL_RGBA;		elementType = GL_UNSIGNED_SHORT; } break;
		case ColorType::RGBs:		{ internalFormat = GL_RGB16;	sourceFormat = GL_RGB;		elementType = GL_UNSIGNED_SHORT; } break;
		case ColorType::RGs:		{ internalFormat = GL_RG16;		sourceFormat = GL_RG;		elementType = GL_UNSIGNED_SHORT; } break;
		case ColorType::LAs:		{ internalFormat = GL_RG16;		sourceFormat = GL_RG;		elementType = GL_UNSIGNED_SHORT; } break;
		case ColorType::Rs:			{ internalFormat = GL_R16;		sourceFormat = GL_RED;		elementType = GL_UNSIGNED_SHORT; } break;
		case ColorType::Ls:			{ internalFormat = GL_R16;		sourceFormat = GL_RED;		elementType = GL_UNSIGNED_SHORT; } break;
	//---
		case ColorType::BC1:		{ internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;	sourceFormat = 0; elementType = 0;	} break;
		case ColorType::BC2:		{ internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;	sourceFormat = 0; elementType = 0;	} break;
		case ColorType::BC3:		{ internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;	sourceFormat = 0; elementType = 0;	} break;
		case ColorType::BC7:		{ internalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM;		sourceFormat = 0; elementType = 0;	} break;
	//---
		default: throw SGE_ERROR("unsupported ColorType '{}'", colorType);
	}
}


#if 0
#pragma mark ========= Texture2D_GL ============
#endif
Texture2D_GL::~Texture2D_GL() {
	if (_tex) {
		glDeleteTextures(1, &_tex);
		_tex = 0;
	}
}

Texture2D_GL::Texture2D_GL(CreateDesc& desc)
	: Base(desc)
{
	glGenTextures(1, &_tex);
	Util::throwIfError();

	_format.set(desc.colorType);

	bind();

	GLsizei w = desc.size.x;
	GLsizei h = desc.size.y;

	// TODO: upload texture with glCompressedTexSubImage2D/glTexSubImage2D
	GLint unpackAlignment = 4;

	const void* pixelDataPtr = nullptr;
	if (desc.uploadRequest) {
		const auto& info = desc.uploadRequest->imageInfo;
		const auto& pixelData = desc.uploadRequest->pixelData;

		if (info.colorType == ColorType::None) {
			throw SGE_ERROR("");
		}

		auto s = Math::clamp(info.size, Vec2i::s_zero(), size() - desc.uploadRequest->offset);
		if (s != info.size) {
			throw SGE_ERROR("out of texture area");
		}

		SGE_ASSERT(desc.uploadRequest->offset == Vec2i::s_zero()); // TODO: upload texture with glCompressedTexSubImage2D/glTexSubImage2D

		if (!ColorUtil::isCompressedType(desc.colorType)) { // TODO BlockCompress texture is not allow check ???
			int pixelSizeInBytes = info.pixelSizeInBytes();

			size_t expectedDataSize = info.strideInBytes * info.height();
			if (pixelData.size() < expectedDataSize) {
				throw SGE_ERROR("out of texture area");
			}

			if (pixelSizeInBytes % 4) unpackAlignment = 1;
		}

		pixelDataPtr = pixelData.data();
	}

	if (ColorUtil::isCompressedType(desc.colorType)) {
		GLsizei imageSize = ColorUtil::bytesPerPixelBlockImageSize(w, h, desc.colorType);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, _format.internalFormat, w, h, 0, imageSize, pixelDataPtr);
		Util::throwIfError();
	} else {
		if (unpackAlignment != 4)
			glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);

		glTexImage2D(GL_TEXTURE_2D, 0, _format.internalFormat, w, h, 0, _format.sourceFormat, _format.elementType, pixelDataPtr);
		Util::throwIfError();

		if (unpackAlignment != 4)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	unbind();
	Util::throwIfError();
}


#if 0
#pragma mark ========= TextureCube_GL ============
#endif
TextureCube_GL::~TextureCube_GL() {
	if (_tex) {
		glDeleteTextures(1, &_tex);
		_tex = 0;
	}
}

TextureCube_GL::TextureCube_GL(CreateDesc& desc)
	: Base(desc)
{
	glGenTextures(1, &_tex);
	Util::throwIfError();

	_format.set(desc.colorType);

	bind();

	GLsizei w = desc.size.x;
	GLsizei h = desc.size.y;

	// TODO: upload texture with glCompressedTexSubImage2D/glTexSubImage2D
	GLint unpackAlignment = 4;
	
	static const constexpr int kFaceMaxCount = UploadRequest::kFaceMaxCount;

	Vector<const void*, kFaceMaxCount> pixelDataPtrs {
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	};

	if (desc.uploadRequest) {
		auto& reqs = desc.uploadRequest->reqs;

		for (int i = 0; i < reqs.size(); ++i) {
			const auto& req = reqs[i];

			const auto& info = req.imageInfo;
			const auto& pixelData = req.pixelData;

			if (info.colorType == ColorType::None) {
				throw SGE_ERROR("");
			}

			auto s = Math::clamp(info.size, Vec2i::s_zero(), size() - req.offset);
			if (s != info.size) {
				throw SGE_ERROR("out of texture area");
			}

			SGE_ASSERT(req.offset == Vec2i::s_zero()); // TODO: upload texture with glCompressedTexSubImage2D/glTexSubImage2D

			if (!ColorUtil::isCompressedType(desc.colorType)) { // TODO BlockCompress texture is not allow check ???
				int pixelSizeInBytes = info.pixelSizeInBytes();

				size_t expectedDataSize = info.strideInBytes * info.height();
				if (pixelData.size() < expectedDataSize) {
					throw SGE_ERROR("out of texture area");
				}
				if (pixelSizeInBytes % 4) unpackAlignment = 1;
			}
			pixelDataPtrs[i] = pixelData.data();
		}
	}

	SGE_ASSERT(pixelDataPtrs.size() == kFaceMaxCount);
	for (int i = 0; i < kFaceMaxCount; ++i) {
		auto* pixelDataPtr = pixelDataPtrs[i];
		GLenum target = Util::getGlTextureCubeFaceOrder(static_cast<TextureCubeFaceOrder>(i));

		if (ColorUtil::isCompressedType(desc.colorType)) {
			GLsizei imageSize = ColorUtil::bytesPerPixelBlockImageSize(w, h, desc.colorType);
			glCompressedTexImage2D(target, 0, _format.internalFormat, w, h, 0, imageSize, pixelDataPtr);
			Util::throwIfError();
		} else {
			if (unpackAlignment != 4)
				glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);

			glTexImage2D(target, 0, _format.internalFormat, w, h, 0, _format.sourceFormat, _format.elementType, pixelDataPtr);
			Util::throwIfError();

			if (unpackAlignment != 4)
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
	}

	unbind();
	Util::throwIfError();
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL