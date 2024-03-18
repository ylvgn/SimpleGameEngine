#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* Texture Mapping
	* FYI: https://nehe.gamedev.net/tutorial/texture_mapping/12038/
*/

class NeHeOGL_Lesson006 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson006;

	static const int kTexture2dCount = 2;

public:
	~NeHeOGL_Lesson006() {
		glDeleteTextures(1, &_texture2d);
		glDeleteTextures(kTexture2dCount, _texture2ds);
	}
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;

private:

	struct MyMesh : public NonCopyable {
		Vector<Tuple3f> pos;
		Vector<Tuple2f> uv;
		Vector<u32>	indices;

		void clean() {
			pos.clear();
			uv.clear();
			indices.clear();
		}

		void bind() {
			glBegin(GL_QUADS);
				int i = 0;
				for (auto& indice : indices) {
					OGL::glTexCoord(uv[i]);
					OGL::glVertex(pos[indice]);
					++i;
				}
			glEnd();
		}
	};
#if SGE_OS_WINDOWS
	void _loadByHBITMAP(MyImage& o, StrView filename);
#endif
	void _loadTexture2D(StrView filename, MyImage& img, GLuint targetTexture);
	void _addCameraOrbitAngle(const Vec2f& deltaAngle);
	void _example1(float uptime);
	void _example2(float uptime);
	void _example3(float uptime);

	Vec2f _camerOrbitAngle	{0,0};		// camera rotate with x-axis y-axis
	Vec3f _camerMovePos		{0,0,0};	// camera move, zoom in/out

	MyHiResTimer _uptime;

	// Storage For One 2D Texture (OpenGL texture handle)
	GLuint  _texture2d;
	MyImage _imageToUpload;

	// Storage For More Than One 2D Texture (OpenGL texture handles)
	GLuint  _texture2ds[kTexture2dCount];
	MyImage _imagesToUpload[kTexture2dCount];

	MyMesh	_mesh;
};

}