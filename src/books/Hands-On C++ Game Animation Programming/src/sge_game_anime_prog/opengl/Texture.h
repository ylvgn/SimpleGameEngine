#pragma once

namespace sge {
/*
	The Texture class only needs a few important functions.
	-load a texture from a file
	-bind a texture index to a uniform index
	-deactivate a texture index
*/

class Texture : public RefCountBase {
public:
	Texture();
	Texture(StrView path);

	~Texture();

	void load(StrView path);

	void set(u32 uniformIndex, u32 textureIndex);
	void unset(u32 textureIndex);

	u32 handle() const { return _handle; }

private:
	u32 _width;
	u32 _height;
	u32 _channels; // somthing like that RGBA channels
	u32 _handle;   // OpenGL texture handle
};

}