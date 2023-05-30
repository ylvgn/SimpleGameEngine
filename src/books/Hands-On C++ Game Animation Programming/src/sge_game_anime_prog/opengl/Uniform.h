#pragma once

/*
	uniforms are constant data;
	they are set once. The value of a uniform remains the same for all vertices processed.
	there will never be an instance of a Uniform class. It only needs public static functions.
	In OpenGL, uniforms are set with the glUniform*** family of functions.
*/

namespace sge {

template <typename T>
class Uniform : public NonCopyable {
	Uniform() = delete;
	~Uniform() = delete;

public:
	static void set(u32 slot, const T& value);
	static void set(u32 slot, T* arr, u32 len);
	static void set(u32 slot, std::vector<T>& arr);
	static void set(u32 slot, Vector<T>& arr);
};

}