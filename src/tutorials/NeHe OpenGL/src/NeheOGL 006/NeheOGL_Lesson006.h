#pragma once

#include "../NeheOGL_NativeUI.h"

#define STBI_FAILURE_USERMSG

namespace sge {
/*
	* Texture Mapping
	* FYI: https://nehe.gamedev.net/tutorial/texture_mapping/12038/
*/

class NeheOGL_Lesson006 : public NeheOGL_NativeUIWindow {
	using Base = NeheOGL_NativeUIWindow;
	using This = NeheOGL_Lesson006;
public:
	~NeheOGL_Lesson006() {
		glDeleteTextures(1, &_texture2d);
	}

	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
private:
	void _loadByHBITMAP(MyImage& o, StrView filename);

	void _loadTexture2D(StrView filename, GLuint targetTexture);
	void _example1(float uptime);

	MyImage _imageToUpload;
	GLuint  _texture2d;			 // Storage For One 2D Texture (OpenGL texture handle)

	MyHiResTimer _uptime;
};

}