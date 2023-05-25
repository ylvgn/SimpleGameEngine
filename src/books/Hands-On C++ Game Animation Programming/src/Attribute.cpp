#include "Attribute.h"
#include "glad/glad.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"

/*
	The Attribute class is templated, but none of its functions are marked as inline.
	The template specializations for each attribute type will live in the Attribute.cpp
*/


namespace sge {

template Attribute<int>;
template Attribute<float>;
template Attribute<vec2>;
template Attribute<vec3>;
template Attribute<vec4>;
template Attribute<vec4i>;
template Attribute<quat>;

template<typename T>
Attribute<T>::Attribute() {
	glGenBuffers(1, &_handle); // generate an OpenGL buffer and store it in the handle
	_count = 0;
}

template<typename T>
Attribute<T>::~Attribute() {
	glDeleteBuffers(1, &_handle);
}

template<typename T>
void Attribute<T>::set(T* inputArray, u32 arrayLength) {
	_count = arrayLength;
	unsigned int size = sizeof(T);

	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	{
		// uses glBufferData to fill the buffer with data.
		glBufferData(GL_ARRAY_BUFFER, size * _count, inputArray, GL_STREAM_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::set(std::vector<T>& input) {
	set(&input[0], static_cast<u32>(input.size()));
}

template<typename T>
void Attribute<T>::set(Vector<T>& input) {
	auto s = input.span();
	set(s.data(), static_cast<u32>(s.size()));
}

template<>
void Attribute<int>::_setAttribPointer(u32 slot) {
	glVertexAttribIPointer(slot, 1, GL_INT, 0, (void*)0);
}

template<>
void Attribute<vec4i>::_setAttribPointer(u32 slot) {
	glVertexAttribIPointer(slot, 4, GL_INT, 0, (void*)0);
}

template<>
void Attribute<float>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec2>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec3>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec4>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<quat>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<typename T>
void Attribute<T>::bind(u32 slot) {
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	{
		glEnableVertexAttribArray(slot);

		// Since the glVertexAttribPointer function is different based on the templated type of the Attribute class,
		// Bind will call the _setAttribPointer function 
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




}