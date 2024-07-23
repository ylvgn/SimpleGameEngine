#include "IndexBuffer.h"

namespace sge {

IndexBuffer::IndexBuffer()
	: _count(0)
{
	// call any opengl function after "gladLoadGL" done
	glGenBuffers(1, &_handle);
}

void IndexBuffer::destroy() {
	if (_handle) {
		glDeleteBuffers(1, &_handle);
		_handle = 0;
	}
	_count = 0;
}

void IndexBuffer::uploadToGpu(ByteSpan data) {
	auto span = spanCast<const IndexType>(data);
	uploadToGpu(span.data(), span.size());
}

void IndexBuffer::uploadToGpu(const IndexType* p, size_t n) {
	if (!p || !n)
		return;

	_count = n;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*p) * n, p, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}