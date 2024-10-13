#pragma once

#include "RenderContext_GL_Base.h"

#if SGE_OS_WINDOWS
#if SGE_RENDER_HAS_OPENGL

namespace sge {

class Renderer_GL;

class RenderContext_GL_Win32 : public RenderContext_GL_Base {
	using Base = RenderContext_GL_Base;
public:
	RenderContext_GL_Win32(CreateDesc& desc);
	~RenderContext_GL_Win32();

	void onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers&	cmd);
	void onCmd_SwapBuffers		(RenderCommand_SwapBuffers&			cmd);
	void onCmd_DrawCall			(RenderCommand_DrawCall&			cmd);
	void onCmd_SetScissorRect	(RenderCommand_SetScissorRect&		cmd) { /*TODO*/ }

protected:
	Renderer_GL*			_renderer	= nullptr;
	NativeUIWindow_Win32*	_window		= nullptr;

	HWND					_hwnd		= nullptr;

	virtual void onBeginRender()	override;
	virtual void onEndRender()		override;

	virtual void onSetFrameBufferSize(const Vec2f& newSize) override;
	virtual void onCommit(RenderCommandBuffer& cmdBuf);

private:
	class FalseContext;
	static const wchar_t* kClassName;

	void _createBuffers();

	void _destroyBuffers();
	void _destroyTestScreenFrameBuffer();
	void _destroyTestShaders();

	void _setTestShaders(const VertexLayout* vertexLayout);
	void _setTestFrameBufferScreenShaders();

	HDC		_win32_dc = nullptr;
	HGLRC	_win32_rc = nullptr;

    GLuint	_viewFramebuffer				= 0;
	GLuint	_viewRenderbuffer				= 0;
    GLuint	_depthRenderbuffer				= 0;

	GLuint	_testShaderProgram				= 0;
	GLuint	_testVertexShader				= 0;
	GLuint	_testPixelShader				= 0;

	GLuint	_testFrameBufferShaderProgram	= 0;
	GLuint	_testFrameBufferVertexShader	= 0;
	GLuint	_testFrameBufferPixelShader		= 0;

	GLuint	_testScreenQuadVertexbuffer		= 0;
	GLuint	_testScreenQuadTexturebuffer	= 0;
	GLuint	_testScreenQuadRenderbuffer		= 0;
}; // RenderContext_GL_Win32

} // namespace sge

#endif // SGE_OS_WINDOWS
#endif // SGE_RENDER_HAS_OPENGL