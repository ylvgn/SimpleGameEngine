#pragma once

#include <sge_render/textures/Texture.h>
#include <sge_render/backend/dx11/Render_DX11_Common.h>

namespace sge {

#if 0
#pragma mark ========= Texture2D_DX11 ============
#endif
class Texture2D_DX11 : public Texture2D {
	using Base = Texture2D;
	using Util = DX11Util;
public:
	Texture2D_DX11(CreateDesc& desc);

	DX11_ID3DShaderResourceView*	resourceView() { return _resourceView; }
	DX11_ID3DSamplerState*			samplerState() { return _samplerState; }

private:
	ComPtr<DX11_ID3DTexture2D>			_tex;
	ComPtr<DX11_ID3DShaderResourceView>	_resourceView;
	ComPtr<DX11_ID3DSamplerState>		_samplerState;
}; // Texture2D_DX11


#if 0
#pragma mark ========= TextureCube_DX11 ============
#endif
class TextureCube_DX11 : public TextureCube {
	using Base = TextureCube;
	using Util = DX11Util;
public:
	TextureCube_DX11(CreateDesc& desc);

	DX11_ID3DShaderResourceView*	resourceView() { return _resourceView; }
	DX11_ID3DSamplerState*			samplerState() { return _samplerState; }

private:
	ComPtr<DX11_ID3DTexture2D>			_tex;
	ComPtr<DX11_ID3DShaderResourceView>	_resourceView;
	ComPtr<DX11_ID3DSamplerState>		_samplerState;
}; // TextureCube_GL

} // namespace sge