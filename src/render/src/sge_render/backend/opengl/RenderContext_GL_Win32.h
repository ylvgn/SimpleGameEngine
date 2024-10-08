#pragma once

#if SGE_OS_WINDOWS
#if SGE_RENDER_HAS_OPENGL

#include "RenderContext_GL_Base.h"

namespace sge {

class RenderContext_GL_Win32 : public RenderContext_GL_Base {
	using Base = RenderContext_GL_Base;
public:
	RenderContext_GL_Win32(CreateDesc& desc);
	~RenderContext_GL_Win32() { _destroy(); }

	void onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers&	cmd);
	void onCmd_SwapBuffers		(RenderCommand_SwapBuffers&			cmd);
	void onCmd_DrawCall			(RenderCommand_DrawCall&			cmd);

	void onCmd_SetScissorRect	(RenderCommand_SetScissorRect& cmd) { /*TODO*/ }

protected:
	Renderer_GL* _renderer = nullptr;

	virtual void onBeginRender() override;
	virtual void onEndRender() override;

	virtual void onSetFrameBufferSize(const Vec2f& newSize) override;
	virtual void onCommit(RenderCommandBuffer& cmdBuf);

private:
	class FalseContext;

	void _createBuffers();

	void _destroy();
	void _destroyBuffers();
	void _destroyTestScreenFrameBuffer();
	void _destroyTestShaders();

	void _setTestShaders(const VertexLayout* vertexLayout);
	void _setTestFrameBufferScreenShaders();

	::HDC	_win32_dc = nullptr;
	::HGLRC	_win32_rc = nullptr;

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

} // namespace sge

#endif // SGE_OS_WINDOWS
#endif // SGE_RENDER_HAS_OPENGL