#include "MyEditMesh.h"

namespace sge {

void MyEditMesh::clear() {
	index.clear();
	pos.clear();
	for (int i = 0; i < kUvCountMax; ++i) {
		uv[i].clear();
	}
	color.clear();
	normal.clear();
}

void MyEditMesh::createGrid(int w, int h, float cellSize, const Color4b& c) {
	clear();

	primitive = RenderPrimitiveType::Lines;

	pos.resize((w + 1 + h + 1) * 2);
	color.resize(pos.size());
	uv[0].resize(pos.size());
	normal.resize(pos.size());

	auto* p = pos.begin();

	auto offset = Vec3f(float(w), 0, float(h)) * (-0.5f) * cellSize;

	for (float x = 0; x <= w; x++) {
		*p = Vec3f(x, 0,		0)	* cellSize + offset; p++;
		*p = Vec3f(x, 0, float(h))	* cellSize + offset; p++;
	}

	for (float z = 0; z <= h; z++) {
		*p = Vec3f(0,			0, z) * cellSize + offset; p++;
		*p = Vec3f(float(w),	0, z) * cellSize + offset; p++;
	}

	SGE_ASSERT(p == pos.end());

	setColor(c);
}

void MyEditMesh::createOrigin() {
	clear();

	primitive = RenderPrimitiveType::Lines;

	pos.resize(6);
	color.resize(pos.size());
	uv[0].resize(pos.size());
	normal.resize(pos.size());

	auto* p = pos.begin();
	auto* c = color.begin();

	Color4b r = {255, 0, 0, 255};
	Color4b g = {0, 255, 0, 255};
	Color4b b = {0, 0, 255, 255};

	*p = Vec3f::s_zero();		*c = r; p++; c++;
	*p = Vec3f::s_right();		*c = r; p++; c++;

	*p = Vec3f::s_zero();		*c = g; p++; c++;
	*p = Vec3f::s_up();			*c = g; p++; c++;

	*p = Vec3f::s_zero();		*c = b; p++; c++;
	*p = Vec3f::s_forward();	*c = b; p++; c++;

	SGE_ASSERT(p == pos.end());
	SGE_ASSERT(c == color.end());
}

void MyEditMesh::updateIndex() {
	size_t _vertexCount = index.size();
	if (_vertexCount >= std::numeric_limits<IndexType>::max()) {
		throw SGE_ERROR("too many vertexs");
	}

	using SRC = decltype(index)::value_type;
	using DST = decltype(_indexData)::value_type;

	_indexData.resize(_vertexCount);
	for (size_t i = 0; i < _vertexCount; i++) {
		SRC vi = index[i];
		_indexData[i] = static_cast<DST>(vi);
	}
}

void MyEditMesh::draw() {
	if (indexCount() > 0) {
		_drawElement();
	} else {
		_drawArray();
	}
}

void MyEditMesh::_drawElement() {
	GLsizei _indexCount = static_cast<GLsizei>(indexCount());

	if (_indexCount <= 0)
		return;

	GLsizei stride = 0;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, pos.data());

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, color.data());

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, stride, uv[0].data());

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, normal.data());

//-----
	glDrawElements(my_getGlPrimitiveTopology(primitive), _indexCount, GL_UNSIGNED_SHORT, _indexData.data()); // only support uint16_t

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void MyEditMesh::_drawArray() {
	GLsizei _vertexCount = static_cast<GLsizei>(vertexCount());
	if (_vertexCount <= 0)
		return;

	GLsizei stride = 0;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, pos.data());

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, color.data());

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, stride, uv[0].data());

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, normal.data());

//-----
	GLenum mode = my_getGlPrimitiveTopology(primitive);
	switch (primitive) {
		case RenderPrimitiveType::Points:		glDrawArrays(mode, 0, _vertexCount); break;
		case RenderPrimitiveType::Lines:		glDrawArrays(mode, 0, _vertexCount / 2); break;
		case RenderPrimitiveType::Triangles:	glDrawArrays(mode, 0, _vertexCount / 3); break; //GLsizei triangleCount = static_cast<GLsizei>(vertexCount() / 3);
		case RenderPrimitiveType::Quads:		glDrawArrays(mode, 0, _vertexCount / 4); break;
		default: throw SGE_ERROR("unsupported RenderPrimitiveType");
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void MyEditMesh::setColor(const Color4b& c) {
	color.resize(pos.size());
	for (auto& _c : color) {
		_c = c;
	}
}

}