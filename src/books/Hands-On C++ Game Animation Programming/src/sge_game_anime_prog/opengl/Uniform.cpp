#include "Uniform.h"

#include "glad/glad.h"
#include <sge_game_anime_prog/math/vec2.h>
#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/vec4.h>
#include <sge_game_anime_prog/math/quat.h>
#include <sge_game_anime_prog/math/mat4.h>

namespace sge {

template Uniform<int>;
template Uniform<float>;

template Uniform<vec2i>;
template Uniform<vec4i>;

template Uniform<vec2f>;
template Uniform<vec3f>;
template Uniform<vec4f>;

template Uniform<quat>;
template Uniform<mat4>;
template Uniform<Color4f>;

#define UNIFORM_IMPL(gl_func, T, dType) \
template<> \
void Uniform<T>::set(u32 slot, const T* data, size_t dataSize) { \
    gl_func(slot, static_cast<GLsizei>(dataSize), reinterpret_cast<const dType*>(&data[0])); \
} \
// ------
UNIFORM_IMPL(glUniform1iv, int,     int)
UNIFORM_IMPL(glUniform1fv, float,   float)
UNIFORM_IMPL(glUniform2iv, vec2i,   int)
UNIFORM_IMPL(glUniform4iv, vec4i,   int)
UNIFORM_IMPL(glUniform2fv, vec2f,   float)
UNIFORM_IMPL(glUniform3fv, vec3f,   float)
UNIFORM_IMPL(glUniform4fv, vec4f,   float)
UNIFORM_IMPL(glUniform4fv, quat,    float)
UNIFORM_IMPL(glUniform4fv, Color4f, float)

template<>
void Uniform<mat4>::set(u32 slot, const mat4* data, size_t dataSize) {
	// The set function for matrices needs to be specified manually;
	// otherwise, the UNIFORM_IMPL macro won't work.
	// This is because the glUniformMatrix4fv function takes an additional Boolean argument
	// asking whether the matrix should be transposed or not.
	glUniformMatrix4fv(
		slot,
		static_cast<GLsizei>(dataSize),
		false, // set the transposed Boolean to false
		(float*)&data[0]
	);
}

template <typename T>
void Uniform<T>::set(u32 slot, const T& value) {
	set(slot, (T*)&value, 1);
}

template <typename T>
void Uniform<T>::set(u32 slot, const Vector<T>& value) {
	set(slot, value.span());
}

template <typename T>
void Uniform<T>::set(u32 slot, const std::vector<T>& value) {
	set(slot, &value[0], value.size());
}

template <typename T>
void Uniform<T>::set(u32 slot, const Span<const T>& value) {
	set(slot, value.data(), value.size());
}

}