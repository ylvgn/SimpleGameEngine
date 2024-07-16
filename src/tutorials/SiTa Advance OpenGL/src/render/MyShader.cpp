#include "MyShader.h"
#include "MyRender_Common.h"

namespace sge {

void MyShader::loadFile(StrView filename) {
	_filename = filename;
	reload();
}

void MyShader::destroy() {
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

void MyShader::reload() {
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

void MyShader::bind() {
	if (!_program)
		SGE_ASSERT(glIsProgram(_program) == GL_TRUE);

	glUseProgram(_program);
}

void MyShader::unbind() {
	glUseProgram(0);
}

GLint MyShader::getUniformLoc(StrView name) {
	auto loc = glGetUniformLocation(_program, name.data());
	if (loc < 0) {
		// This function returns -1 if name does not correspond to an active uniform variable in program
		SGE_LOG_ERROR("cannot find shader uniform variable '{}'", name);
	}
	return loc;
}

void MyShader::setUniform(StrView name, const i32& value) {
	// If location is equal to - 1, the data passed in will be silently ignored and the specified uniform variable will not be changed.
	auto loc = getUniformLoc(name);
	glUniform1iv(loc, 1, &value);
}

void MyShader::setUniform(StrView name, const u32& value) {
	auto loc = getUniformLoc(name);
	glUniform1uiv(loc, 1, &value);
}

void MyShader::setUniform(StrView name, const float& value) {
	auto loc = getUniformLoc(name);
	glUniform1fv(loc, 1, &value);
}

void MyShader::setUniform(StrView name, const Tuple2i& value) {
	auto loc = getUniformLoc(name);
	glUniform2iv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Tuple3i& value) {
	auto loc = getUniformLoc(name);
	glUniform3iv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Tuple4i& value) {
	auto loc = getUniformLoc(name);
	glUniform4iv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Tuple2f& value) {
	auto loc = getUniformLoc(name);
	glUniform2fv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Tuple3f& value) {
	auto loc = getUniformLoc(name);
	glUniform3fv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Tuple4f& value) {
	auto loc = getUniformLoc(name);
	glUniform4fv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Color4f& value) {
	auto loc = getUniformLoc(name);
	glUniform4fv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Quat4f& value) {
	auto loc = getUniformLoc(name);
	glUniform4fv(loc, 1, value.data);
}

void MyShader::setUniform(StrView name, const Mat4f& value) {
	auto loc = getUniformLoc(name);
	glUniformMatrix4fv(loc, 1, false, value._elements);
}

GLint MyShader::getAttribLoc(StrView name) {
	auto loc = glGetAttribLocation(_program, name.data());
	if (loc < 0) {
		// If the named attribute variable is not an active attribute in the specified program object or if name starts with the reserved prefix "gl_", a value of - 1 is returned.
		SGE_LOG_ERROR("cannot find shader attrib variable '{}'", name);
	}
	return loc;
}

void MyShader::_compileShader(GLuint& shader, GLenum type, StrView filename) {
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

		throw SGE_ERROR(tmp.c_str());
	}
}

void MyShader::_getShaderInfoLog(GLuint shader, String& outMsg) {
	outMsg.clear();
	if (!shader) return;

	GLsizei bufLen = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufLen);
	outMsg.resize(bufLen);

	GLsizei outLen = 0;
	glGetShaderInfoLog(shader, bufLen, &outLen, outMsg.begin());

	outMsg.resize(outLen);
}

void MyShader::_linkProgram() {
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

		throw SGE_ERROR(tmp.c_str());
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

		throw SGE_ERROR(tmp.c_str());
	}
}

void MyShader::_getProgramInfoLog(GLuint program, String& outMsg) {
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
