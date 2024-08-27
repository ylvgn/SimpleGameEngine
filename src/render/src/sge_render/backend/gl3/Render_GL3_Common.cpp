#include "Render_GL3_Common.h"

namespace sge {

void GL3Util::compileShader(GLuint& shader, GLenum type, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	compileShader(shader, type, mm, filename);
}

void GL3Util::compileShader(GLuint& shader, GLenum type, ByteSpan sourceCode, StrView filename) {
	if (!shader) {
		shader = glCreateShader(type);
		throwIfError();
	}

	StrView source		= StrView_make(sourceCode);
	auto* const data	= source.data();
	GLint dataSize		= static_cast<GLint>(source.size());

	glShaderSource(shader, 1, &data, &dataSize);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		String errmsg;
		getShaderInfoLog(shader, errmsg);

		throw SGE_ERROR("Error compile shader: {}\nfilename: {}",
			errmsg.c_str(), filename.empty() ? "Unknown" : filename);
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

VertexSemantic GL3Util::parseGlSemanticName(StrView s) {
	using SRC = VertexSemantic;

#define E(SGE_DEFINE, SGE_SEMANTIC) \
		if (s == #SGE_DEFINE) return SGE_SEMANTIC; \
	// -----
		E(positionOS,	SRC::POSITION)
		E(positionHCS,	SRC::POSITION)
		E(positionWS,	SRC::TEXCOORD10)
		E(positionVS,	SRC::TEXCOORD11)
		E(positionCS,	SRC::TEXCOORD12)
		E(positionSS,	SRC::TEXCOORD13)

		E(color,		SRC::COLOR0)
		E(color1,		SRC::COLOR1)
		E(color2,		SRC::COLOR2)
		E(color3,		SRC::COLOR3)

		E(uv,			SRC::TEXCOORD0)
		E(uv1,			SRC::TEXCOORD1)
		E(uv2,			SRC::TEXCOORD2)
		E(uv3,			SRC::TEXCOORD3)
		E(uv4,			SRC::TEXCOORD4)
		E(uv5,			SRC::TEXCOORD5)
		E(uv6,			SRC::TEXCOORD6)
		E(uv7,			SRC::TEXCOORD7)
		E(uv8,			SRC::TEXCOORD8)

		E(normal,		SRC::NORMAL)
		E(tangent,		SRC::TANGENT)
		E(binormal,		SRC::BINORMAL)
#undef E
// -----

	throw SGE_ERROR("unknown VertexLayout_SemanticType {}", s);
}

void GL3Util::dumpActiveAttrib(GLint program) {
	GLint activeCount;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeCount);

	static const size_t kszNameSize = 4096;
	for (int i = 0; i < activeCount; ++i) {
		char out_szName[kszNameSize + 1];
		GLsizei out_len = 0;
		GLint   out_dataSize = 0;
		GLenum  out_dataType = 0;

		glGetActiveAttrib(program, static_cast<GLuint>(i), kszNameSize, &out_len, &out_dataSize, &out_dataType, out_szName);
		out_szName[kszNameSize] = 0; // ensure terminate with 0

		auto loc = glGetAttribLocation(program, out_szName);
		SGE_LOG("layout(location = {}) {}", loc, out_szName);
	}
}

}