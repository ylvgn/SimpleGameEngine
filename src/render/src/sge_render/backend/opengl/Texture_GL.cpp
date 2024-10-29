#include "Texture_GL.h"
#include "Renderer_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {
#if 0
#pragma mark ========= Texture2D_GL::Format ============
#endif
void Texture2D_GL::Format::set(ColorType colorType) {
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
		auto& image = desc.uploadRequest->imageToUpload;
		auto pixelData = image.pixelData();

		if (image.colorType() == ColorType::None) {
			throw SGE_ERROR("");
		}

		auto s = Math::clamp(image.size(), Vec2i::s_zero(), size() - desc.uploadRequest->offset);
		if (s != image.size()) {
			throw SGE_ERROR("out of texture area");
		}
		SGE_ASSERT(desc.uploadRequest->offset == Vec2i::s_zero()); // TODO: upload texture with glCompressedTexSubImage2D/glTexSubImage2D

		if (!ColorUtil::isCompressedType(desc.colorType)) { // TODO BlockCompress texture is not allow check ???
			int pixelSizeInBytes = image.pixelSizeInBytes();

			size_t expectedDataSize = image.strideInBytes() * image.height();
			if (pixelData.size() < expectedDataSize) {
				throw SGE_ERROR("out of texture area");
			}

			if (pixelSizeInBytes % 4) unpackAlignment = 1;
		}

		pixelDataPtr = pixelData.data();
	}

	if (unpackAlignment != 4) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
		Util::throwIfError();
	}

	if (ColorUtil::isCompressedType(desc.colorType)) {
		GLsizei imageSize = ColorUtil::bytesPerPixelBlockImageSize(w, h, desc.colorType);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, _format.internalFormat, w, h, 0, imageSize, pixelDataPtr);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, _format.internalFormat, w, h, 0, _format.sourceFormat, _format.elementType, pixelDataPtr);
	}
	Util::throwIfError();

	if (unpackAlignment != 4) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		Util::throwIfError();
	}

	unbind();
	Util::throwIfError();
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL