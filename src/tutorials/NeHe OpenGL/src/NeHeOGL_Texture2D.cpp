#include "NeHeOGL_Texture2D.h"
#include "NeHeOGL_GL_Common.h"

namespace sge {

void NeHeOGL_Texture2D::create(CreateDesc& desc) {
	destroy();

	_type		= RenderDataType::Texture2D;
	_colorType	= desc.colorType;
	_size		= desc.size;

	glGenTextures(1, &_tex);
	OGL::Scoped_glBindTexture scoped(_tex);
	const auto& colorType = desc.imageToUpload.colorType();
	if (colorType != ColorType::None) {
		if (desc.samplerState.maxLOD == 0) {
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				OGLUtil::getGlInternalFormat(colorType),
				desc.imageToUpload.width(),
				desc.imageToUpload.height(),
				0,
				OGLUtil::getGlColorType(colorType),
				GL_UNSIGNED_BYTE, // u8
				desc.imageToUpload.dataPtr()
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OGLUtil::getGlTextureFilter(desc.samplerState.filter));
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OGLUtil::getGlTextureFilter(desc.samplerState.filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OGLUtil::getGlTextureWrap(desc.samplerState.wrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OGLUtil::getGlTextureWrap(desc.samplerState.wrapV));
	}

	OGLUtil::throwIfError();
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