#include "Texture.h"
#include "stb_image.h"
#include "glad/glad.h"

namespace sge {

Texture::Texture() :
	_width(0),
	_height(0),
	_channels(0)
{
	glGenTextures(1, &_handle);
}

Texture::~Texture() {
	glDeleteTextures(1, &_handle);
}

Texture::Texture(StrView path) {
	glGenTextures(1, &_handle);
	load(path);
}

void Texture::load(StrView path) {
	glBindTexture(GL_TEXTURE_2D, _handle);
	{
		int width, height, channels;

		TempString s(path);
		stbi_uc* data = stbi_load(
			s.c_str(),
			&width, &height, &channels,
			4
			/*
				The last argument specifies the number of components per pixel.
				By setting it to 4, all textures are loaded with RGBA channels.
			*/
		);

		// use glTexImage2D to upload the texture to the GPU
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA, width, height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
		);


		// and glGenerateMipmap to generate the appropriate mipmaps for the image
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		// (hard code) using the same mipmap level and wrapping parameters. You may want to try adding gettersand setters for these properties.
		// set the wrap mode to repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		_width		= width;
		_height		= height;
		_channels	= channels;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::set(u32 uniformIndex, u32 textureIndex) {
	// activate texture unit and bind with OpenGL handle
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, _handle);

	// set the specified uniform index to contain the texture
	glUniform1i(uniformIndex, textureIndex);
}

void Texture::unset(u32 textureIndex) {
	// activate texture unit and unbind with 0
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
}

}