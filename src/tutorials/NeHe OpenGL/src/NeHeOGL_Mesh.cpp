#include "NeHeOGL_Mesh.h"
#include "NeHeOGL_GL_Common.h"

namespace sge {

NeHeOGL_Mesh__RenderState::NeHeOGL_Mesh__RenderState() {
	int oldValue;
	int oldValue2[2];
	GLboolean oldBool;

// polygon mode
	glGetIntegerv(GL_POLYGON_MODE, oldValue2);
//	GLenum polygonFace = oldValue2[0];
	int polygonMode = oldValue2[1];
	wireframe = polygonMode != GL_FILL;

// cull face
	glGetBooleanv(GL_CULL_FACE, &oldBool);
	if (!oldBool) {
		cullFace = false;
		cullFaceMode = GL_BACK;
	} else {
		cullFace = true;

		glGetIntegerv(GL_CULL_FACE_MODE, &oldValue);
		cullFaceMode = oldValue;
	}

// depth test
	glGetBooleanv(GL_DEPTH_TEST, &oldBool);
	if (!oldBool) {
		depthTest		= false;
		depthTestFunc	= GL_LESS;
		depthWriteMask	= true;
	} else {
		depthTest = true;

		glGetIntegerv(GL_DEPTH_FUNC, &oldValue);
		depthTestFunc = oldValue;

		glGetBooleanv(GL_DEPTH_WRITEMASK, &oldBool);
		depthWriteMask = static_cast<bool>(oldBool);
	}

// texture mapping
	glGetBooleanv(GL_TEXTURE_2D, &oldBool);
	textureMapping2d = static_cast<bool>(oldBool);
}


void NeHeOGL_Mesh__RenderState::create(RenderState& rs) {
	wireframe			= rs.wireframe;
	cullFace			= rs.cull != Cull::None;
	cullFaceMode		= OGLUtil::getGlCullMode(rs.cull);
	depthTest			= rs.depthTest.isEnable();
	depthTestFunc		= OGLUtil::getGlDepthTestOp(rs.depthTest.op);
	depthWriteMask		= rs.depthTest.writeMask;
	textureMapping2d	= true;
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
}

void NeHeOGL_Mesh::clear() {
	vertices.clear();
	indices.clear();
}

void NeHeOGL_Mesh::createRect(float w, float h) {
	clear();

	float x = w / 2;
	float y = h / 2;

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

	float x = w / 2;
	float y = h / 2;
	float z = d / 2;

	{
		vertices.resize(8);
		auto* p = vertices.data();

		p->pos.set(-x,-y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p ++;
		p->pos.set( x,-y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p ++;
		p->pos.set( x,-y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p ++;
		p->pos.set(-x,-y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p ++;
														  				
		p->pos.set(-x, y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 1); p ++;
		p->pos.set( x, y,-z); p->color.set(OGL::kbWhite); p->uv.set(1, 0); p ++;
		p->pos.set( x, y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 0); p ++;
		p->pos.set(-x, y, z); p->color.set(OGL::kbWhite); p->uv.set(0, 1); p ++;
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

void NeHeOGL_Mesh::_beginDraw() {
	_lastRenderState = MyRenderState();
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

	static constexpr GLenum kIndiceDataType = OGLUtil::getGlFormat(
		NeHe_RenderDataTypeUtil::get<VertexIndiceDataType>()
	);

	glDrawElements(	GL_TRIANGLES,
					static_cast<GLsizei>(indices.size()),
					kIndiceDataType,
					indices.data());

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
