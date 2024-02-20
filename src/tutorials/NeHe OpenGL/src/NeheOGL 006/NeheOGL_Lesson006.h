#pragma once

#include "../NeheOGL_NativeUI.h"

namespace sge {
/*
	* Texture Mapping
	* FYI: https://nehe.gamedev.net/tutorial/texture_mapping/12038/
*/

struct MyImage {
	int width = 0;
	int height = 0;
	Vector<u8> pixelData;

	void clean() {
		width = 0;
		height = 0;
		pixelData.clear();
	}
};

class NeheOGL_Lesson006 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson006;
public:

	~NeheOGL_Lesson006() {
		if (_bmp) {
			DeleteObject(_bmp);
			_bmp = nullptr;
		}
		glDeleteTextures(1, &_texture2d);
	}

	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;

private:
	void _hBitMapToImage(MyImage& o, HBITMAP BitmapHandle);
	void _loadImage(MyImage& o, StrView filename);
	void _loadTexture2D(StrView filename, GLuint targetTexture);

	void _example1(float uptime);

	HBITMAP _bmp = nullptr;
	MyHiResTimer _uptime;
	
	GLuint  _texture2d;			 // Storage For One 2D Texture (OpenGL texture handle)
};

}