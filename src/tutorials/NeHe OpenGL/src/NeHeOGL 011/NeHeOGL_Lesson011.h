#pragma once

#include "../NeHeOGL_NativeUI.h"

namespace sge {
/*
	* Flag Effect (Waving Texture)
	* FYI: https://nehe.gamedev.net/tutorial/flag_effect_(waving_texture)/16002/
*/

class NeHeOGL_Lesson011 : public NeHeOGL_NativeUIWindow {
	using Base = NeHeOGL_NativeUIWindow;
	using This = NeHeOGL_Lesson011;

	using VertexType = NeHeOGL_Vertex_PosUv;
protected:
	virtual void onInitedGL() override;
	virtual void onDraw() override;
private:

	static constexpr int kSize = 45;

	void _example1();

	Tuple3f* _getVertexPos(int x, int y) {
		if ((x < 0 || x >= kSize) || (y < 0 || y >= kSize)) {
			return nullptr;
		}
		return &_mesh.pos[y * kSize + x];
	}

	Tuple2f* _getVertexUv(int x, int y) {
		if ((x < 0 || x >= kSize) || (y < 0 || y >= kSize)) {
			return nullptr;
		}
		return &_mesh.uv[0][y * kSize + x];
	}

	void _bindMesh(int x, int y);
	void _bindMeshByQuads(int x, int y);

	MyHiResTimer _uptime;

	Texture2D	_tex;
	EditMesh	_mesh;

	int	_wiggleCount = 0;
};

}