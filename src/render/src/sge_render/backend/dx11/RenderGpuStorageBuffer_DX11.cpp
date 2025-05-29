#include "RenderGpuStorageBuffer_DX11.h"
#include "Renderer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

RenderGpuStorageBuffer_DX11::RenderGpuStorageBuffer_DX11(CreateDesc& desc)
	: Base(desc)
{
	::D3D11_UNORDERED_ACCESS_VIEW_DESC rawUAVDesc = {};
	rawUAVDesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;
	rawUAVDesc.Buffer.Flags			= D3D11_BUFFER_UAV_FLAG_RAW;			// 0 when use D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	rawUAVDesc.Format				= DXGI_FORMAT_R32_TYPELESS;				// DXGI_FORMAT_UNKNOWN when use D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	rawUAVDesc.Buffer.NumElements	= Util::castUINT(desc.bufferSize / 4);  // divide 4 is Size in 32-bit units, and use Util::castUINT(desc.bufferSize / desc.stride); when  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED
	rawUAVDesc.Buffer.FirstElement	= 0;

	auto* dev = Util::d3dDevice();
	auto hr = dev->CreateUnorderedAccessView(gpuBuffer()->d3dBuf(), &rawUAVDesc, _uav.ptrForInit());
	Util::throwIfError(hr);
}

} // namespace sge

#endif // SGE_RENDER_HAS_DX11