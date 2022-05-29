#pragma once

#include <sge_render/RenderContext.h>
#include <sge_core/pointer/ComPtr.h>

#include "Render_DX11_Common.h"

namespace sge {
	class RenderContext_DX11 : public RenderContext {

	private:
		using Base = RenderContext;
		using Util = DX11Util;

	public:
		RenderContext_DX11(CreateDesc& desc);

	protected:
		Renderer_DX11* _renderer = nullptr;
		void _createRenderTarget();

		virtual void onBeginRender() override;
		virtual void onEndRender() override;

		virtual void onDraw() override;

		virtual void onSwapBuffers() override;
		virtual void onClearColorAndDepthBuffer() override;

		ComPtr<DX11_IDXGISwapChain>		  _swapChain;
		ComPtr<DX11_ID3DRenderTargetView> _renderTargetView; // back buffer(color buffer)
		ComPtr<DX11_ID3DDepthStencilView> _depthStencilView; // depth buffer
		ComPtr<DX11_ID3DTexture2D>        _depthStencil;

		ComPtr<DX11_ID3DBuffer> _testVertexBuffer;
		ComPtr<DX11_ID3DVertexShader> _testVertexShader;
		ComPtr<DX11_ID3DPixelShader> _testPixelShader;
		ComPtr<DX11_ID3DInputLayout> _testInputLayout;
	};

} // namespace