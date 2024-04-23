#include "NeHeOGL_Texture2D.h"

namespace sge {

GLenum NeHeOGL_Texture2D::_colorType2InternalFormat(ColorType v) {
	auto colorModel = ColorUtil::colorModel(v);
	switch (colorModel) {
		case sge::ColorModel::R: return GL_RED;
		case sge::ColorModel::L: return GL_RED;
	/*
		case sge::ColorModel::RG:
		case sge::ColorModel::LA: return GL_RG;
	*/
		case sge::ColorModel::RGB:	return GL_RGB;
		case sge::ColorModel::RGBA: return GL_RGBA;
		default: throw SGE_ERROR("unsupported ColorType");
	}
}

void NeHeOGL_Texture2D::create(CreateDesc& desc) {
	destroy();
	_type = RenderDataType::Texture2D;

	glGenTextures(1, &_tex);
	OGL::ScopedBindTexture2D scoped(_tex);

	if (desc.imageToUpload.colorType() != ColorType::None) {
		if (desc.samplerState.maxLOD == 0) {
			glTexImage2D(GL_TEXTURE_2D,
				0,
				_colorType2InternalFormat(desc.imageToUpload.colorType()),
				desc.imageToUpload.width(),
				desc.imageToUpload.height(),
				0,
				OGLUtil::getGlColorType(desc.imageToUpload.colorType()),
				GL_UNSIGNED_BYTE, // u8
				desc.imageToUpload.dataPtr()
			);
		}
		else {
			gluBuild2DMipmaps(GL_TEXTURE_2D,
				static_cast<int>(desc.samplerState.maxLOD),
				desc.imageToUpload.width(),
				desc.imageToUpload.height(),
				OGLUtil::getGlColorType(desc.imageToUpload.colorType()),
				GL_UNSIGNED_BYTE,
				desc.imageToUpload.dataPtr()
			);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enumInt(desc.samplerState.minFilter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enumInt(desc.samplerState.magFilter));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, enumInt(desc.samplerState.wrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, enumInt(desc.samplerState.wrapV));
	}
}

void NeHeOGL_Texture2D::createTest() {
	destroy();
	glGenTextures(1, &_tex);

	Vector<Color4b> pixels;
	pixels.resize(256 * 256);
	auto* p = pixels.data();
	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			p->set(static_cast<u8>(x), static_cast<u8>(y), 0, 255);
			p++;
		}
	}

	OGL::ScopedBindTexture2D scoped(_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void NeHeOGL_Texture2D::createByLoadFile(StrView filename) {
	Image img;
	img.loadFile(filename);
	
	img.colorType();

	OGL::ScopedBindTexture2D scoped(_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.dataPtr());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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