#pragma once

namespace sge {
/*
	The Texture class only needs a few important functions.
		- load a texture from a file
		- bind a texture index to a uniform index
		- deactivate a texture index
*/

class Texture : public RefCountBase {
public:
	Texture();
	Texture(StrView path);

	~Texture() { destroy(); }

	void destroy();

	void load(StrView path);

	void set(GLint uniformLoc, GLint texUnit);
	void unset(GLint uniformLoc);

	GLuint handle() const { return _handle; }

private:
	int		_width = 0;
	int		_height = 0;
	int		_channels = 0; // something like that RGBA channels
	GLuint	_handle = 0;   // OpenGL texture handle
};

}