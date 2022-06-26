#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"
#include "RenderGpuBuffer_DX11.h"

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
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = 8;								// set the back buffer width
		swapChainDesc.BufferDesc.Height = 8;							// set the back buffer height
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// use 32-bit color
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.SampleDesc.Count = 1;								// how many multisamples
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferCount = 1;									// one back buffer
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// how swap chain is to be used
		swapChainDesc.OutputWindow = hWnd;								// the window to be used
		swapChainDesc.Windowed = TRUE;									// windowed/full-screen mode
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

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

	// set-up renger target
	DX11_ID3DRenderTargetView* rt = _renderTargetView;
	ctx->OMSetRenderTargets(1, &rt, nullptr);
}

void RenderContext_DX11::onEndRender() {

}

#if 0
// test -> render a single frame
void RenderContext_DX11::onDraw() {
	auto* d3dDevice = _renderer->d3dDevice();
	auto* d3dDeviceContext = _renderer->d3dDeviceContext();
	HRESULT hr;

	struct VERTEX {
		float x, y, z;
		float color[4];
	};

#define SomthingWrong 0

	// vertex layout
	static VERTEX vertexData[] =
	{
#if SomthingWrong
		{0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f} },
		{0.5f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f} },
		{-0.5f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f} },

#else
		{0.0f, 0.5f, 0.0f, {255.0f, 0.0f, 0.0f, 255.0f} },
		{0.5f, -0.5, 0.0f, {0.0f, 255.0f, 0.0f, 255.0f} },
		{-0.5f, -0.5f, 0.0f, {0.0f, 0.0f, 255.0f, 255.0f} },
#endif
	};

	UINT vertexCount = sizeof(vertexData) / sizeof(vertexData[0]);

	if (!_testVertexBuffer) {
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX) * vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		hr = d3dDevice->CreateBuffer(&bd, NULL, _testVertexBuffer.ptrForInit());
		Util::throwIfError(hr);

		// map && unmap buffer
		D3D11_MAPPED_SUBRESOURCE ms;
		hr = d3dDeviceContext->Map(_testVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		Util::throwIfError(hr);
		memcpy(ms.pData, vertexData, sizeof(vertexData));
		d3dDeviceContext->Unmap(_testVertexBuffer, NULL);
	}

	{ // shader
		if (!_testVertexShader || !_testPixelShader) {
			LPCWSTR shaderName = L"Assets/Shaders/test.hlsl";

			DX11_ID3DBlob *errorMsg;
			DX11_ID3DBlob *VS, *PS; // bytecode

			hr = D3DCompileFromFile(shaderName, 0, 0, "VShader", "vs_4_0", 0, 0, &VS, &errorMsg);
			Util::throwIfError(hr);
			hr = D3DCompileFromFile(shaderName, 0, 0, "PShader", "ps_4_0", 0, 0, &PS, &errorMsg);
			Util::throwIfError(hr);

			hr = d3dDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, _testVertexShader.ptrForInit());
			Util::throwIfError(hr);
			hr = d3dDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, _testPixelShader.ptrForInit());
			Util::throwIfError(hr);

			// input vertex layout
			{
				D3D11_INPUT_ELEMENT_DESC ied[] =
				{
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
#if SomthingWrong
					// DXGI_FORMAT_R32G32B32A32_FLOAT: https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
					{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
#else
					{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
#endif
				};

				hr = d3dDevice->CreateInputLayout(ied, sizeof(ied) / sizeof(ied[0]), VS->GetBufferPointer(), VS->GetBufferSize(), _testInputLayout.ptrForInit());
				Util::throwIfError(hr);
			}
		}
	}

	{ // draw
		// set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = 800;
		viewport.Height = 600;
		d3dDeviceContext->RSSetViewports(1, &viewport);

		// input layout
		d3dDeviceContext->IASetInputLayout(_testInputLayout);

		// triangle primitive
		d3dDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		ID3D11Buffer* pVBuffer = _testVertexBuffer;
		d3dDeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

		// shader
		d3dDeviceContext->VSSetShader(_testVertexShader, 0, 0);
		d3dDeviceContext->PSSetShader(_testPixelShader, 0, 0);

		d3dDeviceContext->Draw(vertexCount, 0);
	}
}
#endif

void RenderContext_DX11::onCommit(RenderCommandBuffer& cmdBuf) {
	_dispatch(this, cmdBuf);
}

void RenderContext_DX11::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
	auto* ctx = _renderer->d3dDeviceContext();

	// clear back buffer(color buffer)
	if (_renderTargetView && cmd.color.has_value()) {
		ctx->ClearRenderTargetView(_renderTargetView, cmd.color->data);
	}

	// clear depth buffer
	if (_depthStencilView && cmd.depth.has_value()) {
		ctx->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, *cmd.depth, 0);
	}
}

void RenderContext_DX11::onCmd_DrawCall(RenderCommand_DrawCall& cmd) {
	if (!cmd.vertexLayout) { SGE_ASSERT(false); return; }

	auto* vertexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.vertexBuffer.ptr());
	if (!vertexBuffer) { SGE_ASSERT(false); return; }

	if (cmd.vertexCount <= 0) { SGE_ASSERT(false); return; }
	if (cmd.primitive == RenderPrimitiveType::None) { SGE_ASSERT(false); return; }

	RenderGpuBuffer_DX11* indexBuffer = nullptr;
	if (cmd.indexCount > 0) {
		indexBuffer = static_cast<RenderGpuBuffer_DX11*>(cmd.indexBuffer.ptr());
		if (!indexBuffer) { SGE_ASSERT(false); return; }
	}

	_setTestShaders(cmd.vertexLayout);

	auto* ctx = _renderer->d3dDeviceContext();
	auto primitive = Util::getDxPrimitiveTopology(cmd.primitive);
	ctx->IASetPrimitiveTopology(primitive);

	UINT stride = static_cast<UINT>(cmd.vertexLayout->stride);
	UINT offset = 0;
	UINT vertexCount = static_cast<UINT>(cmd.vertexCount);
	UINT indexCount = static_cast<UINT>(cmd.indexCount);

	DX11_ID3DBuffer* ppVertexBuffers[] = { vertexBuffer->d3dBuf() };
	ctx->IASetVertexBuffers(0, 1, ppVertexBuffers, &stride, &offset);

	if (indexCount > 0) {
		auto indexType = Util::getDxFormat(cmd.indexType);
		ctx->IASetIndexBuffer(indexBuffer->d3dBuf(), indexType, 0);
		ctx->DrawIndexed(indexCount, 0, 0);
	}
	else {
		ctx->Draw(vertexCount, 0);
	}
}

void RenderContext_DX11::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd) {
	HRESULT hr = _swapChain->Present(0, 0);
	Util::throwIfError(hr);
}

void RenderContext_DX11::_createRenderTarget() {
	auto* renderer = Renderer_DX11::instance();
	auto* dev = renderer->d3dDevice();
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer;
	hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ptrForInit())); // IID_PPV_ARGS -> __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)
	Util::throwIfError(hr);

	// use the back buffer address to create the render target
	hr = dev->CreateRenderTargetView(backBuffer, NULL, _renderTargetView.ptrForInit());
	Util::throwIfError(hr);

#if 0
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = backBufferDesc.Width;
	descDepth.Height = backBufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = dev->CreateTexture2D(&descDepth, nullptr, _depthStencil.ptrForInit());
	Util::throwIfError(hr);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = dev->CreateDepthStencilView(_depthStencil.ptr(), &descDSV, _depthStencilView.ptrForInit());
	Util::throwIfError(hr);
#endif
}

void RenderContext_DX11::onSetFrameBufferSize(Vec2f newSize) {
	_renderTargetView.reset(nullptr); // release buffer and render target view before resize

	auto hr = _swapChain->ResizeBuffers(0
		, static_cast<UINT>(Math::max(0.0f, newSize.x))
		, static_cast<UINT>(Math::max(0.0f, newSize.y))
		, DXGI_FORMAT_UNKNOWN
		, 0);
	Util::throwIfError(hr);
}

void RenderContext_DX11::_setTestShaders(const VertexLayout* vertexLayout) {
	HRESULT hr;
	const wchar_t* shaderFile = L"Assets/Shaders/test.hlsl";

	auto* dev = _renderer->d3dDevice();
	auto* ctx = _renderer->d3dDeviceContext();

	if (!_testVertexShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "vs_main", "vs_4_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
		Util::throwIfError(hr);

		_testVertexShaderBytecode = bytecode;

		hr = dev->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, _testVertexShader.ptrForInit());
		Util::throwIfError(hr);
	}

	if (!_testPixelShader) {
		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;
		hr = D3DCompileFromFile(shaderFile, 0, 0, "ps_main", "ps_4_0", 0, 0, bytecode.ptrForInit(), errorMsg.ptrForInit());
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

	Vector_<D3D11_INPUT_ELEMENT_DESC, 32> inputDesc;

	for (auto& e : src->elements) {
		auto& dst = inputDesc.emplace_back();
		auto semanticType = VertexSemanticUtil::getType(e.semantic);
		dst.SemanticName = enumStr(semanticType); //Util::getDxSemanticName(semanticType);
		dst.SemanticIndex = VertexSemanticUtil::getIndex(e.semantic);
		dst.Format = Util::getDxFormat(e.dataType);
		dst.InputSlot = 0;
		dst.AlignedByteOffset = e.offset;
		dst.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		dst.InstanceDataStepRate = 0;
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

} // namespace