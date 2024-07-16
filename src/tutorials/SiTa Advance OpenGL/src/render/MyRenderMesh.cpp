#include "MyRenderMesh.h"
#include "../MyCommon.h"

namespace sge {

void MyRenderMesh::create(MyEditMesh& src) {
	clear();

	_vertexCount = src.pos.size();
	 _indexCount = src.indices.size();

	if (_vertexCount <= 0) return;

	_primitive = RenderPrimitiveType::Triangles;

	{
		_vertexBuffer.resize(_vertexCount);
		auto* dst = _vertexBuffer.begin();

		auto* pos = src.pos.begin();		auto* ed_pos = src.pos.end();
		auto* uv = src.uv[0].begin();		auto* ed_uv = src.uv[0].end();
		auto* color = src.color.begin();	auto* ed_color = src.color.end();
		auto* normal = src.normal.begin();	auto* ed_normal = src.normal.end();

		if (src.uv[0].size() < _vertexCount)
			SGE_ASSERT(false);

		if (src.color.size() < _vertexCount)
			SGE_ASSERT(false);

		if (src.normal.size() < _vertexCount)
			SGE_ASSERT(false);

		for (int i = 0; i < _vertexCount; ++i) {
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

	{
		if (_vertexCount >= std::numeric_limits<IndexType>::max()) {
			throw SGE_ERROR("too many vertexs");
		}

		if (_indexCount > 0) {
			_indexBuffer.resize(_indexCount);
			auto* _src = src.indices.begin();
			auto* _ed = src.indices.end();
			auto* dst = _indexBuffer.begin();
			for (; _src < _ed; _src++) {
				*dst = static_cast<IndexType>(*_src);
				dst++;
			}
		}
	}
}

void MyRenderMesh::clear() {
	_vertexBuffer.clear();
	_indexBuffer.clear();
	_vertexCount = 0;
	_indexCount  = 0;
}

void MyRenderMesh::draw() {
	if (_indexCount > 0) {
		_drawIndice();
	} else {
		_drawArray();
	}
}

void MyRenderMesh::_drawIndice() {
	if (_indexCount <= 0)
		return;

	auto stride = static_cast<GLint>(sizeof(decltype(_vertexBuffer)::value_type));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, _vertexBuffer[0].pos.data);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, _vertexBuffer[0].color.data);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, stride, _vertexBuffer[0].uv.data);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, _vertexBuffer[0].normal.data);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexBuffer.size()), GL_UNSIGNED_SHORT, _indexBuffer.data());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void MyRenderMesh::_drawArray() {
	if (_vertexCount <= 0)
		return;

	auto stride = static_cast<GLint>(sizeof(decltype(_vertexBuffer)::value_type));
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, _vertexBuffer[0].pos.data);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, _vertexBuffer[0].color.data);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, stride, _vertexBuffer[0].uv.data);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, _vertexBuffer[0].normal.data);

	{
		GLsizei triangleCount = static_cast<GLsizei>(_vertexCount / 3);
		glDrawArrays(GL_TRIANGLES, 0, triangleCount);
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

}