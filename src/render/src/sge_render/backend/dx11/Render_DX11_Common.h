#pragma once

#if SGE_RENDER_HAS_DX11

#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h> // tmp

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib") // tmp

#include <sge_render/Render_Common.h>

namespace sge {

class Renderer_DX11;

// renderer ---------------
using DX11_IDXGIFactory			= IDXGIFactory1;
using DX11_IDXGIDevice			= IDXGIDevice;
using DX11_IDXGIAdapter			= IDXGIAdapter3;
using DX11_IDXGISwapChain		= IDXGISwapChain;

using DX11_ID3DDevice			= ID3D11Device1;
using DX11_ID3DDeviceContext	= ID3D11DeviceContext4;
using DX11_ID3DDebug			= ID3D11Debug;

// render context ---------------
using DX11_ID3DTexture1D		= ID3D11Texture1D;
using DX11_ID3DTexture2D		= ID3D11Texture2D;
using DX11_ID3DTexture3D		= ID3D11Texture3D;

using DX11_ID3DRenderTargetView	= ID3D11RenderTargetView;
using DX11_ID3DDepthStencilView	= ID3D11DepthStencilView;

using DX11_ID3DBuffer			= ID3D11Buffer;
using DX11_ID3DBlob				= ID3DBlob;

using DX11_ID3DVertexShader		= ID3D11VertexShader;
using DX11_ID3DPixelShader		= ID3D11PixelShader;
using DX11_ID3DInputLayout		= ID3D11InputLayout;

struct DX11Util {
	DX11Util() = delete;

	static void throwIfError(HRESULT hr);
	static bool assertIfError(HRESULT hr);
	static String getStrFromHRESULT(HRESULT hr);
	static void reportError(HRESULT hr);

private:
	static bool _checkError(HRESULT hr) {
		return FAILED(hr); // if got error, return true
	}
};

inline
void DX11Util::throwIfError(HRESULT hr) {
	if (_checkError(hr)) {
		reportError(hr);
		throw SGE_ERROR("HRESULT = {}", hr);
	}
}

inline
bool DX11Util::assertIfError(HRESULT hr) {
	if (_checkError(hr)) {
		reportError(hr);
		SGE_ASSERT(false);
		return false;
	}
	return true;
}

inline
String DX11Util::getStrFromHRESULT(HRESULT hr) {
	// retrieving-error-messages: https://docs.microsoft.com/en-us/windows/win32/seccrypto/retrieving-error-messages

	const int bufSize = 4096;
	wchar_t buf[bufSize + 1];

	auto dwChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								nullptr,
								hr,
								0,
								buf,
								bufSize,
								nullptr);

	buf[bufSize] = 0; // ensure terminate with 0

	auto str = dwChars ? UtfUtil::toString(buf) : String("Error message not found.");
	return str;
}

inline
void DX11Util::reportError(HRESULT hr) {
	if (_checkError(hr)) {
		auto str = getStrFromHRESULT(hr);
		SGE_LOG("HRESULT(0x{:0X}) {}", static_cast<u32>(hr), str);
	}
}

} // namespace

#endif // SGE_RENDER_HAS_DX11