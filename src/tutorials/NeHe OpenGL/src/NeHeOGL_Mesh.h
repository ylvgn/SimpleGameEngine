#pragma once

#include "NeHeOGL_Vertex.h"
#include "NeHeOGL_RenderState.h"

namespace sge {

enum class NeHeOGL_RenderPrimitiveType {
	None,
	Points,
	Lines,
	Triangles,
};

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

	float	lineWidth = 1.f;
};

class NeHeOGL_Mesh {
public:
	using VertexDataType		= NeHeOGL_Vertex_PosColorUvNormal;

	using MyRenderState			= NeHeOGL_Mesh__RenderState;
	using RenderDataType		= NeHeOGL_RenderDataType;
	using RenderPrimitiveType	= NeHeOGL_RenderPrimitiveType;

	void clear();

	void createRect (float w, float h);
	void createCube (float w, float h, float d);
	void createPlane(float w, float h);
	void createGrid(int gridSize);
	void createCoordinate();

	void draw();
	void drawInherit();

	void drawVertexArrays();
	void drawInheritVertexArrays();

	void drawByVertexArrays();
	void drawByVertexElements();

	void bindVertexes();
	void unbindVertexes();

	void bindRenderState();
	void unbindRenderState();

	void loadObjFile(StrView filename);
	void loadObjMem(ByteSpan data, StrView filename);

	size_t vertexCount	() const { return vertices.size(); }
	size_t indexCount	() const { return _indexType == RenderDataType::UInt16 ? indices.size() : indices32.size(); }

	RenderDataType indexType() const { return _indexType; }

	RenderPrimitiveType				primitive = RenderPrimitiveType::Triangles;

	Vector<VertexDataType>			vertices;
	Vector<u16>						indices;
	Vector<u32>						indices32;

	NeHeOGL_RenderState				renderState;

private:
	void _addToIndiceOfGrid(int size, const Vec2i& step, bool flipXY);

	void _beginDraw();
	void _endDraw();

	RenderDataType	_indexType = RenderDataType::UInt16;

	MyRenderState	_curRenderState;
	MyRenderState	_lastRenderState;
};

}