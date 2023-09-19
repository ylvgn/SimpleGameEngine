#pragma once

namespace sge {

class Shader : public RefCountBase {
	static constexpr size_t kInfoLogSize = 512;
	static constexpr size_t kNameSize    = 128;
public:
	Shader();
	Shader(StrView vertex, StrView fragment);
	~Shader();

	// load shaders from files and compile them
	void load(StrView vertex, StrView fragment);

	void bind();   // call it before a shader is used
	void unbind(); // call it after a shader no longer in use

	u32 findAttributeByName(StrView name) const;
	u32 findUniformByName(StrView name) const;

	u32 handle() const { return _handle; }

private:

	// read the contents of a file into string
	String _readFile(StrView path);
	String _readMem(ByteSpan data, StrView path);

	// compile shader source code and return an OpenGL handle.
	u32 _compileVertexShader(StrView vertex);
	u32 _compileFragmentShader(StrView fragment);

	// link two shaders into a shader program.
	bool _linkShaders(u32 vertex, u32 fragment);

	// populate(fill out) the attribute and uniform dictionaries(maps)
	void _populateAttributes();
	void _populateUniforms();

	// OpenGL shader program handle (GPU resource)
	u32 _handle;

	// a key(the name of the attribute or uniform) and unsigned int for a value(the index of the uniform or attribute)
	StringMap<u32> _attributes;
	StringMap<u32> _uniforms;
};

inline u32 Shader::findAttributeByName(StrView name) const {
	auto it = _attributes.find(TempString(name).c_str());
	if (it == _attributes.end()) {
		SGE_LOG("Retrieving bad attribute index: {}\n", name);
		return 0;
	}
	return it->second;
}

inline u32 Shader::findUniformByName(StrView name) const {
	auto it = _uniforms.find(TempString(name).c_str());
	if (it == _uniforms.end()) {
		SGE_LOG("Retrieving bad uniform index: {}\n", name);
		return 0;
	}
	return it->second;
}

}