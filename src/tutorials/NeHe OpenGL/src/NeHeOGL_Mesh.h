#pragma once

#include "NeHeOGL_Vertex.h"
#include "NeHeOGL_RenderState.h"

namespace sge {

class NeHeOGL_Mesh {
public:
	using VertexDataType = NeHeOGL_Vertex_PosColorUv;
	using VertexIndiceDataType = u16;

	void clear();

	void createRect(float w, float h);
	void createCube(float w, float h, float d);

	void draw();

	Vector<VertexDataType>			vertices;
	Vector<VertexIndiceDataType>	indices;

	NeHeOGL_RenderState				renderState;

private:

	struct MyRenderState {
		using RenderState	= NeHeOGL_RenderState;
		using Cull			= RenderState::Cull;

		MyRenderState();

		void create(RenderState& rs);
		void bind();

		bool	wireframe			: 1;
		bool	textureMapping2d	: 1;
		bool	depthTest			: 1;
		bool	depthWriteMask		: 1;
		int		depthTestFunc;

		bool	cullFace			: 1;
		int		cullFaceMode;
	};
	MyRenderState _curRenderState;
	MyRenderState _lastRenderState;

	void _beginDraw();
	void _endDraw();

	void _bindVertexs();
};

}