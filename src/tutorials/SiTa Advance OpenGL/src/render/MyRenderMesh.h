#pragma once

#include "MyEditMesh.h"

namespace sge {

class MyRenderMesh {
public:
	void create(MyEditMesh& src);
	void clear();

	void draw();

	RenderPrimitiveType primitive() const { return _primitive; }

	size_t vertexCount()			const { return _vertexCount; }
	size_t indexCount()				const { return _indexCount; }

private:
	void _drawIndice();
	void _drawArray();

	using IndexType = u16;

	struct MyVertex {
		Color4b	color;
		Vec2f	uv;
		Vec3f	pos;
		Vec3f	normal;
	};

	Vector<MyVertex>	_vertexBuffer;
	Vector<IndexType>	_indexBuffer;

	size_t _vertexCount = 0;
	size_t _indexCount  = 0;

	RenderPrimitiveType _primitive = RenderPrimitiveType::Triangles;
};

}