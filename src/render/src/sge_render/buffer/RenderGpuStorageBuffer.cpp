#include "RenderGpuStorageBuffer.h"
#include "../RenderDataType.h"
#include "../Renderer.h"

namespace sge {

RenderGpuStorageBuffer::RenderGpuStorageBuffer(CreateDesc& desc) {
	if (desc.bufferSize <= 0)	throw SGE_ERROR("buffer size <= 0");
	if (desc.stride <= 0)		throw SGE_ERROR("stride <= 0");

	RenderGpuBuffer_CreateDesc gpuBufDesc;
	gpuBufDesc.type			= RenderGpuBufferType::Storage;
	gpuBufDesc.bufferSize	= desc.bufferSize;
	gpuBufDesc.stride		= desc.stride;

	_gpuBuffer = Renderer::instance()->createGpuBuffer(gpuBufDesc);
}

} // namespace sge
