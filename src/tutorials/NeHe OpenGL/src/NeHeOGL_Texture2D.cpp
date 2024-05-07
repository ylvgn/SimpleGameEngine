#include "NeHeOGL_Texture2D.h"

namespace sge {

GLenum NeHeOGL_Texture2D::_colorType2InternalFormat(ColorType v) {
	switch (v) {
//		case ColorType::Lb:		return GL_R8;
//		case ColorType::Ls:		return GL_R16;
		case ColorType::Lf:		return GL_R;

//		case ColorType::Rb:		return GL_R8;
//		case ColorType::Rs:		return GL_R16;
		case ColorType::Rf:		return GL_R;

//		case ColorType::RGb:	return GL_RG8;
//		case ColorType::RGs:	return GL_RG16;
//		case ColorType::RGf:	return GL_RG;

		case ColorType::RGBb:	return GL_RGB8;
		case ColorType::RGBs:	return GL_RGB16;
		case ColorType::RGBf:	return GL_RGB; // GL_RGB32F

		case ColorType::RGBAb:	return GL_RGBA8;
		case ColorType::RGBAs:	return GL_RGBA16;
		case ColorType::RGBAf:	return GL_RGBA;	// GL_RGBA32F

		default: throw SGE_ERROR("unsupported ColorType");
	}
}

void NeHeOGL_Texture2D::create(CreateDesc& desc) {
	destroy();
	_type = RenderDataType::Texture2D;

	glGenTextures(1, &_tex);
	OGL::ScopedBindTexture2D scoped(_tex);

	const auto& colorType = desc.imageToUpload.colorType();
	if (colorType != ColorType::None) {
		if (desc.samplerState.maxLOD == 0) {
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				_colorType2InternalFormat(colorType),
				desc.imageToUpload.width(),
				desc.imageToUpload.height(),
				0,
				OGLUtil::getGlColorType(colorType),
				GL_UNSIGNED_BYTE, // u8
				desc.imageToUpload.dataPtr()
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enumInt(desc.samplerState.filter));
		}
		else {
			gluBuild2DMipmaps(
				GL_TEXTURE_2D,
				static_cast<int>(desc.samplerState.maxLOD),
				desc.imageToUpload.width(),
				desc.imageToUpload.height(),
				OGLUtil::getGlColorType(colorType),
				GL_UNSIGNED_BYTE, // u8
				desc.imageToUpload.dataPtr()
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // add in Filter enum ???
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enumInt(desc.samplerState.filter));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, enumInt(desc.samplerState.wrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, enumInt(desc.samplerState.wrapV));
	}
}

void NeHeOGL_Texture2D::destroy() {
	if (_tex) {
		glDeleteTextures(1, &_tex);
		_tex = 0;
	}
}

void NeHeOGL_Texture2D::bind() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _tex);
}

void NeHeOGL_Texture2D::unbind() {
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

}