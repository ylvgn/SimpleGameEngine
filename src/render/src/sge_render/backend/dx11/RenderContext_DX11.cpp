#include "RenderContext_DX11.h"
#include "Renderer_DX11.h"
#include "RenderGpuBuffer_DX11.h"
#include "Material_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

RenderContext_DX11::RenderContext_DX11(CreateDesc& desc)
	: Base(desc)
{
	_renderer = Renderer_DX11::instance();

	auto* dxgiFactory = _renderer->dxgiFactory();
	auto* dev = _renderer->d3dDevice();
	auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);
	auto* hWnd = win->_hwnd;

	{
		::DXGI_SWAP_CHAIN_DESC swapChainDesc				= {};
		swapChainDesc.BufferDesc.Width						= 8;								// set the back buffer width
		swapChainDesc.BufferDesc.Height						= 8;								// set the back buffer height
		swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;		// use 32-bit color
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
		swapChainDesc.SampleDesc.Count						= 1;								// how many multisamples
		swapChainDesc.SampleDesc.Quality					= 0;
		swapChainDesc.BufferCount							= 1;								// one back buffer
		swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;	// how swap chain is to be used
		swapChainDesc.OutputWindow							= hWnd;								// the window to be used
		swapChainDesc.Windowed								= TRUE;								// windowed/full-screen mode
		swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags									= 0;

		auto hr = dxgiFactory->CreateSwapChain(dev, &swapChainDesc, _swapChain.ptrForInit());
		Util::throwIfError(hr);
	}
}

void RenderContext_DX11::onBeginRender() {
	auto* ctx = _renderer->d3dDeviceContext();

	// clear render target
	if (!_renderTargetView) {
		_createRenderTarget();
	}

	// set-up render target
	DX11_ID3DRenderTargetView* rt = _renderTargetView;
	ctx->OMSetRenderTargets(1, &rt, _depthStencilView);

	::D3D11_VIEWPORT viewport	= {};
	viewport.TopLeftX			= 0;
	viewport.TopLeftY			= 0;
	viewport.Width				= _frameBufferSize.x;
	viewport.Height				= _frameBufferSize.y;
	viewport.MinDepth			= 0;
	viewport.MaxDepth			= 1;

	::D3D11_RECT scissorRect	= {};
	scissorRect.left			= 0;
	scissorRect.top				= 0;
	scissorRect.right			= static_cast<LONG>(_frameBufferSize.x);
	scissorRect.bottom			= static_cast<LONG>(_frameBufferSize.y);

	ctx->RSSetViewports(1, &viewport);
	ctx->RSSetScissorRects(1, &scissorRect);
}

void RenderContext_DX11::onEndRender() {
}

void RenderContext_DX11::onCommit(RenderCommandBuffer& cmdBuf) {
	_dispatch(this, cmdBuf);
}

void RenderContext_DX11::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
	auto* ctx = _renderer->d3dDeviceContext();

	// clear back buffer(color buffer)
	if (_renderTargetView && cmd.color.has_value()) {
		ctx->ClearRenderTargetView(_renderTargetView, cmd.color->data);
	}

	// clear depth&stencil buffer
	if (_depthStencilView && (cmd.depth.has_value() || cmd.stencil.has_value())) {
		ctx->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, *cmd.depth, static_cast<UINT8>(*cmd.stencil));
	}
}

void RenderContext_DX11::onCmd_DrawCall(RenderCommand_DrawCall& cmd) {
	auto* ctx = _renderer->d3dDeviceContext();

	if (cmd.isComputeCall()) {
		auto* mtlPass = cmd.getMaterialPass();
		if (!mtlPass) { SGE_ASSERT(false); return; }

		_setTestDefaultRenderState();

		auto scopedBind = mtlPass->scopedBind(*this, cmd);
		auto tc = Vec3<UINT>::s_cast(cmd.threadGroupCount.value());
		ctx->Dispatch(tc.x, tc.y, tc.z);
	}
	else {
		if (!cmd.vertexLayout) { SGE_ASSERT(false); return; }

		auto* vertexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.vertexBuffer.ptr());

		if (!vertexBuffer && cmd.vertexCount <= 0) { SGE_ASSERT(false); return; }
		if (cmd.primitive == RenderPrimitiveType::None) { SGE_ASSERT(false); return; }

		RenderGpuBuffer_DX11* indexBuffer = nullptr;
		if (cmd.indexCount > 0) {
			indexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.indexBuffer.ptr());
			if (!indexBuffer) { SGE_ASSERT(false); return; }
		}

		_setTestDefaultRenderState();

		if (auto* pass = cmd.getMaterialPass()) {
			pass->bind(*this, cmd);
		}
		else {
			_setTestShaders(cmd.vertexLayout);
		}

		auto primitive = Util::getDxPrimitiveTopology(cmd.primitive);
		ctx->IASetPrimitiveTopology(primitive);

		UINT stride			= static_cast<UINT>(cmd.vertexLayout->stride);
		UINT vertexOffset	= static_cast<UINT>(cmd.vertexOffset);
		UINT indexOffset	= static_cast<UINT>(cmd.indexOffset);
		UINT vertexCount	= static_cast<UINT>(cmd.vertexCount);
		UINT indexCount		= static_cast<UINT>(cmd.indexCount);

		DX11_ID3DBuffer* ppVertexBuffers[] = { vertexBuffer ? vertexBuffer->d3dBuf() : nullptr };
		ctx->IASetVertexBuffers(0, 1, ppVertexBuffers, &stride, &vertexOffset);

//		_renderer->validateContext();

		if (indexCount > 0) {
			auto indexType = Util::getDxFormat(cmd.indexType);
			ctx->IASetIndexBuffer(indexBuffer->d3dBuf(), indexType, indexOffset);
			ctx->DrawIndexed(indexCount, 0, 0);
		}
		else {
			ctx->Draw(vertexCount, 0);
		}

		// test
		if (vertexBuffer) {
			_statistics.drawCall++;
			_statistics.verts += vertexBuffer->bufferSize() / stride;
		}
	}
}

void RenderContext_DX11::onCmd_SetScissorRect(RenderCommand_SetScissorRect& cmd) {
	auto* ctx = _renderer->d3dDeviceContext();
	::D3D11_RECT rc;
	Util::convert(rc, cmd.rect);
	ctx->RSSetScissorRects(1, &rc);
}

void RenderContext_DX11::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd) {
	HRESULT hr = _swapChain->Present(_renderer->vsync() ? 1 : 0, 0);
	Util::throwIfError(hr);
}

void RenderContext_DX11::_createRenderTarget() {
	auto* renderer = Renderer_DX11::instance();
	auto* dev = renderer->d3dDevice();
	HRESULT hr;

	ComPtr<DX11_ID3DTexture2D> backBuffer;
	hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ptrForInit()));
	Util::throwIfError(hr);

	// use the back buffer address to create the render target
	hr = dev->CreateRenderTargetView(backBuffer, NULL, _renderTargetView.ptrForInit());
	Util::throwIfError(hr);

	// create depth stencil texture
	::D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	::D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width					 = backBufferDesc.Width;
	descDepth.Height				 = backBufferDesc.Height;
	descDepth.MipLevels				 = 1;
	descDepth.ArraySize				 = 1;
	descDepth.Format				 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count		 = 1;
	descDepth.SampleDesc.Quality	 = 0;
	descDepth.Usage					 = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags				 = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags		 = 0;
	descDepth.MiscFlags				 = 0;
	hr = dev->CreateTexture2D(&descDepth, nullptr, _depthStencil.ptrForInit());
	Util::throwIfError(hr);

	// create the depth stencil view
	::D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = dev->CreateDepthStencilView(_depthStencil.ptr(), &descDSV, _depthStencilView.ptrForInit());
	Util::throwIfError(hr);
}

void RenderContext_DX11::onSetFrameBufferSize(const Vec2f& newSize) {
	_renderTargetView.reset(nullptr); // release buffer and render target view before resize
	_depthStencilView.reset(nullptr);

	auto hr = _swapChain->ResizeBuffers(0
									  , static_cast<UINT>(Math::max(0.0f, newSize.x))
									  , static_cast<UINT>(Math::max(0.0f, newSize.y))
									  , DXGI_FORMAT_UNKNOWN
									  , 0);
	Util::throwIfError(hr);
}

void RenderContext_DX11::_setTestDefaultRenderState() {
	auto* dev = _renderer->d3dDevice();
	auto* ctx = _renderer->d3dDeviceContext();

	HRESULT hr;
	if (!_testRasterizerState) {
		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.CullMode				 = D3D11_CULL_BACK;
		rasterDesc.DepthBias			 = 0;
		rasterDesc.DepthBiasClamp		 = 0.0f;
		rasterDesc.DepthClipEnable		 = true;

		bool wireframe = false;
		rasterDesc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;

		rasterDesc.FrontCounterClockwise = true;
		rasterDesc.MultisampleEnable	 = false;
		rasterDesc.ScissorEnable		 = false;
		rasterDesc.SlopeScaledDepthBias  = 0.0f;

		hr = dev->CreateRasterizerState(&rasterDesc, _testRasterizerState.ptrForInit());
		Util::throwIfError(hr);
	}

	if (!_testDepthStencilState) {
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		bool depthTest = true;
		if (depthTest) {
			depthStencilDesc.DepthEnable	= true;
			depthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS_EQUAL;
			depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
		} else {
			depthStencilDesc.DepthEnable	= false;
			depthStencilDesc.DepthFunc		= D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
		}

		depthStencilDesc.StencilEnable		= false;
		depthStencilDesc.StencilReadMask	= 0xFF;
		depthStencilDesc.StencilWriteMask	= 0xFF;

		hr = dev->CreateDepthStencilState(&depthStencilDesc, _testDepthStencilState.ptrForInit());
		Util::throwIfError(hr);
	}

	if (!_testBlendState) {
		D3D11_BLEND_DESC blendStateDesc = {};
		blendStateDesc.AlphaToCoverageEnable  = false;
		blendStateDesc.IndependentBlendEnable = false;
		auto& rtDesc = blendStateDesc.RenderTarget[0];

		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		bool blendEnable = true;
		if (blendEnable) {
			rtDesc.BlendEnable	  = true;
			rtDesc.BlendOp        = D3D11_BLEND_OP_ADD;
			rtDesc.BlendOpAlpha   = D3D11_BLEND_OP_ADD;
			rtDesc.SrcBlend       = D3D11_BLEND_SRC_ALPHA;
			rtDesc.DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
			rtDesc.SrcBlendAlpha  = D3D11_BLEND_SRC_ALPHA;
			rtDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		}else{
			rtDesc.BlendEnable	  = false;
		}

		hr = dev->CreateBlendState(&blendStateDesc, _testBlendState.ptrForInit());
		Util::throwIfError(hr);
	}

	ctx->RSSetState(_testRasterizerState);
	ctx->OMSetDepthStencilState(_testDepthStencilState, 1);

	Color4f blendColor(1,1,1,1);
	ctx->OMSetBlendState(_testBlendState, blendColor.data, 0xffffffff);
}

void RenderContext_DX11::_setTestShaders(const VertexLayout* vertexLayout) {
	HRESULT hr;
	const wchar_t* shaderFile = L"Assets/Shaders/test.hlsl";

	auto* dev = _renderer->d3dDevice();
	auto* ctx = _renderer->d3dDeviceContext();

	if (!_testVertexShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "vs_main", "vs_5_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		_testVertexShaderBytecode = bytecode;

		hr = dev->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _testVertexShader.ptrForInit());
		Util::throwIfError(hr);
	}

	if (!_testPixelShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "ps_main", "ps_5_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		hr = dev->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _testPixelShader.ptrForInit());
		Util::throwIfError(hr);
	}

	ctx->VSSetShader(_testVertexShader, 0, 0);
	ctx->PSSetShader(_testPixelShader, 0, 0);

	auto* inputLayout = _getTestInputLayout(vertexLayout);
	if (!inputLayout) { SGE_ASSERT(false); return; }
	ctx->IASetInputLayout(inputLayout);
}

DX11_ID3DInputLayout* RenderContext_DX11::_getTestInputLayout(const VertexLayout* src) {
	if (!src) return nullptr;

	auto it = _testInputLayouts.find(src);
	if (it != _testInputLayouts.end()) {
		return it->second;
	}

	Vector<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;

	for (auto& e : src->elements) {
		auto& dst					= inputDesc.emplace_back();
		auto semanticType			= VertexSemanticUtil::getType(e.semantic);
		dst.SemanticName			= Util::getDxSemanticName(semanticType);
		dst.SemanticIndex			= VertexSemanticUtil::getIndex(e.semantic);
		dst.Format					= Util::getDxFormat(e.dataType);
		dst.InputSlot				= 0;
		dst.AlignedByteOffset		= e.offset;
		dst.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		dst.InstanceDataStepRate	= 0;
	}

	ComPtr<DX11_ID3DInputLayout>	outLayout;

	auto* dev = _renderer->d3dDevice();
	auto hr = dev->CreateInputLayout(inputDesc.data()
									, static_cast<UINT>(inputDesc.size())
									, _testVertexShaderBytecode->GetBufferPointer()
									, _testVertexShaderBytecode->GetBufferSize()
									, outLayout.ptrForInit());
	Util::throwIfError(hr);

	_testInputLayouts[src] = outLayout;
	return outLayout;
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11