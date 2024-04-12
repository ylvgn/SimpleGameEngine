#pragma once

#include "NeHeOGL_GL_Common.h"
#include "NeHeOGL_Image.h"

namespace sge {

class NeHeOGL_Texture2D : public NonCopyable {
public:
	using Image = NeHeOGL_Image;

	~NeHeOGL_Texture2D() { destroy(); }

	void createTest();
	void createByLoadFile(StrView filename);

	void destroy();

	void bind();
	void unbind();
private:
	GLuint _tex = 0;
};

}