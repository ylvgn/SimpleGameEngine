#include "Uniform.h"

#include <sge_game_anime_prog/math/vec2.h>
#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/vec4.h>
#include <sge_game_anime_prog/math/quat.h>
#include <sge_game_anime_prog/math/mat4.h>
#include <sge_game_anime_prog/math/dual_quat.h>

namespace sge {

#define SGE_UNIFORM_IMPL(FUNC, T, DATA_TYPE) \
template<> \
void Uniform<T>::set(GLuint loc, const T* data, size_t dataSize) { \
    FUNC(loc, \
		 static_cast<GLsizei>(dataSize), \
		 reinterpret_cast<const DATA_TYPE*>(&data[0]) \
	); \
} \
//----
	SGE_UNIFORM_IMPL(glUniform1iv, int,     int)
	SGE_UNIFORM_IMPL(glUniform2iv, vec2i,   int)
	SGE_UNIFORM_IMPL(glUniform4iv, vec4i,   int)
	SGE_UNIFORM_IMPL(glUniform1fv, float,   float)
	SGE_UNIFORM_IMPL(glUniform2fv, vec2f,   float)
	SGE_UNIFORM_IMPL(glUniform3fv, vec3f,   float)
	SGE_UNIFORM_IMPL(glUniform4fv, vec4f,   float)
	SGE_UNIFORM_IMPL(glUniform4fv, quat4f,  float)
	SGE_UNIFORM_IMPL(glUniform4fv, Color4f, float)
#undef SGE_UNIFORM_IMPL

template<>
void Uniform<mat4f>::set(GLuint loc, const mat4f* data, size_t dataSize) {
	// The set function for matrices needs to be specified manually;
	// otherwise, the UNIFORM_IMPL macro won't work.
	// This is because the glUniformMatrix4fv function takes an additional Boolean argument
	// asking whether the matrix should be transposed or not.

	glUniformMatrix4fv (loc,
						static_cast<GLsizei>(dataSize),
						false, // set the transposed Boolean to false
						static_cast<const GLfloat*>(data->ptr())
	);
}

template<>
void Uniform<dual_quat4f>::set(GLuint loc, const dual_quat4f* data, size_t dataSize) {
	// mat2x4
	glUniformMatrix2x4fv(loc,
						 static_cast<GLsizei>(dataSize),
						 false,
						 static_cast<const GLfloat*>(data->ptr())
	);
}

template <typename T>
void Uniform<T>::set(GLuint loc, const T& value) {
	set(loc, (T*)&value, 1);
}

template <typename T>
void Uniform<T>::set(GLuint loc, const Span<const T>& value) {
	set(loc, value.data(), value.size());
}

template Uniform<int>;
template Uniform<vec2i>;
template Uniform<vec4i>;
template Uniform<float>;
template Uniform<vec2f>;
template Uniform<vec3f>;
template Uniform<vec4f>;
template Uniform<quat4f>;
template Uniform<Color4f>;
template Uniform<mat4f>;
template Uniform<dual_quat4f>;

}