#include "NeHeOGL_Texture2D.h"

namespace sge {

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

	glBindTexture(GL_TEXTURE_2D, _tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void NeHeOGL_Texture2D::unbind() {
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

}