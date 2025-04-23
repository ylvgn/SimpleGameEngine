#pragma once

#include <sge_render/Render_Common.h>

#if SGE_RENDER_HAS_DX11

#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <sge_render/vertex/Vertex.h>
#include <sge_render/shader/Shader.h>
#include <sge_render/textures/Texture.h>

#include <sge_core/native_ui/win32/Win32Util.h>

namespace sge {

class Renderer_DX11;

// renderer ---------------
using DX11_IDXGIFactory				= IDXGIFactory1;
using DX11_IDXGIDevice				= IDXGIDevice;
using DX11_IDXGIAdapter				= IDXGIAdapter3;
using DX11_IDXGISwapChain			= IDXGISwapChain;

using DX11_ID3DDevice				= ID3D11Device1;
using DX11_ID3DDeviceContext		= ID3D11DeviceContext4;
using DX11_ID3DDebug				= ID3D11Debug;

// render context ---------------
using DX11_ID3DTexture1D			= ID3D11Texture1D;
using DX11_ID3DTexture2D			= ID3D11Texture2D;
using DX11_ID3DTexture3D			= ID3D11Texture3D;

using DX11_ID3DSamplerState			= ID3D11SamplerState;
using DX11_ID3DShaderResourceView	= ID3D11ShaderResourceView;

using DX11_ID3DRenderTargetView		= ID3D11RenderTargetView;
using DX11_ID3DDepthStencilView		= ID3D11DepthStencilView;

using DX11_ID3DBuffer				= ID3D11Buffer;
using DX11_ID3DBlob					= ID3DBlob;

using DX11_ID3DVertexShader			= ID3D11VertexShader;
using DX11_ID3DPixelShader			= ID3D11PixelShader;
using DX11_ID3DInputLayout			= ID3D11InputLayout;

using DX11_ID3DRasterizerState		= ID3D11RasterizerState;
using DX11_ID3DDepthStencilState	= ID3D11DepthStencilState;
using DX11_ID3DBlendState			= ID3D11BlendState;

// render reflect -------------
using DX11_ID3DShaderReflection		= ID3D11ShaderReflection;

struct DX11Util {
	DX11Util() = delete;

	static void throwIfError(::HRESULT hr);
	static bool assertIfError(::HRESULT hr);
	static void reportError(::HRESULT hr);

	static constexpr UINT castUINT(size_t v) { SGE_ASSERT(v < UINT_MAX); return UINT(v); }

	static D3D11_PRIMITIVE_TOPOLOGY		getDxPrimitiveTopology(RenderPrimitiveType t);
	static DXGI_FORMAT					getDxFormat(RenderDataType t);
	static DXGI_FORMAT					getDxColorType(ColorType t);

	static D3D11_CULL_MODE				getDxCullMode(RenderState_Cull c);
	static D3D11_COMPARISON_FUNC		getDxDepthTestOp(RenderState_DepthTestOp v);
	static D3D11_BLEND_OP				getDxBlendOp(RenderState_BlendOp v);
	static D3D11_BLEND					getDxBlendFactor(RenderState_BlendFactor v);

	static D3D11_FILTER					getDxTextureFilter(TextureFilter t);
	static D3D11_TEXTURE_ADDRESS_MODE	getDxTextureWrap(TextureWrap t);
	static D3D11_TEXTURECUBE_FACE		getDxTextureCubeFaceOrder(TextureCubeFaceOrder t);

	static const char*					getDxSemanticName(VertexSemanticType v);
	static VertexSemanticType			parseDxSemanticName(StrView s);

	static const char*					getDxStageProfile(ShaderStageMask s);

	static ByteSpan						toSpan(DX11_ID3DBlob* blob);
	static StrView						toStrView(DX11_ID3DBlob* blob) { return StrView_make(toSpan(blob)); }

	static void convert(Rect2f& o, const ::D3D11_RECT& i) {
		o.x = float(i.left);
		o.y = float(i.top);
		o.w = float(i.right - i.left);
		o.h = float(i.bottom - i.top);
	}

	static void convert(::D3D11_RECT& o, const Rect2f& i) {
		o.left   = LONG(i.x);
		o.top    = LONG(i.y);
		o.right  = LONG(i.xMax());
		o.bottom = LONG(i.yMax());
	}

	static Rect2f toRect2f(const ::D3D11_RECT& i)	{ Rect2f o;			convert(o, i);	return o; }
	static ::D3D11_RECT toD3DRect(const Rect2f& i)	{ ::D3D11_RECT o;	convert(o, i);	return o; }

private:
	static bool _checkError(::HRESULT hr) {
		return FAILED(hr); // if got error, return true
	}
};

SGE_INLINE
ByteSpan DX11Util::toSpan(DX11_ID3DBlob* blob) {
	if (!blob) return ByteSpan();
	return ByteSpan(reinterpret_cast<const u8*>(blob->GetBufferPointer()),
					static_cast<size_t>(blob->GetBufferSize()));
}

SGE_INLINE
void DX11Util::throwIfError(::HRESULT hr) {
	if (_checkError(hr)) {
		reportError(hr);
		throw SGE_ERROR("HRESULT = {}", hr);
	}
}

SGE_INLINE
bool DX11Util::assertIfError(::HRESULT hr) {
	if (_checkError(hr)) {
		reportError(hr);
		SGE_ASSERT(false);
		return false;
	}
	return true;
}

SGE_INLINE
const char* DX11Util::getDxStageProfile(ShaderStageMask s) {
	using SRC = ShaderStageMask;
	switch (s) {
		case SRC::Vertex:	return ShaderStageProfile::DX11_VS;
		case SRC::Pixel:	return ShaderStageProfile::DX11_PS;
	//---
		default: return "";
	}
}

SGE_INLINE
void DX11Util::reportError(::HRESULT hr) {
	if (_checkError(hr)) {
		auto str = Win32Util::error(hr);
		SGE_LOG("HRESULT(0x{:0X}) {}", static_cast<u32>(hr), str);
	}
}

SGE_INLINE
D3D11_PRIMITIVE_TOPOLOGY DX11Util::getDxPrimitiveTopology(RenderPrimitiveType t) {
	// https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_primitive_topology
	using SRC = RenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case SRC::Lines:		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case SRC::Triangles:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//---
		default: throw SGE_ERROR("unknown RenderPrimitiveType");
	}
}

SGE_INLINE
DXGI_FORMAT DX11Util::getDxFormat(RenderDataType t) {
	// https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
	using SRC = RenderDataType;
	switch (t) {
		case SRC::Int8:			return DXGI_FORMAT_R8_SINT;
		case SRC::Int8x2:		return DXGI_FORMAT_R8G8_SINT;
//		case SRC::Int8x3:		return DXGI_FORMAT_R8G8B8_SINT; //does not support in DX11
		case SRC::Int8x4:		return DXGI_FORMAT_R8G8B8A8_SINT;

		case SRC::UInt8:		return DXGI_FORMAT_R8_UINT;
		case SRC::UInt8x2:		return DXGI_FORMAT_R8G8_UINT;
//		case SRC::UInt8x3:		return DXGI_FORMAT_R8G8B8_UINT; //does not support in DX11
		case SRC::UInt8x4:		return DXGI_FORMAT_R8G8B8A8_UINT;

		case SRC::SNorm8:		return DXGI_FORMAT_R8_SNORM;
		case SRC::SNorm8x2:		return DXGI_FORMAT_R8G8_SNORM;
//		case SRC::SNorm8x3:		return DXGI_FORMAT_R8G8B8_SNORM; //does not support in DX11
		case SRC::SNorm8x4:		return DXGI_FORMAT_R8G8B8A8_SNORM;

		case SRC::UNorm8:		return DXGI_FORMAT_R8_UNORM;
		case SRC::UNorm8x2:		return DXGI_FORMAT_R8G8_UNORM;
//		case SRC::UNorm8x3:		return DXGI_FORMAT_R8G8B8_UNORM; //does not support in DX11
		case SRC::UNorm8x4:		return DXGI_FORMAT_R8G8B8A8_UNORM;

		case SRC::Int16:		return DXGI_FORMAT_R16_SINT;
		case SRC::Int16x2:		return DXGI_FORMAT_R16G16_SINT;
//		case SRC::Int16x3:		return DXGI_FORMAT_R16G16B16_SINT; //does not support in DX11
		case SRC::Int16x4:		return DXGI_FORMAT_R16G16B16A16_SINT;

		case SRC::UInt16:		return DXGI_FORMAT_R16_UINT;
		case SRC::UInt16x2:		return DXGI_FORMAT_R16G16_UINT;
//		case SRC::UInt16x3:		return DXGI_FORMAT_R16G16B16_UINT; //does not support in DX11
		case SRC::UInt16x4:		return DXGI_FORMAT_R16G16B16A16_UINT;

		case SRC::SNorm16:		return DXGI_FORMAT_R16_SNORM;
		case SRC::SNorm16x2:	return DXGI_FORMAT_R16G16_SNORM;
//		case SRC::SNorm16x3:	return DXGI_FORMAT_R16G16B16_SNORM; //does not support in DX11
		case SRC::SNorm16x4:	return DXGI_FORMAT_R16G16B16A16_SNORM;

		case SRC::UNorm16:		return DXGI_FORMAT_R16_UNORM;
		case SRC::UNorm16x2:	return DXGI_FORMAT_R16G16_UNORM;
//		case SRC::UNorm16x3:	return DXGI_FORMAT_R16G16B16_UNORM; //does not support in DX11
		case SRC::UNorm16x4:	return DXGI_FORMAT_R16G16B16A16_UNORM;

		case SRC::Int32:		return DXGI_FORMAT_R32_SINT;
		case SRC::Int32x2:		return DXGI_FORMAT_R32G32_SINT;
//		case SRC::Int32x3:		return DXGI_FORMAT_R32G32B32_SINT; //does not support in DX11
		case SRC::Int32x4:		return DXGI_FORMAT_R32G32B32A32_SINT;

		case SRC::UInt32:		return DXGI_FORMAT_R32_UINT;
		case SRC::UInt32x2:		return DXGI_FORMAT_R32G32_UINT;
//		case SRC::UInt32x3:		return DXGI_FORMAT_R32G32B32_UINT; //does not support in DX11
		case SRC::UInt32x4:		return DXGI_FORMAT_R32G32B32A32_UINT;

//		case SRC::SNorm32:		return DXGI_FORMAT_R32_SNORM;
//		case SRC::SNorm32x2:	return DXGI_FORMAT_R32G32_SNORM;
//		case SRC::SNorm32x3:	return DXGI_FORMAT_R32G32B32_SNORM; //does not support in DX11
//		case SRC::SNorm32x4:	return DXGI_FORMAT_R32G32B32A32_SNORM;

//		case SRC::UNorm32:		return DXGI_FORMAT_R32_UNORM;
//		case SRC::UNorm32x2:	return DXGI_FORMAT_R32G32_UNORM;
//		case SRC::UNorm32x3:	return DXGI_FORMAT_R32G32B32_UNORM; //does not support in DX11
//		case SRC::UNorm32x4:	return DXGI_FORMAT_R32G32B32A32_UNORM;

	//--
		case SRC::Float16:		return DXGI_FORMAT_R16_FLOAT;
		case SRC::Float16x2:	return DXGI_FORMAT_R16G16_FLOAT;
//		case SRC::Float16x3:	return DXGI_FORMAT_R16G16B16_FLOAT; //does not support in DX11
		case SRC::Float16x4:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
	//---
		case SRC::Float32:		return DXGI_FORMAT_R32_FLOAT;
		case SRC::Float32x2:	return DXGI_FORMAT_R32G32_FLOAT;
		case SRC::Float32x3:	return DXGI_FORMAT_R32G32B32_FLOAT;
		case SRC::Float32x4:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
	//---
		default: throw SGE_ERROR("unsupported RenderDataType '{}'", t);
	}
}

SGE_INLINE
DXGI_FORMAT DX11Util::getDxColorType(ColorType t) {
	// https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
	using SRC = ColorType;
	switch (t) {
		case SRC::Lb:		return DXGI_FORMAT_R8_UNORM;
		case SRC::Ls:		return DXGI_FORMAT_R16_UNORM;
		case SRC::Lf:		return DXGI_FORMAT_R32_FLOAT;

		case SRC::Rb:		return DXGI_FORMAT_R8_UNORM;
		case SRC::Rs:		return DXGI_FORMAT_R16_UNORM;
		case SRC::Rf:		return DXGI_FORMAT_R32_FLOAT;

		case SRC::RGb:		return DXGI_FORMAT_R8G8_UNORM;
		case SRC::RGs:		return DXGI_FORMAT_R16G16_UNORM;
		case SRC::RGf:		return DXGI_FORMAT_R32G32_FLOAT;

//		case SRC::RGBb:		return DXGI_FORMAT_R8G8B8_UNORM;		// DX Not Support
//		case SRC::RGBf:		return DXGI_FORMAT_R32G32B32_FLOAT;		// DX Not Support

		case SRC::RGBAb:	return DXGI_FORMAT_R8G8B8A8_UNORM;
		case SRC::RGBAs:	return DXGI_FORMAT_R16G16B16A16_UNORM;
		case SRC::RGBAf:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
	//---
		case SRC::BC1:		return DXGI_FORMAT_BC1_UNORM;
		case SRC::BC2:		return DXGI_FORMAT_BC2_UNORM;
		case SRC::BC3:		return DXGI_FORMAT_BC3_UNORM;
		case SRC::BC4:		return DXGI_FORMAT_BC4_UNORM;
		case SRC::BC5:		return DXGI_FORMAT_BC5_UNORM;
		case SRC::BC6h:		return DXGI_FORMAT_BC6H_UF16;
		case SRC::BC7:		return DXGI_FORMAT_BC7_UNORM;
	//---
		default:			throw SGE_ERROR("unsupported ColorType '{}'", t);
	}
}

SGE_INLINE
D3D11_CULL_MODE DX11Util::getDxCullMode(RenderState_Cull c) {
	// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_cull_mode
	using SRC = RenderState_Cull;
	switch (c) {
		case SRC::None:		return D3D11_CULL_NONE;
		case SRC::Back:		return D3D11_CULL_BACK;
		case SRC::Front:	return D3D11_CULL_FRONT;
	//---
		default: throw SGE_ERROR("unsupported RenderState CullMode '{}'", c);
	}
}

SGE_INLINE
D3D11_COMPARISON_FUNC DX11Util::getDxDepthTestOp(RenderState_DepthTestOp d) {
	// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_comparison_func
	using SRC = RenderState_DepthTestOp;
	switch (d) {
		case SRC::Always:		return  D3D11_COMPARISON_ALWAYS;
		case SRC::Less:			return  D3D11_COMPARISON_LESS;
		case SRC::Equal:		return  D3D11_COMPARISON_EQUAL;
		case SRC::LessEqual:	return  D3D11_COMPARISON_LESS_EQUAL;
		case SRC::Greater:		return  D3D11_COMPARISON_GREATER;
		case SRC::GreaterEqual:	return  D3D11_COMPARISON_GREATER_EQUAL;
		case SRC::NotEqual:		return  D3D11_COMPARISON_NOT_EQUAL;
		case SRC::Never:		return  D3D11_COMPARISON_NEVER;
	//---
		default: throw SGE_ERROR("unsupported RenderState DepthTestOp '{}'", d);
	}
}

SGE_INLINE
D3D11_BLEND_OP DX11Util::getDxBlendOp(RenderState_BlendOp b) {
	// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_blend_op
	using SRC = RenderState_BlendOp;
	switch (b) {
		case SRC::Add:		return D3D11_BLEND_OP_ADD;
		case SRC::Min:		return D3D11_BLEND_OP_MIN;
		case SRC::Max:		return D3D11_BLEND_OP_MAX;
		case SRC::Sub:		return D3D11_BLEND_OP_SUBTRACT;
		case SRC::RevSub:	return D3D11_BLEND_OP_REV_SUBTRACT;
	//---
		default: throw SGE_ERROR("unsupported RenderState BlendOp '{}'", b);
	}
}

SGE_INLINE
D3D11_BLEND DX11Util::getDxBlendFactor(RenderState_BlendFactor b) {
	// https://docs.microsoft.com/zh-tw/windows/win32/api/d3d11/ne-d3d11-d3d11_blend
	using SRC = RenderState_BlendFactor;
	switch (b) {
		case SRC::Zero:					return D3D11_BLEND_ZERO;
		case SRC::One:					return D3D11_BLEND_ONE;
		case SRC::SrcAlpha:				return D3D11_BLEND_SRC_ALPHA;
		case SRC::DstAlpha:				return D3D11_BLEND_DEST_ALPHA;
		case SRC::SrcColor:				return D3D11_BLEND_SRC_COLOR;
		case SRC::DstColor:				return D3D11_BLEND_DEST_COLOR;
		case SRC::ConstColor:			return D3D11_BLEND_BLEND_FACTOR;
//		case SRC::ConstAlpha:			return 
		case SRC::OneMinusSrcAlpha:		return D3D11_BLEND_INV_SRC_ALPHA;
		case SRC::OneMinusSrcColor:		return D3D11_BLEND_INV_SRC_COLOR;
		case SRC::OneMinusDstAlpha:		return D3D11_BLEND_INV_DEST_ALPHA;
		case SRC::OneMinusDstColor:		return D3D11_BLEND_INV_DEST_COLOR;
		case SRC::OneMinusConstColor:	return D3D11_BLEND_INV_BLEND_FACTOR;
//		case SRC::OneMinusConstAlpha:	return 
		case SRC::SrcAlphaSaturate:		return D3D11_BLEND_SRC_ALPHA_SAT;
		default: throw SGE_ERROR("unsupported RenderState BlendFactor '{}'", b);
	}
}

SGE_INLINE
D3D11_FILTER DX11Util::getDxTextureFilter(TextureFilter t) {
	// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_filter
	using SRC = TextureFilter;
	switch (t) {
		case SRC::Point:		return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case SRC::Linear:		return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case SRC::Bilinear:		return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case SRC::Trilinear:	return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case SRC::Anisotropic:	return D3D11_FILTER_ANISOTROPIC;
	//---
		default: throw SGE_ERROR("unsupported TextureFilter '{}'", t);
	}
}

SGE_INLINE
D3D11_TEXTURE_ADDRESS_MODE DX11Util::getDxTextureWrap(TextureWrap t) {
	// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_texture_address_mode
	using SRC = TextureWrap;
	switch (t) {
		case SRC::Repeat:		return D3D11_TEXTURE_ADDRESS_WRAP;
		case SRC::Clamp:		return D3D11_TEXTURE_ADDRESS_CLAMP;
		case SRC::Mirror:		return D3D11_TEXTURE_ADDRESS_MIRROR;
		case SRC::MirrorOnce:	return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	//---
		default: throw SGE_ERROR("unsupported TextureWrap '{}'", t);
	}
}

SGE_INLINE
D3D11_TEXTURECUBE_FACE DX11Util::getDxTextureCubeFaceOrder(TextureCubeFaceOrder t) {
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_texturecube_face
	using SRC = TextureCubeFaceOrder;
	switch (t) {
		case SRC::Right:	return D3D11_TEXTURECUBE_FACE_POSITIVE_X;
		case SRC::Left:		return D3D11_TEXTURECUBE_FACE_NEGATIVE_X;
		case SRC::Top:		return D3D11_TEXTURECUBE_FACE_POSITIVE_Y;
		case SRC::Bottom:	return D3D11_TEXTURECUBE_FACE_NEGATIVE_Y;
		case SRC::Front:	return D3D11_TEXTURECUBE_FACE_POSITIVE_Z;
		case SRC::Back:		return D3D11_TEXTURECUBE_FACE_NEGATIVE_Z;
	//---
		default: throw SGE_ERROR("unsupported TextureCubeFaceOrder '{}'", t);
	}
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11