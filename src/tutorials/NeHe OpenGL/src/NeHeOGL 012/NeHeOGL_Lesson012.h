#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* Display Lists
	* FYI: https://nehe.gamedev.net/tutorial/display_lists/15003/
*/

class NeHeOGL_Lesson012 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson012;

	using VertexType = NeHeOGL_Vertex_PosUv;

protected:
	virtual void onInitedGL() override;
	virtual void onDraw() override;
private:
	static constexpr int kColorCount = 5;

	const Vector<Color4b, kColorCount> kBoxColors_unused = { // 64 bytes (un_used)
		OGL::kbRed,
		OGL::kbOrange,
		OGL::kbYellow,
		OGL::kbGreen,
		OGL::kbCyan,
	};
	constexpr static const Color4b kBoxColors[kColorCount] = { // 20 bytes = 5 * sizeof(Color4b)
		OGL::kbRed,
		OGL::kbOrange,
		OGL::kbYellow,
		OGL::kbGreen,
		OGL::kbCyan,
	};

	static Color4b kTopColors[kColorCount];
	static_assert( sizeof(kTopColors) == sizeof(kBoxColors) );

	void _example1();

	void _buildLists();

	void _bindMeshByQuads(EditMesh& mesh);

	Texture2D	_tex;
	EditMesh	_boxMesh;
	EditMesh	_topMesh;

	GLuint  _nBoxDisplayList = 0; // Storage For The Display List
	GLuint  _nTopDisplayList = 0; // Storage For The Second Display List
};

}