#include "Render_GL3_Common.h"

namespace sge {

void GL3Util::compileShader(GLuint& shader, GLenum type, StrView filename) {
	if (!shader) {
		shader = glCreateShader(type);
	}
	
	MemMapFile mm;
	mm.open(filename);
	StrView sourceCode	= StrView_make(mm.span());
	auto* const data	= sourceCode.data();
	GLint dataSize		= static_cast<GLint>(sourceCode.size());

	glShaderSource(shader, 1, &data, &dataSize);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		String errmsg;
		getShaderInfoLog(shader, errmsg);

		TempString tmp;
		FmtTo(tmp, "Error compile shader filename = {}\n{}", filename, errmsg);

		throw SGE_ERROR("{}", tmp.c_str());
	}
}

void GL3Util::getShaderInfoLog(GLuint shader, String& outMsg) {
	outMsg.clear();
	if (!shader) return;

	GLsizei bufLen = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufLen);
	outMsg.resize(bufLen);

	GLsizei outLen = 0;
	glGetShaderInfoLog(shader, bufLen, &outLen, outMsg.begin());

	outMsg.resize(outLen);
}

void GL3Util::getProgramInfoLog(GLuint program, String& outMsg) {
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