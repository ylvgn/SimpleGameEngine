#include "NeHeOGL_Mesh.h"
#include "NeHeOGL_GL_Common.h"

namespace sge {

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

	indices.resize(6 * 6);
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
	using Cull = NeHeOGL_RenderState::Cull;
	using DepthTestOp = NeHeOGL_RenderState::DepthTestOp;

	{
		int oldValue;
		GLboolean oldBool;

		int oldValue2[2];

		glGetIntegerv(GL_POLYGON_MODE, oldValue2);
		//GLenum polygonFace = oldValue2[0];
		int polygonMode = oldValue2[1];
		_lastRenderState.wireframe = polygonMode != GL_FILL;

		glGetBooleanv(GL_CULL_FACE, &oldBool);
		if (!oldBool) {
			_lastRenderState.cullFace = false;
			_lastRenderState.cullFaceMode = GL_FRONT_AND_BACK;
		} else {
			_lastRenderState.cullFace = true;

			glGetIntegerv(GL_CULL_FACE_MODE, &oldValue);
			_lastRenderState.cullFaceMode = oldValue;
		}

		glGetBooleanv(GL_DEPTH_TEST, &oldBool);
		if (!oldBool) {
			_lastRenderState.depthTest = false;
			_lastRenderState.depthTestFunc = GL_LESS;
			_lastRenderState.depthWriteMask = true;
		} else {
			_lastRenderState.depthTest = true;

			glGetIntegerv(GL_DEPTH_FUNC, &oldValue);
			_lastRenderState.depthTestFunc = oldValue;

			glGetBooleanv(GL_DEPTH_WRITEMASK, &oldBool);
			_lastRenderState.depthWriteMask = static_cast<bool>(oldBool);
		}
	}

	_curRenderState.wireframe = renderState.wireframe;
	_curRenderState.cullFace = renderState.cull != Cull::None;
	_curRenderState.cullFaceMode = OGLUtil::getGlCullMode(renderState.cull);
	_curRenderState.depthTest = renderState.depthTest.isEnable();
	_curRenderState.depthTestFunc = OGLUtil::getGlDepthTestOp(renderState.depthTest.op);
	_curRenderState.depthWriteMask = renderState.depthTest.writeMask;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}

void NeHeOGL_Mesh::draw() {
	if (indices.empty())
		return;

	_beginDraw();

	_bindVertexs();
	_bindRenderState(_curRenderState);

	static constexpr GLenum kIndiceDataType = OGLUtil::getGlFormat(
		NeHe_RenderDataTypeUtil::get<VertexIndiceDataType>()
	);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), kIndiceDataType, indices.data());

	_endDraw();
}

void NeHeOGL_Mesh::_endDraw() {
	_bindRenderState(_lastRenderState);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void NeHeOGL_Mesh::_bindVertexs() {
	int stride = sizeof(vertices[0]);

	OGL::vertexPointer(&vertices[0].pos, stride);
	OGL::colorPointer(&vertices[0].color, stride);

	//glVertexPointer(3, GL_FLOAT, stride, vertices[0].pos.data);
	//glColorPointer(4, GL_UNSIGNED_BYTE, stride, vertices[0].color.data);
}

void NeHeOGL_Mesh::_bindRenderState(MyRenderState& rs) {
	if (rs.wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (rs.cullFace) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
	glCullFace(rs.cullFaceMode);

	if (rs.depthTest) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	glDepthFunc(rs.depthTestFunc);
	glDepthMask(rs.depthWriteMask ? GL_TRUE : GL_FALSE);
}

}
