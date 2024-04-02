#pragma once

#include "NeHeOGL_Vertex.h"
#include "NeHeOGL_RenderState.h"

namespace sge {

class NeHeOGL_Mesh {
	using VertexDataType = NeHeOGL_Vertex_PosColor;
	using VertexIndiceDataType = u16;
public:
	void clear();
	void draw();

	void createCube(float w, float h, float d);

	Vector<VertexDataType> vertices;
	Vector<VertexIndiceDataType> indices;

	NeHeOGL_RenderState renderState;

private:

	struct MyRenderState {
		MyRenderState()
			: wireframe(false)
			, cullFace(false)
			, depthTest(false)
			, depthWriteMask(true)
			, cullFaceMode(0)
			, depthTestFunc(0)
		{}

		bool wireframe		: 1;
		bool cullFace		: 1;
		bool depthTest		: 1;
		bool depthWriteMask : 1;

		int cullFaceMode;
		int depthTestFunc;
	};

	void _beginDraw();
	void _endDraw();

	void _bindVertexs();
	void _bindRenderState(MyRenderState& rs);

	MyRenderState _curRenderState;
	MyRenderState _lastRenderState;
};

}