#include "MyTexture2D.h"

namespace sge {

void MyTexture2D::destroy() {
	if (_tex) {
		glDeleteTextures(1, &_tex);
		_tex = 0;
	}
}

void MyTexture2D::create(int width, int height, Color4b* pixels) {
	destroy();

	if (!Math::isPow2(width) || !Math::isPow2(height))
		throw SGE_ERROR("width or height must be power of 2");

	glGenTextures(1, &_tex);
	glBindTexture(GL_TEXTURE_2D, _tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	_width  = width;
	_height = height;
}

void MyTexture2D::subImage(int x, int y, int width, int height, Color4b* pixels) {
	glBindTexture(GL_TEXTURE_2D, _tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyTexture2D::bind() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void MyTexture2D::unbind() {
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

}