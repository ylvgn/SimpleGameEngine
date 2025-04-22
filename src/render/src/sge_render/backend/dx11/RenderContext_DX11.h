#pragma once

#include <sge_render/RenderContext.h>
#include "Render_DX11_Common.h"

namespace sge {

class RenderContext_DX11 : public RenderContext {
private:
	using Base = RenderContext;
	using Util = DX11Util;
public:
	RenderContext_DX11(CreateDesc& desc);

	void onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd);
	void onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd);
	void onCmd_DrawCall(RenderCommand_DrawCall& cmd);
	void onCmd_SetScissorRect(RenderCommand_SetScissorRect& cmd);
//	void onCmd_SetViewport(RenderCommand_SetViewport& cmd); no use now

	Renderer_DX11* renderer() { return _renderer; }

protected:
	Renderer_DX11* _renderer = nullptr;
	void _createRenderTarget();

	virtual void onCommit(RenderCommandBuffer& cmdBuf) override;
	virtual void onBeginRender() override;
	virtual void onEndRender() override;
	virtual void onSetFrameBufferSize(const Vec2f& newSize) override;

	ComPtr<DX11_IDXGISwapChain>			_swapChain;
	ComPtr<DX11_ID3DRenderTargetView>	_renderTargetView;
	ComPtr<DX11_ID3DDepthStencilView>	_depthStencilView;
	ComPtr<DX11_ID3DTexture2D>			_depthStencil;

	ComPtr<DX11_ID3DBuffer>				_testVertexBuffer;
	ComPtr<DX11_ID3DVertexShader>		_testVertexShader;
	ComPtr<DX11_ID3DPixelShader>		_testPixelShader;
	ComPtr<DX11_ID3DInputLayout>		_testInputLayout;
	ComPtr<DX11_ID3DBlob>				_testVertexShaderBytecode;

	ComPtr<DX11_ID3DRasterizerState>	_testRasterizerState;
	ComPtr<DX11_ID3DDepthStencilState>	_testDepthStencilState;
	ComPtr<DX11_ID3DBlendState>			_testBlendState;

	DX11_ID3DInputLayout* _getTestInputLayout(const VertexLayout* src);
	VectorMap<const VertexLayout*, ComPtr<DX11_ID3DInputLayout>> _testInputLayouts;

	void _setTestShaders(const VertexLayout* vertexLayout);
	void _setTestDefaultRenderState();
}; // RenderContext_DX11

} // namespace