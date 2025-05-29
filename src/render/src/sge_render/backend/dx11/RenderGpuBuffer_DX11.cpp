#include "RenderGpuBuffer_DX11.h"
#include "Renderer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

void RenderGpuBuffer_DX11::onCreate(CreateDesc& desc)  {
	if (desc.bufferSize <= 0)	throw SGE_ERROR("buffer size = 0");
	if (desc.stride <= 0)		throw SGE_ERROR("stride == 0");

	::D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = Util::castUINT(Math::alignTo(desc.bufferSize, decltype(desc.bufferSize)(16)));
	bd.StructureByteStride = Util::castUINT(desc.stride);

	switch (desc.type) {
		case Type::Vertex: {
			bd.Usage			= D3D11_USAGE_DYNAMIC;
			bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Index: {
			bd.Usage			= D3D11_USAGE_DYNAMIC;
			bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Const: {
			bd.Usage			= D3D11_USAGE_DYNAMIC;
			bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
		}break;
		case Type::Storage: {
			bd.Usage			= D3D11_USAGE_DEFAULT; // GPU read/write
			bd.BindFlags		= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;
			bd.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS; // D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
		}break;
	//---
		default:
			throw SGE_ERROR("unsupported gpu buffer type: {}", desc.type);
	}

	if (bd.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) {
		if (bd.ByteWidth != Math::alignTo(bd.ByteWidth, bd.StructureByteStride)) {
			throw SGE_ERROR("ByteWidth({}) must be an exact multiple of the StructureByteStride({})", bd.ByteWidth, bd.StructureByteStride);
		}
	}

	auto* renderer = Renderer_DX11::instance();
	auto* dev = renderer->d3dDevice();

	auto hr = dev->CreateBuffer(&bd, nullptr, _d3dBuf.ptrForInit());
	Util::throwIfError(hr);
}

void RenderGpuBuffer_DX11::onUploadToGpu(ByteSpan data, size_t offset) {
	auto* renderer = Renderer_DX11::instance();
	auto* ctx = renderer->d3dDeviceContext();

	::D3D11_MAPPED_SUBRESOURCE mapped = {};

	auto hr = ctx->Map(_d3dBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	Util::throwIfError(hr);

	u8* dst = reinterpret_cast<u8*>(mapped.pData);
	memcpy(dst + offset, data.data(), data.size());

	ctx->Unmap(_d3dBuf, 0);
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11