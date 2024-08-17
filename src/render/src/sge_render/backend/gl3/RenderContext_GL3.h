#pragma once

#include "../../RenderContext.h"
#include "Render_GL3_Common.h"

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

	void _destroy();
	void _destroyTestShaders();

	void _setTestShaders(const VertexLayout* vertexLayout);

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