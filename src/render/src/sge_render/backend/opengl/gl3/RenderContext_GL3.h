#pragma once

#include <sge_render/RenderContext.h>
#include "Render_GL3_Common.h"

#if SGE_RENDER_HAS_GL3

namespace sge {

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

private:
	class GLVertexArray : public GLObject {
	public:
		~GLVertexArray() { _destroy(); }
		void bind();
	private:
		void _destroy();
	};

	void _createBuffers();

	void _destroy();
	void _destroyBuffers();
	void _destroyTestScreenFrameBuffer();
	void _destroyTestShaders();

	void _setTestShaders(const VertexLayout* vertexLayout);
	void _setTestFrameBufferScreenShaders();

#if SGE_OS_WINDOWS
	::HDC	_win32_dc = nullptr;
	::HGLRC	_win32_rc = nullptr;
#endif

	GLVertexArray	_vao;

    GLuint _viewFramebuffer		= 0;
	GLuint _viewRenderbuffer	= 0;
    GLuint _depthRenderbuffer	= 0;

	GLuint _testShaderProgram	= 0;
	GLuint _testVertexShader	= 0;
	GLuint _testPixelShader		= 0;

	GLuint _testFrameBufferShaderProgram	= 0;
	GLuint _testFrameBufferVertexShader		= 0;
	GLuint _testFrameBufferPixelShader		= 0;

	GLuint _testScreenQuadVertexbuffer		= 0;
	GLuint _testScreenQuadTexturebuffer		= 0;
	GLuint _testScreenQuadRenderbuffer		= 0;
};

}
#endif // SGE_RENDER_HAS_GL3