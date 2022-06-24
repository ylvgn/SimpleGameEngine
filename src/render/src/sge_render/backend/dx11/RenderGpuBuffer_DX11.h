#pragma once
#include "Render_DX11_Common.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderGpuBuffer_DX11 : public RenderGpuBuffer {
	using Base = RenderGpuBuffer;
	using Util = DX11Util;
public:
	RenderGpuBuffer_DX11(CreateDesc& desc);
	DX11_ID3DBuffer* d3dBuf() { return _d3dBuf; }
protected:
	virtual void onUploadToGpu(ByteSpan data, size_t offset) override;
private:
	ComPtr<DX11_ID3DBuffer> _d3dBuf;

}; // RenderGpuBuffer_DX11

} // namespace