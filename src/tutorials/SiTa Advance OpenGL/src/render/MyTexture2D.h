#pragma once

#include "MyRender_Common.h"

namespace sge {

class MyTexture2D : public Object {
public:
	~MyTexture2D() { destroy(); }

	void destroy();
	void create(int width, int height, Color4b* pixels);

	void bind() const;
	void unbind() const;

	bool	valid () const { return _tex != 0; }
	int		width () const { return _width; }
	int		height() const { return _height; }

	void	loadPngFile(StrView filename, bool mulAlpha);

private:
	GLuint	_tex = 0;
	int		_width = 0;
	int		_height = 0;
};

}
