#pragma once

namespace sge {
/*
	Attributes are per-vertex data in the graphics pipeline.
	A vertex is made up of attributes.
	For example, a vertex has a position and a normal, which are both attributes.

	start implementing an Attribute class to hold vertex data that is passed to shaders.
	Attributes can have different data types.

	Attributes aka vertex buffer (GPU buffer)
*/

template<typename T>
class Attribute : public NonCopyable {
public:
	Attribute();
	~Attribute() { destroy(); }

	void destroy();

	// We need a way of binding and unbinding attributes from bind slots defined by the shader.
	// bind and unbind the attribute to a slot specified in the Shader class.
	void bind(GLuint loc);
	void unbind(GLuint loc);

	// it will upload an array of data to the GPU.
	// Each element in the array represents the attribute for one vertex.
	void uploadToGpu(ByteSpan data);

	GLuint	handle	() const { return _handle; }
	size_t	count	() const { return _count;  }

private:

	// it will need to be implemented once for each type of attribute that will be supported.
	// wraps glVertesAttribPointer or glVertesAttribIPointer
	// int, float, vec2/3/4, quat
	void _setAttribPointer(GLuint loc);

	GLuint	_handle = 0;	// OpenGL buffer handle
	size_t	_count = 0;		// The count represents how many total attributes there are
};

}