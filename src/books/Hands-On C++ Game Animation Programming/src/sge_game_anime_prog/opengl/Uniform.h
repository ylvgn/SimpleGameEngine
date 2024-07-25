#pragma once

namespace sge {
/*
	uniforms are constant data;
	they are set once. The value of a uniform remains the same for all vertices processed.
	there will never be an instance of a Uniform class.
	It only needs public static functions.
	In OpenGL, uniforms are set with the glUniform*** family of functions.
*/

template <typename T>
struct Uniform {
	Uniform() = delete;

	static void set(GLuint loc, const T& value);
	static void set(GLuint loc, const T* data, size_t dataSize);
	static void set(GLuint loc, const Span<const T>& value);
};

}