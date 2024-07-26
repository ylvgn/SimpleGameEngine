#pragma once

namespace sge {

class MyTexture2D : public NonCopyable {
public:
	~MyTexture2D() { destroy(); }

	void destroy();

	void create(int width, int height, Color4b* pixels);
	void subImage(int x, int y, int width, int height, Color4b* pixels);

	void bind();
	void unbind();

	bool	valid()		const { return _tex != 0; }
	int		width()		const { return _width; }
	int		height()	const { return _height; }

private:
	GLuint	_tex = 0;
	int		_width = 0;
	int		_height = 0;
};

}