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
	~IndexBuffer() { destroy(); }

	void destroy();

	void uploadToGpu(ByteSpan data);
	void uploadToGpu(const IndexType* p, size_t n);

	GLuint	handle		() const { return _handle; }
	size_t	count		() const { return _count; }
	size_t	byteSize	() const { return _count * sizeof(IndexType); }

private:
	GLuint _handle;	// OpenGL buffer handle
	size_t _count;	// The count represents how many total index buffer there are
};

}