#pragma once

#include "NeHeOGL_Vertex.h"
#include "NeHeOGL_RenderState.h"

namespace sge {

class NeHeOGL_Mesh {
public:
	using VertexDataType = NeHeOGL_Vertex_PosColor;
	using VertexIndiceDataType = u16;

	void clear();
	void draw();

	void createCube(float w, float h, float d);

	Vector<VertexDataType> vertices;
	Vector<VertexIndiceDataType> indices;

	NeHeOGL_RenderState renderState;

private:
	struct MyRenderState {
		using RenderState	= NeHeOGL_RenderState;
		using Cull			= RenderState::Cull;

		MyRenderState()
			: wireframe(false)
			, cullFace(false)
			, depthTest(false)
			, depthWriteMask(true)
			, cullFaceMode(0)
			, depthTestFunc(0)
		{}

		void create();
		void create(RenderState& rs);
		
		void bind();

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

	MyRenderState _curRenderState;
	MyRenderState _lastRenderState;
};

}