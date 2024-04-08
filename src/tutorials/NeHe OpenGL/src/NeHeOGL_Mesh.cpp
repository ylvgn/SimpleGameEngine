#include "NeHeOGL_Mesh.h"
#include "NeHeOGL_GL_Common.h"

namespace sge {

void NeHeOGL_Mesh::MyRenderState::create() {
	int oldValue;
	int oldValue2[2];
	GLboolean oldBool;

	glGetIntegerv(GL_POLYGON_MODE, oldValue2);
//	GLenum polygonFace = oldValue2[0];
	int polygonMode = oldValue2[1];
	wireframe = polygonMode != GL_FILL;

	glGetBooleanv(GL_CULL_FACE, &oldBool);
	if (!oldBool) {
		cullFace = false;
		cullFaceMode = GL_FRONT_AND_BACK;
	} else {
		cullFace = true;

		glGetIntegerv(GL_CULL_FACE_MODE, &oldValue);
		cullFaceMode = oldValue;
	}

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
}

void NeHeOGL_Mesh::MyRenderState::create(RenderState& rs) {
	wireframe		= rs.wireframe;
	cullFace		= rs.cull != Cull::None;
	cullFaceMode	= OGLUtil::getGlCullMode(rs.cull);
	depthTest		= rs.depthTest.isEnable();
	depthTestFunc	= OGLUtil::getGlDepthTestOp(rs.depthTest.op);
	depthWriteMask	= rs.depthTest.writeMask;
}

void NeHeOGL_Mesh::MyRenderState::bind() {
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (cullFace) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
	glCullFace(cullFaceMode);

	if (depthTest) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	glDepthFunc(depthTestFunc);
	glDepthMask(depthWriteMask ? GL_TRUE : GL_FALSE);
}

void NeHeOGL_Mesh::clear() {
	vertices.clear();
	indices.clear();
}

void NeHeOGL_Mesh::createCube(float w, float h, float d) {
	clear();

	float x = w / 2;
	float y = h / 2;
	float z = d / 2;

	vertices.resize(8);
	vertices[0].pos.set(-x,-y,-z); vertices[0].color.set(OGL::kbWhite);
	vertices[1].pos.set( x,-y,-z); vertices[1].color.set(OGL::kbWhite);
	vertices[2].pos.set( x,-y, z); vertices[2].color.set(OGL::kbWhite);
	vertices[3].pos.set(-x,-y, z); vertices[3].color.set(OGL::kbWhite);

	vertices[4].pos.set(-x, y,-z); vertices[4].color.set(OGL::kbWhite);
	vertices[5].pos.set( x, y,-z); vertices[5].color.set(OGL::kbWhite);
	vertices[6].pos.set( x, y, z); vertices[6].color.set(OGL::kbWhite);
	vertices[7].pos.set(-x, y, z); vertices[7].color.set(OGL::kbWhite);

	indices.assign({
		0, 2, 1, // top
		0, 3, 2,
		3, 6, 2, // front
		3, 7, 6,
		2, 5, 1, // rigt
		2, 6, 5,
		7, 5, 6, // bottom
		7, 4, 5,
		4, 1, 5, // back
		4, 0, 1,
		4, 3, 0, // left
		4, 7, 3,
	});
}

void NeHeOGL_Mesh::_beginDraw() {
	_lastRenderState.create();
	_curRenderState.create(renderState);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
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

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void NeHeOGL_Mesh::_bindVertexs() {
	int stride = sizeof(vertices[0]);

	OGL::vertexPointer(&vertices[0].pos, stride);
	OGL::colorPointer(&vertices[0].color, stride);
}

}
