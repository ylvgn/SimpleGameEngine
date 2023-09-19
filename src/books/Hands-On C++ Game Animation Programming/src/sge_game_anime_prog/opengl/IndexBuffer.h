#pragma once

namespace sge {
/*
	IndexBuffer is bound to GL_ELEMENT_ARRAY_BUFFER and can be used to draw primitives.
	IndexBuffer aka index buffer (GPU buffer), similar to Attribute
*/
class IndexBuffer : public NonCopyable {
public:
	IndexBuffer();
	~IndexBuffer();

	inline void uploadToGpu(ByteSpan data) {
		auto s = spanCast<const u32>(data);
		_internalUploadToGpu(s.data(), s.size());
	}

	inline u32		handle() const { return _handle; }
	inline size_t	count()  const { return _count;  }

private:
	void _internalUploadToGpu(const u32* data, size_t dataSize);

	u32		_handle;	// OpenGL buffer handle
	size_t	_count;		// The count represents how many total indexbuffer there are
};

}