#include "Attribute.h"

#include "glad/glad.h"
#include <sge_game_anime_prog/math/quat.h>

namespace sge {
/*
	The Attribute class is templated, but none of its functions are marked as inline.
	The template specializations for each attribute type will live in the Attribute.cpp
*/

template<typename T>
Attribute<T>::Attribute()
	: _count(0)
{
	// generate an OpenGL buffer and store it in the handle
	// call any opengl function after "gladLoadGL" done
	glGenBuffers(1, &_handle);
}

template<typename T>
Attribute<T>::~Attribute() {
	glDeleteBuffers(1, &_handle);
}

template<typename T>
void Attribute<T>::bind(u32 slot) {
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	{
		glEnableVertexAttribArray(slot);

		// Since the 'glVertexAttribPointer' function is different based on the templated type of the Attribute class,
		// bind function will call the _setAttribPointer function 
		_setAttribPointer(slot);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::unbind(u32 slot) {
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	{
		glDisableVertexAttribArray(slot);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::uploadToGpu(ByteSpan data) {
	_count = data.size();
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	{
		// use glBufferData to fill the buffer with data
		glBufferData(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STREAM_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<>
void Attribute<int>::_setAttribPointer(u32 slot) {
	glVertexAttribIPointer(slot, 1, GL_INT, 0, nullptr);
}

template<>
void Attribute<vec4i>::_setAttribPointer(u32 slot) {
	glVertexAttribIPointer(slot, 4, GL_INT, 0, nullptr);
}

template<>
void Attribute<float>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void Attribute<vec2f>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void Attribute<vec3f>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void Attribute<vec4f>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<quat4f>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void Attribute<Color4f>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template Attribute<int>;
template Attribute<float>;
template Attribute<vec2f>;
template Attribute<vec3f>;
template Attribute<vec4f>;
template Attribute<vec4i>;
template Attribute<quat4f>;
template Attribute<Color4f>;

}