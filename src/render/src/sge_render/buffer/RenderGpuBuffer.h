#pragma once

namespace sge {

enum class RenderGpuBufferType {
	None,
	Vertex,
	Index,
	Const,
};
#define RenderGpuBufferType_ENUM_LIST(E) \
	E(None)		\
	E(Vertex)	\
	E(Index)	\
	E(Const)	\
//----
SGE_ENUM_STR_UTIL(RenderGpuBufferType)

struct RenderGpuBuffer_CreateDesc {
	using Type = RenderGpuBufferType;

	Type	type = Type::None;
	size_t	bufferSize = 0;
	size_t	stride = 16;
};

class RenderGpuBuffer : public Object {
public:
	using Type = RenderGpuBufferType;
	using CreateDesc = RenderGpuBuffer_CreateDesc;

	RenderGpuBuffer(CreateDesc& desc);

	void uploadToGpu(ByteSpan data, size_t offset = 0) {
		if (data.size() + offset > _desc.bufferSize) {
			throw SGE_ERROR("out of range");
		}
		onUploadToGpu(data, offset);
	}

protected:
	virtual void onUploadToGpu(ByteSpan data, size_t offset) = 0;
	CreateDesc _desc;
}; // RenderGpuBuffer

} // namespace