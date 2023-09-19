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

void IndexBuffer::_internalUploadToGpu(const u32* data, size_t len) {
	_count = len;
	size_t bufferSize = sizeof(u32) * len; // cuz now DrawUtil::draw use GL_UNSIGNED_INT ..

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}