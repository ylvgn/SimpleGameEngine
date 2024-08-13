#include "Shader_GL3.h"

namespace sge {

void Shader_GL3::loadFile(StrView filename) {
	_filename = filename;
	reload();
}

void Shader_GL3::destroy() {
	if (_vertexShader) {
		glDeleteShader(_vertexShader);
		_vertexShader = 0;
	}
	if (_pixelShader) {
		glDeleteShader(_pixelShader);
		_pixelShader = 0;
	}
	if (_program) {
		glDeleteProgram(_program);
		_program = 0;
	}
}

void Shader_GL3::reload() {
	destroy();

	{
		TempString tmp(_filename + ".glsl_vs");
		_compileShader(_vertexShader, GL_VERTEX_SHADER, tmp.c_str());
	}
	{
		TempString tmp(_filename + ".glsl_ps");
		_compileShader(_pixelShader, GL_FRAGMENT_SHADER, tmp.c_str());
	}

	_linkProgram();
}

void Shader_GL3::bind() {
	if (!_program)
		SGE_ASSERT(glIsProgram(_program) == GL_TRUE);

	glUseProgram(_program);
}

void Shader_GL3::unbind() {
	glUseProgram(0);
}

GLint Shader_GL3::glGetAttribLoc(StrView name) {
	auto loc = glGetAttribLocation(_program, name.data());
	if (loc < 0) {
		throw SGE_ERROR("cannot find shader attrib variable \"{}\"", name);
	}
	return loc;
}

GLint Shader_GL3::glGetUniformLoc(StrView name) {
	auto loc = glGetUniformLocation(_program, name.data());
	if (loc < 0) {
		throw SGE_ERROR("cannot find shader uniform variable \"{}\"", name);
	}
	return loc;
}

void Shader_GL3::_compileShader(GLuint& shader, GLenum type, StrView filename) {
	if (!shader) {
		shader = glCreateShader(type);
	}
	
	MemMapFile mm;
	mm.open(filename);
	StrView sourceCode = StrView_make(mm.span());
	auto* const data = sourceCode.data();
	GLint dataSize = static_cast<GLint>(sourceCode.size());
	glShaderSource(shader, 1, &data, &dataSize);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		String errmsg;
		_getShaderInfoLog(shader, errmsg);

		TempString tmp;
		FmtTo(tmp, "Error compile shader filename = {}\n{}", filename, errmsg);

		throw SGE_ERROR("{}", tmp.c_str());
	}
}

void Shader_GL3::_getShaderInfoLog(GLuint shader, String& outMsg) {
	outMsg.clear();
	if (!shader) return;

	GLsizei bufLen = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufLen);
	outMsg.resize(bufLen);

	GLsizei outLen = 0;
	glGetShaderInfoLog(shader, bufLen, &outLen, outMsg.begin());

	outMsg.resize(outLen);
}

void Shader_GL3::_linkProgram() {
	if (!_program) {
		_program = glCreateProgram();
	}
	if (_vertexShader) {
		glAttachShader(_program, _vertexShader);
	}
	if (_pixelShader) {
		glAttachShader(_program, _pixelShader);
	}

	glLinkProgram(_program);
	GLint linked;
	glGetProgramiv(_program, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		String errmsg;
		_getProgramInfoLog(_program, errmsg);

		TempString tmp;
		FmtTo(tmp, "Error link shader filename = {}\n{}", _filename, errmsg);

		throw SGE_ERROR("{}", tmp.c_str());
	}

	//----
	glValidateProgram(_program);
	GLint validated;
	glGetProgramiv(_program, GL_VALIDATE_STATUS, &validated);
	if (validated != GL_TRUE) {
		String errmsg;
		_getProgramInfoLog(_program, errmsg);

		TempString tmp;
		FmtTo(tmp, "Error validate shader filename = {}\n{}", _filename, errmsg);

		throw SGE_ERROR("{}", tmp.c_str());
	}
}

void Shader_GL3::_getProgramInfoLog(GLuint program, String& outMsg) {
	outMsg.clear();
	if (!program) return;

	GLsizei bufLen = 0;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLen);
	outMsg.resize(bufLen);

	GLsizei outLen = 0;
	glGetProgramInfoLog(program, bufLen, &outLen, outMsg.begin());

	outMsg.resize(outLen);
}

}
