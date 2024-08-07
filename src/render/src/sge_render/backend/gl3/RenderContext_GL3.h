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
	~RenderContext_GL3() { _destroy(); }

	void onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers&	cmd);
	void onCmd_SwapBuffers		(RenderCommand_SwapBuffers&			cmd);
	void onCmd_DrawCall			(RenderCommand_DrawCall&			cmd);

	void onCmd_SetScissorRect	(RenderCommand_SetScissorRect& cmd) { /*TODO*/ }

protected:
	Renderer_GL3* _renderer = nullptr;

	virtual void onBeginRender() override;
	virtual void onEndRender() override;

	virtual void onSetFrameBufferSize(const Vec2f& newSize) override;
	virtual void onCommit(RenderCommandBuffer& cmdBuf);

	void _destroy();
	void _destroyTestShaders();

	void _setTestShaders();

#if SGE_OS_WINDOWS
	::HDC	_win32_dc = nullptr;
	::HGLRC	_win32_rc = nullptr;
#endif

	GLuint _testShaderProgram		= 0;
	GLuint _testVertexShader		= 0;
	GLuint _testPixelShader			= 0;
	GLuint _testVertexArrayObject	= 0;
};

}