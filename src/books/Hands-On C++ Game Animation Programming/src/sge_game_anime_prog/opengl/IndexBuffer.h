#pragma once

/*
	IndexBuffer is bound to GL_ELEMENT_ARRAY_BUFFER and can be used to draw primitives.
	IndexBuffer aka index buffer (GPU buffer), similar to Attribute
*/
namespace sge {

class IndexBuffer : public NonCopyable {
public:
	IndexBuffer();
	~IndexBuffer();

	void uploadToGpu(const u32* data, size_t len);
	void uploadToGpu(const Vector<u32>& data);
	void uploadToGpu(const std::vector<u32>& data);
	void uploadToGpu(ByteSpan data);

	inline u32		handle() const { return _handle; }
	inline size_t	count()  const { return _count; }

private:
	u32 _handle;   // OpenGL buffer handle
	size_t _count; // The count represents how many total indexbuffer there are
};

}