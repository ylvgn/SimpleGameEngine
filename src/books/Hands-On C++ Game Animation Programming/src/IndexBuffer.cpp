#include "IndexBuffer.h"

namespace sge {

IndexBuffer::IndexBuffer()
	: _count(0)
{
	glGenBuffers(1, &_handle);
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &_handle);
}

void IndexBuffer::uploadToGpu(const u32* data, size_t len) {
	_count = len;
	size_t bufferSize = sizeof(u32) * len;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::uploadToGpu(const Vector<u32>& data) {
	auto s = data.span();
	uploadToGpu(s.data(), s.size());
}

void IndexBuffer::uploadToGpu(const std::vector<u32>& data) {
	uploadToGpu(&data[0], data.size());
}

void IndexBuffer::uploadToGpu(ByteSpan data) {
	auto s = spanCast<const u32>(data);
	uploadToGpu(s.data(), s.size());
}

}