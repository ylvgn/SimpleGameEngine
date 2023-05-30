#include "Attribute.h"
#include "glad/glad.h"

#include <sge_game_anime_prog/math/vec2.h>
#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/vec4.h>
#include <sge_game_anime_prog/math/quat.h>

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
template Attribute<Color4f>;

template<typename T>
Attribute<T>::Attribute()
	: _count(0)
{
	glGenBuffers(1, &_handle); // generate an OpenGL buffer and store it in the handle
}

template<typename T>
Attribute<T>::~Attribute() {
	glDeleteBuffers(1, &_handle);
}

template<typename T>
void Attribute<T>::uploadToGpu(const T* data, size_t len) {
	//uploadToGpu(spanCast<const u8>(Span<const T>(data, len)));
	uploadToGpu(spanCast<const u8>(Span<const T>(data, len)));
}

template<typename T>
void Attribute<T>::uploadToGpu(const std::vector<T>& data) {
#if 1
	uploadToGpu(spanCast<const u8>(Span<const T>(data.data(), data.size())));
#else
	uploadToGpu(&data[0], data.size());
#endif
}

template<typename T>
void Attribute<T>::uploadToGpu(const Vector<T>& data) {
	uploadToGpu(spanCast<const u8>(data.span()));
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
void Attribute<vec2>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void Attribute<vec3>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void Attribute<vec4>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<quat>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<>
void Attribute<Color4f>::_setAttribPointer(u32 slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

template<typename T>
void Attribute<T>::bind(u32 slot) {
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	{
		glEnableVertexAttribArray(slot);

		// Since the glVertexAttribPointer function is different based on the templated type of the Attribute class,
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

}