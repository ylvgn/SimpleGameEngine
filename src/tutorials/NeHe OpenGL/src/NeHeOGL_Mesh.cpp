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

		p->pos.set(-x, y, 0); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p ++;
		p->pos.set(-x,-y, 0); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p ++;
		p->pos.set( x,-y, 0); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p ++;
		p->pos.set( x, y, 0); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p ++;
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

	{
		vertices.resize(8);
		auto* p = vertices.begin();
			p->pos.set(-x,-y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p ++;
			p->pos.set( x,-y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p ++;
			p->pos.set( x,-y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p ++;
			p->pos.set(-x,-y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p ++;
														  				
			p->pos.set(-x, y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p ++;
			p->pos.set( x, y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p ++;
			p->pos.set( x, y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p ++;
			p->pos.set(-x, y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p ++;
		SGE_ASSERT(p == vertices.end());
	}

	indices.assign({
		0, 1, 2, // top
		0, 2, 3,
		3, 2, 6, // front
		3, 6, 7,
		2, 1, 5, // rigt
		2, 5, 6,
		7, 6, 5, // bottom
		7, 5, 4,
		4, 5, 1, // back
		4, 1, 0,
		4, 0, 3, // left
		4, 3, 7,
	});
}

void NeHeOGL_Mesh::createPlane(float w, float h) {
	clear();

	float x = w * 0.5f;
	float z = h * 0.5f;

	vertices.resize(4);
	auto* p = vertices.begin();
		p->pos.set(-x, 0,-z); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p ++;
		p->pos.set(-x, 0, z); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p ++;
		p->pos.set( x, 0, z); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p ++;
		p->pos.set( x, 0,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p ++;
	SGE_ASSERT(p == vertices.end());

	indices.assign({
		0, 1, 2,
		0, 2, 3,
	});
}

void NeHeOGL_Mesh::createGrid(int gridSize) {
	clear();
	renderState.wireframe = true;

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

	int		triangleRowCount = 1;
	Vec2i	p0, p1, p2;
	u32		v1, v2, v3;

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

			v1 = p0.y * verticesPerRow + p0.x;
			v2 = p2.y * verticesPerRow + p2.x;
			v3 = p1.y * verticesPerRow + p1.x;

			int rx = direction.x < 0 ? 1 : 0;
			int ry = direction.y < 0 ? 1 : 0;
			int rf = flipXY ? 0 : 1;
			if (rx ^ ry ^ rf) {
				indices.push_back(v1);
				indices.push_back(v3);
				indices.push_back(v2);
			} else {
				indices.push_back(v1);
				indices.push_back(v2);
				indices.push_back(v3);
			}
		}

		center.y += direction.y;
		triangleRowCount += 2;
	}
}

void NeHeOGL_Mesh::createCoordinate() {
	primitive = RenderPrimitiveType::Lines;
	renderState.lineWidth = 2.5f;

	vertices.resize(6);

	auto* p = vertices.begin();
		p->pos.set(Vec3f::s_zero());	p->color.set(OGL::kbRed);	p++;
		p->pos.set(Vec3f::s_right());	p->color.set(OGL::kbRed);	p++;
		p->pos.set(Vec3f::s_zero());	p->color.set(OGL::kbGreen); p++;
		p->pos.set(Vec3f::s_up());		p->color.set(OGL::kbGreen); p++;
		p->pos.set(Vec3f::s_zero());	p->color.set(OGL::kbBlue);	p++;
		p->pos.set(Vec3f::s_forward()); p->color.set(OGL::kbBlue);	p++;
	SGE_ASSERT(p == vertices.end());
}

void NeHeOGL_Mesh::_beginDraw() {
	_lastRenderState.createByInherit();
	_curRenderState.create(renderState);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	_bindVertexs();

	_curRenderState.bind();
}

void NeHeOGL_Mesh::draw() {
	if (indices.empty())
		return;

	_beginDraw();

	glDrawElements(	OGLUtil::getGlPrimitiveTopology(primitive),
					static_cast<GLsizei>(indices.size()),
					OGLUtil::getGlFormat(_indexType),
					indices.data());

	_endDraw();
}

void NeHeOGL_Mesh::drawVertexes() {
	if (vertices.empty())
		return;

	_beginDraw();

	glDrawArrays(OGLUtil::getGlPrimitiveTopology(primitive), 0, static_cast<GLsizei>(vertices.size()));

	_endDraw();
}

void NeHeOGL_Mesh::_endDraw() {
	_lastRenderState.bind();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void NeHeOGL_Mesh::_bindVertexs() {
	size_t stride = sizeof(vertices[0]);

	OGL::vertexPointer(&vertices[0].pos, stride);
	OGL::colorPointer(&vertices[0].color, stride);
	OGL::texCoordPointer(&vertices[0].uv, stride);
}

}
