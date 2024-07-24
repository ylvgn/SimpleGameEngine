#include "MyRenderMesh.h"
#include "../MyCommon.h"

namespace sge {

void MyVertexBuffer::create(const Span<const MyVertex_PosColorUv> data) {
	destroy();

	if (data.empty())
		return;

	glGenBuffers(1, &_p);
	glBindBuffer(GL_ARRAY_BUFFER, _p);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size_bytes()), data.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyVertexBuffer::destroy() {
	if (_p) {
		glDeleteBuffers(1, &_p);
		_p = 0;
	}
}

void MyIndexBuffer::create(const Span<const IndexType> data) {
	destroy();
	if (data.empty())
		return;

	glGenBuffers(1, &_p);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _p);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size_bytes()), data.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MyIndexBuffer::destroy() {
	if (_p) {
		glDeleteBuffers(1, &_p);
		_p = 0;
	}
}

void MyRenderMesh::create(MyEditMesh& src) {
	clear();

	vertexCount = src.vertexCount();
	indexCount = src.indexCount();

	if (vertexCount <= 0)
		return;

	primitive = src.primitive;

	Vector<Test_VertexType> vertexData;

	{ // vertex buffer
		vertexData.resize(vertexCount);
		auto* dst = vertexData.begin();

		auto* pos = src.pos.begin();		auto* ed_pos = src.pos.end();
		auto* uv = src.uv[0].begin();		auto* ed_uv = src.uv[0].end();
		auto* color = src.color.begin();	auto* ed_color = src.color.end();
		auto* normal = src.normal.begin();	auto* ed_normal = src.normal.end();

		if (src.uv[0].size() < vertexCount)
			SGE_ASSERT(false);

		if (src.color.size() < vertexCount)
			SGE_ASSERT(false);

		if (src.normal.size() < vertexCount)
			SGE_ASSERT(false);

		for (int i = 0; i < vertexCount; ++i) {
			dst->pos	= *pos;		pos++;
			dst->uv		= *uv;		uv++;
			dst->color	= *color;	color++;
			dst->normal = *normal;	normal++;

			dst++;
		}

		SGE_ASSERT(pos == ed_pos);
		SGE_ASSERT(uv == ed_uv);
		SGE_ASSERT(color == ed_color);
		SGE_ASSERT(normal == ed_normal);
	}

	{ // index buffer
		src.updateIndex();
	}

	_updateVBO(vertexData, src.indexData());
}

void MyRenderMesh::clear() {
	vertexCount = 0;
	 indexCount = 0;
}

void MyRenderMesh::_updateVBO(const Span<const Test_VertexType> vertexData, const Span<const IndexType> indexData) {
	vertexBuffer.create(vertexData);
	indexBuffer.create(indexData);
}

}