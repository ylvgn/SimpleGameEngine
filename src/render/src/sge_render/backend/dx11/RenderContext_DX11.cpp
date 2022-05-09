#include "Renderer_DX11.h"
#include "RenderContext_DX11.h"

namespace sge {

	RenderContext_DX11::RenderContext_DX11(CreateDesc& desc)
		: Base(desc)
	{
		_renderer = Renderer_DX11::current();

		auto* dxgiFactory = _renderer->dxgiFactory();
		auto* d3dDevice = _renderer->d3dDevice();

		auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);
		auto* hWnd = win->_hwnd;

		DXGI_SWAP_CHAIN_DESC scd; // swapChainDesc
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferDesc.Width = 8;								// set the back buffer width
		scd.BufferDesc.Height = 8;								// set the back buffer height
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = hWnd;                                // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

		auto hr = dxgiFactory->CreateSwapChain(d3dDevice, &scd, _swapChain.ptrForInit());
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::_createRenderTarget() {
		auto* d3dDevice = _renderer->d3dDevice();

		HRESULT hr;

		hr = _swapChain->ResizeBuffers(0, 800, 600, DXGI_FORMAT_UNKNOWN, 0); // tmp
		Util::throwIfError(hr);

		ComPtr<ID3D11Texture2D> backBuffer;
		hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ptrForInit())); // IID_PPV_ARGS -> __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)
		Util::throwIfError(hr);

		// use the back buffer address to create the render target
		hr = d3dDevice->CreateRenderTargetView(backBuffer, NULL, _renderTargetView.ptrForInit());
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onBeginRender() {
		auto* d3dDeviceContext = _renderer->d3dDeviceContext();

		// clear render target
		if (!_renderTargetView) {
			_createRenderTarget();
		}

		// set-up renger target
		DX11_ID3DRenderTargetView* rt = _renderTargetView;
		d3dDeviceContext->OMSetRenderTargets(1, &rt, nullptr);
	}

	void RenderContext_DX11::onClearColorAndDepthBuffer() {
		auto* renderer = Renderer_DX11::current();
		auto* d3dDeviceContext = _renderer->d3dDeviceContext();

		// clear back buffer(color buffer)
		if (_renderTargetView) {
			float color[4] = { 0, 0, 0.5f, 1 };
			d3dDeviceContext->ClearRenderTargetView(_renderTargetView, color);
		}

		// clear depth buffer
		if (_depthStencilView) {
			float depth = 0;
			d3dDeviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, depth, 0);
		}
	}

	// test -> render a single frame
	void RenderContext_DX11::onDraw() {
		auto* d3dDevice = _renderer->d3dDevice();
		auto* d3dDeviceContext = _renderer->d3dDeviceContext();
		HRESULT hr;

		struct VERTEX {
			float x, y, z;
			u8 color[4];
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

				ID3D10Blob* errorMsg;
				ID3D10Blob *VS, *PS; // bytecode

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

	void RenderContext_DX11::onSwapBuffers() {
		HRESULT hr;
		hr = _swapChain->Present(0, 0);
		Util::throwIfError(hr);
	}

	void RenderContext_DX11::onEndRender() {

	}

} // namespace