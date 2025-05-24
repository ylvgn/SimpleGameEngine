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

	void onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd);
	void onCmd_SwapBuffers		(RenderCommand_SwapBuffers& cmd);
	void onCmd_DrawCall			(RenderCommand_DrawCall& cmd);
	void onCmd_SetScissorRect	(RenderCommand_SetScissorRect& cmd);
//	void onCmd_SetViewport		(RenderCommand_SetViewport& cmd); no use now

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

	void _destroyTestShaders();

	void _setTestShaders(const VertexLayout* vertexLayout);

	HDC		_win32_dc = nullptr;
	HGLRC	_win32_rc = nullptr;

	GLuint	_testShaderProgram	= 0;
	GLuint	_testVertexShader	= 0;
	GLuint	_testPixelShader	= 0;
}; // RenderContext_GL_Win32

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL
#endif // SGE_OS_WINDOWS