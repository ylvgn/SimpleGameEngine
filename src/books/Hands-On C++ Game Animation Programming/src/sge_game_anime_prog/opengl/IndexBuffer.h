#pragma once

namespace sge {
/*
	IndexBuffer is bound to GL_ELEMENT_ARRAY_BUFFER and can be used to draw primitives.
	IndexBuffer aka index buffer (GPU buffer), similar to Attribute
*/

class IndexBuffer : public NonCopyable {
public:
	using IndexType = u32;

	IndexBuffer();
	~IndexBuffer();

	void uploadToGpu(ByteSpan data);

	u32		handle()	const { return _handle; }
	size_t	dataSize()  const { return _dataSize; }
	size_t	count()		const { return _dataSize / sizeof(IndexType); }

private:
	void _internal_uploadToGpu(const IndexType* data, size_t dataSize);

	u32		_handle;	// OpenGL buffer handle
	size_t	_dataSize;	// The count represents how many total indexbuffer there are
};

}