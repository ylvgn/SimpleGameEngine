#pragma once

#include "../../RenderContext.h"
#include "Render_GL3_Common.h"
#include "Shader_GL3.h"

namespace sge {

#if 0
#pragma mark ========= MyVertexArray ============
#endif
class MyVertexArray : public NonCopyable {
public:
	~MyVertexArray() { destroy(); }

	void create() {
		if (!_p) {
			glGenVertexArrays(1, &_p);
		}
	}

	void destroy() {
		if (_p) {
			glDeleteVertexArrays(1, &_p);
			_p = 0;
		}
	}

	void bind()		const { glBindVertexArray(_p); }
	void unbind()	const { glBindVertexArray(0); }
private:
	GLuint _p = 0;
};

#if 0
#pragma mark ========= MyVertexBuffer ============
#endif
class MyVertexBuffer : public NonCopyable {
public:
	template<class MyVertexT>
	void create(const Span<const MyVertexT> data) {
		destroy();

		if (data.empty())
			return;

		glGenBuffers(1, &_p);
		glBindBuffer(GL_ARRAY_BUFFER, _p);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size_bytes()), data.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void destroy() {
		if (_p) {
			glDeleteBuffers(1, &_p);
			_p = 0;
		}
	}

	void bind	() const { glBindBuffer(GL_ARRAY_BUFFER, _p); }
	void unbind	() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
private:
	GLuint _p = 0;
};

#if 0
#pragma mark ========= MyIndexBuffer ============
#endif
class MyIndexBuffer : public NonCopyable {
public:
	using IndexType = u16;

	void create(const Span<const IndexType> data) {
		destroy();
		if (data.empty())
			return;

		glGenBuffers(1, &_p);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _p);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.size_bytes()), data.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void destroy() {
		if (_p) {
			glDeleteBuffers(1, &_p);
			_p = 0;
		}
	}

	void bind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _p); }
	void unbind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
private:
	GLuint _p = 0;
};

#if 0
#pragma mark ========= Scoped_RenderContext_GL3_VertexAttrib ============
#endif
class Scoped_RenderContext_GL3_VertexAttrib : public NonCopyable {
public:
	Scoped_RenderContext_GL3_VertexAttrib(GLuint program, StrView name, GLint numComponets, GLenum type, bool normalized, GLsizei stride, intptr_t vertexBufferOffset) {
		_loc = glGetAttribLocation(program, name.data());
		if (_loc < 0)
			return;
		glVertexAttribPointer(_loc, numComponets, type, normalized, stride, reinterpret_cast<const void*>(vertexBufferOffset));
		glEnableVertexAttribArray(_loc);
	}

	~Scoped_RenderContext_GL3_VertexAttrib() {
		if (_loc < 0)
			return;
		glDisableVertexAttribArray(_loc);
	}
private:
	GLint _loc = -1;
};

#if 0
#pragma mark ========= RenderContext_GL3 ============
#endif
class RenderContext_GL3 : public RenderContext {
private:
	using Base = RenderContext;
	using Util = GL3Util;

public:
	RenderContext_GL3(CreateDesc& desc);

	void testRender() override;

protected:
	Renderer_GL3* _renderer = nullptr;

	virtual void onBeginRender() override;
	virtual void onEndRender() override;

	virtual void onSetFrameBufferSize(const Vec2f& newSize) override;

	void onTestDraw();
	void onClearColorAndDepthBuffer();
	void onSwapBuffers();
private:

#if SGE_OS_WINDOWS
	::HDC	_win32_dc = nullptr;
	::HGLRC	_win32_rc = nullptr;
#endif

// just for test
	SPtr<Shader_GL3> _testShader;
	MyVertexArray _vertexArray;
};

}