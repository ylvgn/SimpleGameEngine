#pragma once

#include "Render_DX11_Common.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

#if SGE_RENDER_HAS_DX11

namespace sge {

class RenderGpuBuffer_DX11 : public RenderGpuBuffer {
	using Base = RenderGpuBuffer;
	using Util = DX11Util;
public:
	DX11_ID3DBuffer* d3dBuf() { return _d3dBuf; }
protected:
	virtual void onCreate(CreateDesc& desc) final;
	virtual void onUploadToGpu(ByteSpan data, size_t offset) final;
private:
	ComPtr<DX11_ID3DBuffer> _d3dBuf;

}; // RenderGpuBuffer_DX11

} // namespace sge

#endif // SGE_RENDER_HAS_DX11