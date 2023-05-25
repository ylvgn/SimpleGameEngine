#include "Uniform.h"

#include "glad/glad.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat4.h"

namespace sge {

template Uniform<int>;
template Uniform<vec4i>;
template Uniform<vec2i>;
template Uniform<float>;
template Uniform<vec2>;
template Uniform<vec3>;
template Uniform<vec4>;
template Uniform<quat>;
template Uniform<mat4>;

#define UNIFORM_IMPL(gl_func, tType, dType) \
template<> \
void Uniform<tType>::set(u32 slot, tType* data, u32 length) { \
    gl_func(slot, static_cast<GLsizei>(length), reinterpret_cast<dType*>(&data[0])); \
} \
// ------
UNIFORM_IMPL(glUniform1iv, int,   int)
UNIFORM_IMPL(glUniform4iv, vec4i, int)
UNIFORM_IMPL(glUniform2iv, vec2i, int)
UNIFORM_IMPL(glUniform1fv, float, float)
UNIFORM_IMPL(glUniform2fv, vec2,  float)
UNIFORM_IMPL(glUniform3fv, vec3,  float)
UNIFORM_IMPL(glUniform4fv, vec4,  float)
UNIFORM_IMPL(glUniform4fv, quat,  float)

template<>
void Uniform<mat4>::set(u32 slot, mat4* inputArray, u32 len) {
	// The set function for matrices needs to be specified manually;
	// otherwise, the UNIFORM_IMPL macro won't work.
	// This is because the glUniformMatrix4fv function takes an additional Boolean argument
	// asking whether the matrix should be transposed or not.
	glUniformMatrix4fv(
		slot,
		static_cast<GLsizei>(len),
		false, // set the transposed Boolean to false
		(float*)&inputArray[0]
	);
}

template <typename T>
void Uniform<T>::set(u32 slot, const T& value) {
	set(slot, (T*)&value, 1);
}

template <typename T>
void Uniform<T>::set(u32 slot, Vector<T>& value) {
	auto span = value.span();
	set(slot, span.data(), static_cast<u32>(span.size()));
}

template <typename T>
void Uniform<T>::set(u32 slot, std::vector<T>& value) {
	set(slot, &value[0], static_cast<u32>(value.size()));
}

}