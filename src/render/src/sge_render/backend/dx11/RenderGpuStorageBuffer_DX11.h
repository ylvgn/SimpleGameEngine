#pragma once

#include "Render_DX11_Common.h"
#include <sge_render/buffer/RenderGpuStorageBuffer.h>
#include "RenderGpuBuffer_DX11.h"

#if SGE_RENDER_HAS_DX11

namespace sge {

class RenderGpuStorageBuffer_DX11 : public RenderGpuStorageBuffer {
	using This = RenderGpuStorageBuffer_DX11;
	using Base = RenderGpuStorageBuffer;
	using Util = DX11Util;
public:
	RenderGpuStorageBuffer_DX11(CreateDesc& desc);

	RenderGpuBuffer_DX11*			gpuBuffer() { return static_cast<RenderGpuBuffer_DX11*>(Base::gpuBuffer()); }
	DX11_ID3DUnorderedAccessView*	uav()		{ return _uav; }
private:
	ComPtr<DX11_ID3DUnorderedAccessView> _uav;

}; // RenderGpuStorageBuffer_DX11

} // namespace sge

#endif // SGE_RENDER_HAS_DX11