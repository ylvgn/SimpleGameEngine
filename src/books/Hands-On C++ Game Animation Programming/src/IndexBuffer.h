#pragma once

/*
	IndexBuffer is bound to GL_ELEMENT_ARRAY_BUFFER and can be used to draw primitives.
*/
namespace sge {

class IndexBuffer : public NonCopyable {
public:
	IndexBuffer();
	~IndexBuffer();

	void set(u32* inputArray, u32 arrLen);
	void set(Vector<u32>& input);
	void set(std::vector<u32>& input);

	inline u32 handle() const { return _handle; }
	inline u32 count()  const { return _count; }

private:
	u32 _handle; // OpenGL handle
	u32 _count;
};

}