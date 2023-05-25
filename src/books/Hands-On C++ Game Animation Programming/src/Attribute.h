#pragma once

/*
Attributes are per-vertex data in the graphics pipeline.
A vertex is made up of attributes.
For example, a vertex has a position and a normal, which are both attributes.

start implementing an Attribute class to hold vertex data that is passed to shaders.
Attributes can have different data types.

Attributes aka vertex data (GPU buffer)
*/
namespace sge {

template<typename T>
class Attribute : public NonCopyable {
public:
	Attribute();
	~Attribute();

	// it will upload an array of data to the GPU.
	// Each element in the array represents the attribute for one vertex.
	void set(T* inputArray, u32 arrayLength);
	void set(std::vector<T>& input);
	void set(Vector<T>& input);

	// We need a way of binding and unbinding attributes from bind slots defined by the shader.
	// bind and unbind the attribute to a slot specified in the Shader class.
	void bind(u32 slot);
	void unbind(u32 slot);

	inline u32 handle() const { return _handle; }
	inline u32 count()  const { return _count; }

protected:
	u32 _handle; // OpenGL buffer handle
	u32 _count;  // The count represents how many total attributes there are

private:

	// it will need to be implemented once for each type of attribute that will be supported.
	// wraps glVertesAttribPointer or glVertesAttribIPointer
	// int, float, vec2/3/4, quat
	void _setAttribPointer(u32 slot);
	
};

}