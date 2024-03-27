#include "IndexBuffer.h"

namespace sge {

IndexBuffer::IndexBuffer()
	: _dataSize(0)
{
	// call any opengl function after "gladLoadGL" done
	glGenBuffers(1, &_handle);
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &_handle);
}

void IndexBuffer::uploadToGpu(ByteSpan data) {
	auto s = spanCast<const IndexType>(data);
	_internal_uploadToGpu(s.data(), s.size());
}

void IndexBuffer::_internal_uploadToGpu(const IndexType* data, size_t len) {
	if (len <= 0) return;

	SGE_ASSERT(data != nullptr);

	_dataSize = sizeof(data[0]) * len;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _dataSize, data, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}