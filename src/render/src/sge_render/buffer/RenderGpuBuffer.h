#pragma once

#include "../RenderDataType.h"

namespace sge {

class RenderGpuBuffer_CreateDesc {
public:
	using Type = RenderGpuBufferType;

	Type	type		= Type::None;
	size_t	bufferSize	= 0;
	size_t	stride		= 16;
};

class RenderGpuBuffer : public Object {
public:
	using Type		 = RenderGpuBufferType;
	using CreateDesc = RenderGpuBuffer_CreateDesc;

	void create(CreateDesc& desc);

	size_t bufferSize() const { return _desc.bufferSize; }
	void uploadToGpu(ByteSpan data, size_t offset = 0);

	const CreateDesc& desc() const { return _desc; }

protected:
	virtual void onCreate(CreateDesc& desc) = 0;
	virtual void onUploadToGpu(ByteSpan data, size_t offset) = 0;
	CreateDesc _desc;
}; // RenderGpuBuffer

} // namespace sge