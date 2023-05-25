#include "IndexBuffer.h"

namespace sge {

IndexBuffer::IndexBuffer() {
	glGenBuffers(1, &_handle);
	_count = 0;
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &_handle);
}

void IndexBuffer::set(u32* inputArray, u32 len) {
	_count = len;
	u32 size = sizeof(u32);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * _count, inputArray, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::set(Vector<u32>& input) {
	auto s = input.span();
	set(s.data(), static_cast<u32>(s.size()));
}

void IndexBuffer::set(std::vector<u32>& input) {
	set(&input[0], static_cast<u32>(input.size()));

}
}