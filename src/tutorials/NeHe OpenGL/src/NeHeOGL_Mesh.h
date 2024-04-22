#pragma once

#include "NeHeOGL_Vertex.h"
#include "NeHeOGL_RenderState.h"

namespace sge {

struct NeHeOGL_Mesh__RenderState {
	using RenderState	= NeHeOGL_RenderState;
	using Cull			= RenderState::Cull;

	void createByInherit();
	void create(RenderState& rs);
	void bind();

	bool	wireframe			: 1;
	bool	textureMapping2d	: 1;
	bool	cullFace			: 1;
	bool	depthTest			: 1;
	bool	depthWriteMask		: 1;

	int		cullFaceMode;
	int		depthTestFunc;

	float	lineWidth;
};

class NeHeOGL_Mesh {
public:
	using MyRenderState	 = NeHeOGL_Mesh__RenderState;
	using VertexDataType = NeHeOGL_Vertex_PosColorUv;
	using RenderDataType = NeHe_RenderDataType;

	void clear();

	void createRect(float w, float h);
	void createCube(float w, float h, float d);
	void createPlane(float w, float h);
	void createGrid(int grideSize);
	void createCoordinate();

	void draw();
	void drawVertexes();

	RenderPrimitiveType				primitive = RenderPrimitiveType::Triangles;

	Vector<VertexDataType>			vertices;
	Vector<u32>						indices;

	NeHeOGL_RenderState				renderState;
private:
	void _addToIndiceOfGrid(int size, const Vec2i& step, bool flipXY);

	void _beginDraw();
	void _endDraw();

	void _bindVertexs();

	RenderDataType	_indexType = RenderDataType::UInt32;

	MyRenderState	_curRenderState;
	MyRenderState	_lastRenderState;
};

}