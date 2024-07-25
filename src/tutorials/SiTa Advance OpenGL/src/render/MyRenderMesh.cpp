#include "MyRenderMesh.h"
#include "../MyCommon.h"

namespace sge {

template<class MyVertexT>
void MyVertexBuffer::create(const Span<const MyVertexT> data) {
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

	primitive = src.primitive;

	using MyVertexT = Test_VertexType;
	Vector<MyVertexT> vertexData;

	vertexCount = src.vertexCount();
	if (vertexCount > 0) {
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

	indexCount = src.indexCount();
	if (indexCount > 0) {
		src.updateIndex();
	}

	_updateVBO<MyVertexT>(vertexData, src.indexData());
}

void MyRenderMesh::createCg(MyEditMesh& src) {
	clear();

	primitive = src.primitive;

	using MyVertexT = TestCg_VertexType;
	Vector<MyVertexT> vertexData;

	vertexCount = src.vertexCount();
	if (vertexCount > 0) {
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
			dst->pos.x = pos->x;
			dst->pos.y = pos->y;
			dst->pos.z = pos->z;
			dst->pos.w = 1;
			pos++;

			dst->color.r = float(color->r) / 255;
			dst->color.g = float(color->g) / 255;
			dst->color.b = float(color->b) / 255;
			dst->color.a = float(color->a) / 255;
			color++;

			dst->normal.x = normal->x;
			dst->normal.y = normal->y;
			dst->normal.z = normal->z;
			dst->normal.w = 0;
			normal++;

			dst->uv		= *uv;		uv++;

			dst++;
		}

		SGE_ASSERT(pos == ed_pos);
		SGE_ASSERT(uv == ed_uv);
		SGE_ASSERT(color == ed_color);
		SGE_ASSERT(normal == ed_normal);
	}

	indexCount = src.indexCount();
	if (indexCount > 0) {
		src.updateIndex();
	}

	_updateVBO<MyVertexT>(vertexData, src.indexData());
}

void MyRenderMesh::clear() {
	vertexCount = 0;
	 indexCount = 0;
}

}