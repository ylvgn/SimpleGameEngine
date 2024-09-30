#include "Render_GL_Common.h"

namespace sge {
#if 0
#pragma mark ========= GLVertexBuffer ============
#endif
template<class T>
void GLVertexBuffer::create(Span<T> data) {
	destroy();

	if (data.empty())
		return;

	glGenBuffers(1, &_gl);
	glBindBuffer(GL_ARRAY_BUFFER, _gl);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size_bytes()), data.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLVertexBuffer::destroy() {
	if (_gl) {
		glDeleteBuffers(1, &_gl);
		_gl = 0;
	}
}

#if 0
#pragma mark ========= GLIndexBuffer ============
#endif
void GLIndexBuffer::create(const Span<const u16> data) {
	destroy();
	if (data.empty())
		return;

	glGenBuffers(1, &_gl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gl);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size_bytes()), data.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLIndexBuffer::create(const Span<const u32> data) {
	destroy();
	if (data.empty())
		return;

	glGenBuffers(1, &_gl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _gl);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size_bytes()), data.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLIndexBuffer::destroy() {
	if (_gl) {
		glDeleteBuffers(1, &_gl);
		_gl = 0;
	}
}

}