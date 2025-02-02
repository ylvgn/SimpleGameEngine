#include "Shader.h"

#include <sge_core/file/MemMapFile.h>

namespace sge {

Shader::Shader()
	: _handle(glCreateProgram())
{}

Shader::Shader(StrView vertex, StrView fragment)
	: _handle(glCreateProgram())
{
	load(vertex, fragment);
}

Shader::~Shader() {
	glDeleteProgram(_handle);
}

// Once the shaders are read, call the Compile, Link, and Populate helper functions to load the shader
void Shader::load(StrView vertex, StrView fragment) {
	// This function takes two strings, which are either filenames or inline shader definitions.

	u32 v_shader, f_shader;
	if (File::exists(vertex)) {
		v_shader = _compileVertexShader(_readFile(vertex));
	} else {
		v_shader = _compileVertexShader(vertex);
	}
	SGE_ASSERT(v_shader != 0);

	if (File::exists(fragment)) {
		f_shader = _compileFragmentShader(_readFile(fragment));
	} else {
		f_shader = _compileFragmentShader(fragment);
	}
	SGE_ASSERT(f_shader != 0);

	if (!_linkShaders(v_shader, f_shader)) {
		throw SGE_ERROR("_linkShaders");
	}
	_populateAttributes();
	_populateUniforms();
}

void Shader::bind() {
	glUseProgram(_handle);
}

void Shader::unbind() {
	glUseProgram(0);
}

String Shader::_readFile(StrView path) {
	MemMapFile mm;
	mm.open(path);
	return _readMem(mm, path);
}

String Shader::_readMem(ByteSpan data, StrView) {
	return String(StrView_make(data));
}

u32 Shader::_compileVertexShader(StrView vertex) {
	// First, create the shader object with glCreateShader,
	u32 v_shader = glCreateShader(GL_VERTEX_SHADER);
	TempString v_source = vertex;
	const char* sz = v_source.c_str();

	// then set the source for the shader with glShaderSource.
	glShaderSource(v_shader, 1, &sz, NULL);

	// Finally, compile the shader with glCompileShader.
	glCompileShader(v_shader);

	// Check for errors with glGetShaderiv
	int success;
	glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		char infoLog[kInfoLogSize];
		glGetShaderInfoLog(v_shader, kInfoLogSize, NULL, infoLog);
		glDeleteShader(v_shader);
		SGE_LOG("Vertex compilation failed.\n{}", infoLog);
		return 0;
	};
	return v_shader;
}

u32 Shader::_compileFragmentShader(StrView fragment) {
	// The CompileFragmentShader function is almost identical to the CompileVertexShader function.
	// The only real difference is the argument to glCreateShader
	u32 f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	TempString v_source = fragment;
	const char* sz = v_source.c_str();
	glShaderSource(f_shader, 1, &sz, NULL);
	glCompileShader(f_shader);

	int success;
	glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		char infoLog[kInfoLogSize];
		glGetShaderInfoLog(f_shader, kInfoLogSize, NULL, infoLog);
		glDeleteShader(f_shader);
		SGE_LOG("Fragment compilation failed.\n{}", infoLog);
		return 0;
	};
	return f_shader;
}

bool Shader::_linkShaders(u32 vertex, u32 fragment) {

	// Attach the shaders to the shader program handle that the constructor created.
	glAttachShader(_handle, vertex);
	glAttachShader(_handle, fragment);

	// Link the shaders by calling glLinkProgram
	glLinkProgram(_handle);

	// check for errors with glGetProgramiv
	int success;
	glGetProgramiv(_handle, GL_LINK_STATUS, &success);
	if (success != GL_TRUE) {
		char infoLog[kInfoLogSize];
		glGetProgramInfoLog(_handle, kInfoLogSize, NULL, infoLog);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		SGE_LOG("Shader linking failed.\n{}", infoLog);
		return false;
	}

	// Once the shaders are linked, you only need the program
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return true;
}

void Shader::_populateAttributes() {
	int count = -1;
	int length;
	char name[kNameSize];
	int size;
	GLenum type;

	glUseProgram(_handle);
	{
		// You can count the number of active attributes in a shader program with the glGetProgramiv function,
		glGetProgramiv(_handle, GL_ACTIVE_ATTRIBUTES, &count);

		// Then loop through all the attributes by index
		for (int i = 0; i < count; ++i) {
			memset(name, 0, sizeof(char) * kNameSize);

			// and use glGetActiveAttrib to get the name of each attribute.
			glGetActiveAttrib(_handle, static_cast<GLuint>(i), kNameSize, &length, &size, &type, name);

			// Finally, call glGetAttribLocation to get the location of each attribute
			auto loc = glGetAttribLocation(_handle, name);

			if (loc >= 0) {
				_attributes[name] = loc;
			}
		}
	}
	glUseProgram(0);
}

void Shader::_populateUniforms() {
	// The PopulateUniforms is very similar to the PopulateAttributes
	int count = -1;
	int length;
	char name[kNameSize];
	int size;
	GLenum type;
	TempString testName;

	glUseProgram(_handle);
	{
		// glGetProgramiv needs to take GL_ACTIVE_UNIFORMS as the parameter name
		// and you need to call glGetActiveUniform and glGetUniformLocation:
		glGetProgramiv(_handle, GL_ACTIVE_UNIFORMS, &count);

		for (int i = 0; i < count; ++i) {
			memset(name, 0, sizeof(char) * kNameSize);
			glGetActiveUniform(_handle, static_cast<GLuint>(i), kNameSize, &length, &size, &type, name);

			int uniform = glGetUniformLocation(_handle, name);
			if (uniform >= 0) { // Is uniform valid?

				// When a valid uniform is encountered,
				// you need to determine whether the uniform is an array or not.
				// To do this, search for the array bracket '[' in the uniform name.
				// If the bracket is found, the uniform is an array

				TempString uniformName = name;
				size_t found = uniformName.find('[');
				if (found != TempString::npos) { // uniform array
					
					// If you encounter an uniform array, erase everything out of the string, starting with '['
					// This will leave you with only the uniform name.
					uniformName.erase(uniformName.begin() + found, uniformName.end());

					// Then, enter a loop where you try to retrieve every index from the array by appending[+index + ] to the uniform name.
					// Once the first invalid index is found, break the loop

					// Populate subscripted names too
					u32 uniformIndex = 0;
					while (true) {
						testName.clear();
						FmtTo(testName, "{}[{}]", uniformName.c_str(), uniformIndex++);
						int loc = glGetUniformLocation(_handle, testName.c_str());
						if (loc < 0) {
							break; // invalid index
						}
						_uniforms[testName.c_str()] = loc;
					}
				}
				_uniforms[uniformName.c_str()] = uniform;
			}
		}
	}
	glUseProgram(0);
}

void Shader::dumpUniformBlocks() {
	GLint blockCount;
	glGetProgramiv(_handle, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	TempString name;

	TempString out_ubName;
	for (int i = 0; i < blockCount; ++i) {
		GLint out_nameLen;
		glGetActiveUniformBlockiv(_handle, GLuint(i), GL_UNIFORM_BLOCK_NAME_LENGTH, &out_nameLen);
		out_ubName.resize(out_nameLen);

		glGetActiveUniformBlockName(_handle, GLuint(i), out_nameLen, NULL, out_ubName.data());

		auto ubIndex = glGetUniformBlockIndex(_handle, out_ubName.c_str());
		SGE_ASSERT(ubIndex != GL_INVALID_INDEX); // NOTE: ubIndex != i

		SGE_LOG("uniform block '{}' ({})", out_ubName.c_str(), ubIndex);
	}
}

void Shader::dumpActiveAttrib() {
	GLint activeCount = 0;
	glGetProgramiv(_handle, GL_ACTIVE_ATTRIBUTES, &activeCount);

	static const size_t kszNameSize = 1024;
	char out_szName[kszNameSize + 1];
	for (int i = 0; i < activeCount; ++i) {
		GLsizei out_length;
		GLint   out_dataSize;
		GLenum  out_dataType;

		glGetActiveAttrib(_handle, GLuint(i), kszNameSize, &out_length, &out_dataSize, &out_dataType, out_szName);
		out_szName[kszNameSize] = 0; // ensure terminate with 0

		auto loc = glGetAttribLocation(_handle, out_szName);
		SGE_ASSERT(loc >= 0); // NOTE: loc != i

		SGE_LOG("layout(location = {}) {}", loc, out_szName);
	}
}

} // namespace sge