#pragma once

#include "RenderGpuBuffer.h"

namespace sge {

class RenderGpuStorageBuffer_CreateDesc {
public:
	size_t	bufferSize = 0;
	size_t	stride	   = 0;
};

//! storage only on GPU side
class RenderGpuStorageBuffer : public Object {
public:
	using CreateDesc = RenderGpuStorageBuffer_CreateDesc;

	size_t bufferSize() const { return _gpuBuffer ? _gpuBuffer->bufferSize() : 0; }

	RenderGpuBuffer* gpuBuffer() { return _gpuBuffer; }

protected:
	RenderGpuStorageBuffer(CreateDesc& desc); // please create from 'Renderer::createGpuStorageBuffer'

	SPtr<RenderGpuBuffer> _gpuBuffer;
}; // RenderGpuStorageBuffer

} // namespace sge