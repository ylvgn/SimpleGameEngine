#include "Render_GL_Common.h"

namespace sge {
#if 0
#pragma mark ========= GLVertexArray ============
#endif
void GLVertexArray::create() {
	destroy();
	glGenVertexArrays(1, &_gl);
}

void GLVertexArray::destroy() {
	if (_gl) {
		glDeleteVertexArrays(1, &_gl);
		_gl = 0;
	}
}

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

#if 0
#pragma mark ========= GLFrameBuffer ============
#endif
void GLFrameBuffer::create() {
	destroy();

	glGenFramebuffers(1, &_gl);
	glBindFramebuffer(GL_FRAMEBUFFER, _gl);
}

void GLFrameBuffer::destroy() {
	if (_gl) {
		glDeleteFramebuffers(1, &_gl);
		_gl = 0;
	}
}

#if 0
#pragma mark ========= GLRenderBuffer ============
#endif
void GLRenderBuffer::create(GLsizei width, GLsizei height) {
	destroy();

	glGenRenderbuffers(1, &_gl);
	glBindRenderbuffer(GL_RENDERBUFFER, _gl);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _gl);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw SGE_ERROR("Framebuffer is not complete");
}

void GLRenderBuffer::destroy() {
	if (_gl) {
		glDeleteRenderbuffers(1, &_gl);
		_gl = 0;
	}
}

}