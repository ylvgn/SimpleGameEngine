#include "NeHeOGL_Mesh.h"
#include "NeHeOGL_GL_Common.h"

namespace sge {

void NeHeOGL_Mesh__RenderState::createByInherit() {
	int			oldInt32;
	int			oldInt32x2[2];
	GLboolean	oldBoolean;
	GLfloat		oldFloat32;

// polygon mode
	glGetIntegerv(GL_POLYGON_MODE, oldInt32x2);
//	GLenum polygonFace = oldInt32x2[0];
	int polygonMode = oldInt32x2[1];
	wireframe = polygonMode != GL_FILL;

// cull face
	glGetBooleanv(GL_CULL_FACE, &oldBoolean);
	if (!oldBoolean) {
		cullFace = false;
		cullFaceMode = GL_BACK;
	} else {
		cullFace = true;

		glGetIntegerv(GL_CULL_FACE_MODE, &oldInt32);
		cullFaceMode = oldInt32;
	}

	// depth test
	glGetBooleanv(GL_DEPTH_TEST, &oldBoolean);
	if (!oldBoolean) {
		depthTest = false;
		depthTestFunc = GL_LESS;
		depthWriteMask = true;
	}
	else {
		depthTest = true;

		glGetIntegerv(GL_DEPTH_FUNC, &oldInt32);
		depthTestFunc = oldInt32;

		glGetBooleanv(GL_DEPTH_WRITEMASK, &oldBoolean);
		depthWriteMask = static_cast<bool>(oldBoolean);
	}

// texture mapping
	glGetBooleanv(GL_TEXTURE_2D, &oldBoolean);
	textureMapping2d = static_cast<bool>(oldBoolean);

// line width
	glGetFloatv(GL_LINE_WIDTH, &oldFloat32);
	lineWidth = oldFloat32;
}

void NeHeOGL_Mesh__RenderState::create(RenderState& rs) {
// polygon mode
	wireframe			= rs.wireframe;
// cull face
	cullFace			= rs.cull != Cull::None;
	cullFaceMode		= OGLUtil::getGlCullMode(rs.cull);
// depth test
	depthTest			= rs.depthTest.isEnable();
	depthTestFunc		= OGLUtil::getGlDepthTestOp(rs.depthTest.op);
	depthWriteMask		= rs.depthTest.writeMask;
// texture mapping
	textureMapping2d	= true;
// line width
	lineWidth			= rs.lineWidth;
}

void NeHeOGL_Mesh__RenderState::bind() {
// polygon mode
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// cull face
	if (cullFace) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
	glCullFace(cullFaceMode);

// depth test
	if (depthTest) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
	glDepthFunc(depthTestFunc);
	glDepthMask(depthWriteMask ? GL_TRUE : GL_FALSE);

// texture mapping
	if (textureMapping2d) glEnable(GL_TEXTURE_2D);
	else glDisable(GL_TEXTURE_2D);

// line width
	glLineWidth(lineWidth);
}

void NeHeOGL_Mesh::clear() {
	vertices.clear();
	indices.clear();
}

void NeHeOGL_Mesh::createRect(float w, float h) {
	clear();

	float x = w * 0.5f;
	float y = h * 0.5f;

	{
		vertices.resize(4);
		auto* p = vertices.data();

		p->pos.set(-x, y, 0); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p->normal.set(0,0,0); p ++;
		p->pos.set(-x,-y, 0); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p->normal.set(0,0,0); p ++;
		p->pos.set( x,-y, 0); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p->normal.set(0,0,0); p ++;
		p->pos.set( x, y, 0); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p->normal.set(0,0,0); p ++;
	}

	indices.assign({
		0, 1, 2,
		0, 2, 3,
	});
}

void NeHeOGL_Mesh::createCube(float w, float h, float d) {
	clear();

	float x = w * 0.5f;
	float y = h * 0.5f;
	float z = d * 0.5f;

	Vec3f v[] = {
		{-x, y,-z},
		{ x, y,-z},
		{ x, y, z},
		{-x, y, z},
		{-x,-y,-z},
		{ x,-y,-z},
		{ x,-y, z},
		{-x,-y, z},
	};

	vertices.resize(36);
	auto* p = vertices.begin();
		// top
		p->pos.set(v[0]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[1]); p->uv.set(1,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[2]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		p->pos.set(v[0]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[2]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[3]); p->uv.set(0,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		// front
		p->pos.set(v[3]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[2]); p->uv.set(1,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[6]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		p->pos.set(v[3]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[6]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[7]); p->uv.set(0,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		// right
		p->pos.set(v[1]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[5]); p->uv.set(1,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[6]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		p->pos.set(v[1]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[6]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[2]); p->uv.set(0,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		// bottom
		p->pos.set(v[7]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[6]); p->uv.set(1,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[5]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		p->pos.set(v[7]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[5]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[4]); p->uv.set(0,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		// back
		p->pos.set(v[4]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[5]); p->uv.set(1,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[1]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		p->pos.set(v[4]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[1]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[0]); p->uv.set(0,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		// left
		p->pos.set(v[4]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[0]); p->uv.set(1,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[3]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;

		p->pos.set(v[4]); p->uv.set(0,0); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[3]); p->uv.set(1,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
		p->pos.set(v[7]); p->uv.set(0,1); p->color.set(OGL::kbWhite); p->normal.set(0,0,0); p++;
	SGE_ASSERT(p == vertices.end());

	indices.resize(36);
	for (u16 i = 0; i < 36; ++i) {
		indices[i] = i;
	}
}

void NeHeOGL_Mesh::createPlane(float w, float h) {
	clear();

	float x = w * 0.5f;
	float z = h * 0.5f;

	vertices.resize(4);
	auto* p = vertices.begin();
		p->pos.set(-x, 0,-z); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p->normal.set(0,0,0); p++;
		p->pos.set(-x, 0, z); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p->normal.set(0,0,0); p++;
		p->pos.set( x, 0, z); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p->normal.set(0,0,0); p++;
		p->pos.set( x, 0,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p->normal.set(0,0,0); p++;
	SGE_ASSERT(p == vertices.end());

	indices.assign({
		0, 1, 2,
		0, 2, 3,
	});
}

void NeHeOGL_Mesh::createGrid(int gridSize) {
	clear();

	_indexType = RenderDataType::UInt32;

	if (gridSize <= 1) {
		createPlane(1, 1);
		return;
	}

	int verticesPerRow = gridSize + 1;
	vertices.resize(verticesPerRow * verticesPerRow);

	auto* p = vertices.begin();
		Vec3f offset { -gridSize *0.5f, 0, -gridSize *0.5f };
		for (float y = 0; y < verticesPerRow; y ++) {
			for (float x = 0; x < verticesPerRow; x++) {
				p->color.set(OGL::kbWhite);
				p->pos.set(x, 0, y);
				p->pos += offset;
				p->uv.set(x / (verticesPerRow-1), y / (verticesPerRow-1));
				p->normal.set(0,0,0);
				p++;
			}
		}
	SGE_ASSERT(p == vertices.end());

	_addToIndiceOfGrid(verticesPerRow, {  1, 1 }, false);
	_addToIndiceOfGrid(verticesPerRow, { -1,-1 }, false);
	_addToIndiceOfGrid(verticesPerRow, { -1, 1 }, false);
	_addToIndiceOfGrid(verticesPerRow, {  1,-1 }, false);
	_addToIndiceOfGrid(verticesPerRow, {  1, 1 }, true );
	_addToIndiceOfGrid(verticesPerRow, { -1,-1 }, true );
	_addToIndiceOfGrid(verticesPerRow, { -1, 1 }, true );
	_addToIndiceOfGrid(verticesPerRow, {  1,-1 }, true );
}

void NeHeOGL_Mesh::_addToIndiceOfGrid(int verticesPerRow, const Vec2i& direction, bool flipXY) {
	int totalRow = verticesPerRow / 2;
	Vec2i center { verticesPerRow / 2, verticesPerRow / 2 };

	Span<u32> dstSpan;
	{
		size_t oldSize = indexCount();
		// tot: n*a1+n*(n-1)*d/2
			// 1: 1 triangle
			// 2: 3(1+2)
			// 3: 5(3+2)
			// 4: 7(5+2)
			// 5: 9(7+2)
		int tot = totalRow*1 + totalRow*(totalRow - 1);
		size_t newSize = oldSize + tot*3;
		indices32.resize(newSize);
		dstSpan = indices32.subspan(oldSize);
	}

	{
		auto* dst = dstSpan.begin();
		
		int	triangleRowCount = 1;
		Vec2i p0, p1, p2;

		for (int row = 1; row <= totalRow; row++) {
			Vec2i st(center);
			for (int i = 0; i < triangleRowCount; ++i) {
				p0.set(st);
				int odd = i % 2;

				if (odd) {
					p1.set(st.x - direction.x,	st.y);
					p2.set(st.x,				st.y + direction.y);

				} else {
					p1.set(st.x,				st.y + direction.y);
					p2.set(st.x + direction.x,	st.y + direction.y);

					st.x += direction.x;
				}

				if (flipXY) {
					p0 = p0.yx();
					p1 = p1.yx();
					p2 = p2.yx();
				}

				*dst = p0.y * verticesPerRow + p0.x; dst++;
				*dst = p2.y * verticesPerRow + p2.x; dst++;
				*dst = p1.y * verticesPerRow + p1.x; dst++;
			}

			center.y += direction.y;
			triangleRowCount += 2;
		}

		SGE_ASSERT(dst == dstSpan.end());
	}

	int rx = direction.x < 0 ? 1 : 0;
	int ry = direction.y < 0 ? 1 : 0;
	int rf = flipXY ? 0 : 1;
	if (rx ^ ry ^ rf) {
		auto* dst = dstSpan.begin();
		auto* end = dstSpan.end();
		for (; dst < end; dst+=3) {
			swap(dst[0], dst[1]);
		}
		SGE_ASSERT(dst == dstSpan.end());
	}
}

void NeHeOGL_Mesh::createCoordinate() {
	clear();

	primitive = RenderPrimitiveType::Lines;
	renderState.lineWidth = 2.5f;
	renderState.cull = RenderState_Cull::None;
	renderState.wireframe = false;

	vertices.resize(6);
	auto* p = vertices.begin();
		p->pos.set(Vec3f::s_zero());	p->color.set(OGL::kbRed);	p->uv.set(0,0); p->normal.set(0,0,0); p++;
		p->pos.set(Vec3f::s_right());	p->color.set(OGL::kbRed);	p->uv.set(0,0); p->normal.set(0,0,0); p++;
		p->pos.set(Vec3f::s_zero());	p->color.set(OGL::kbGreen); p->uv.set(0,0); p->normal.set(0,0,0); p++;
		p->pos.set(Vec3f::s_up());		p->color.set(OGL::kbGreen); p->uv.set(0,0); p->normal.set(0,0,0); p++;
		p->pos.set(Vec3f::s_zero());	p->color.set(OGL::kbBlue);	p->uv.set(0,0); p->normal.set(0,0,0); p++;
		p->pos.set(Vec3f::s_forward()); p->color.set(OGL::kbBlue);	p->uv.set(0,0); p->normal.set(0,0,0); p++;
	SGE_ASSERT(p == vertices.end());
}

void NeHeOGL_Mesh::bindVertexes() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	size_t stride = sizeof(vertices[0]);
	OGL::vertexPointer(&vertices[0].pos, stride);
	OGL::colorPointer(&vertices[0].color, stride);
	OGL::texCoordPointer(&vertices[0].uv, stride);
}

void NeHeOGL_Mesh::unbindVertexes() {
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void NeHeOGL_Mesh::bindRenderState() {
	_lastRenderState.createByInherit();
	_curRenderState.create(renderState);
	_curRenderState.bind();
}

void NeHeOGL_Mesh::unbindRenderState() {
	_lastRenderState.bind();
}

void NeHeOGL_Mesh::loadObjFile(StrView filename) {
	WavefrontObjLoader::loadFile(*this, filename);
}

void NeHeOGL_Mesh::loadObjMem(ByteSpan data, StrView filename) {
	WavefrontObjLoader::loadMem(*this, data, filename);
}

void NeHeOGL_Mesh::_beginDraw() {
	bindRenderState();
	bindVertexes();
}

void NeHeOGL_Mesh::draw() {
	if (indices.empty())
		return;

	_beginDraw();
		drawByVertexElements();
	_endDraw();
}

void NeHeOGL_Mesh::_endDraw() {
	unbindRenderState();
	unbindVertexes();
}

void NeHeOGL_Mesh::drawInherit() {
	if (indices.empty())
		return;

	bindVertexes();
		drawByVertexElements();
	unbindVertexes();
}

void NeHeOGL_Mesh::drawVertexArrays() {
	if (vertices.empty())
		return;

	_beginDraw();
		drawByVertexArrays();
	_endDraw();
}

void NeHeOGL_Mesh::drawInheritVertexArrays() {
	if (vertices.empty())
		return;

	bindVertexes();
		drawByVertexArrays();
	unbindVertexes();
}

void NeHeOGL_Mesh::drawByVertexElements() {
	if (_indexType == RenderDataType::UInt16) {
		glDrawElements(
			OGLUtil::getGlPrimitiveTopology(primitive),
			static_cast<GLsizei>(indices.size()),
			OGLUtil::getGlFormat(_indexType),
			indices.data()
		);
	}
	else if (_indexType == RenderDataType::UInt32) {
		glDrawElements(
			OGLUtil::getGlPrimitiveTopology(primitive),
			static_cast<GLsizei>(indices32.size()),
			OGLUtil::getGlFormat(_indexType),
			indices32.data()
		);
	}
	else
		throw SGE_ERROR("unsupported index type");
}

void NeHeOGL_Mesh::drawByVertexArrays() {
	glDrawArrays(
		OGLUtil::getGlPrimitiveTopology(primitive),
		0,
		static_cast<GLsizei>(vertices.size())
	);
}

}
