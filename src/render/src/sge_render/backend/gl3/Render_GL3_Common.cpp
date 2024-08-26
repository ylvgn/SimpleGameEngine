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

VertexSemantic GL3Util::parseGlSemanticName(StrView vkName) {
	auto pair = StringUtil::splitByChar(vkName, '_');

	StrView defineName = pair.second.empty() ? pair.first : pair.second;

#define E(DEFINE, OUTPUT) \
	if (defineName == #DEFINE) return OUTPUT; \
// -----
		E(positionOS,	VertexSemantic::POSITION)
		E(positionHCS,	VertexSemantic::POSITION)
		E(positionWS,	VertexSemantic::TEXCOORD10)
		E(positionVS,	VertexSemantic::TEXCOORD11)
		E(positionCS,	VertexSemantic::TEXCOORD12)
		E(positionSS,	VertexSemantic::TEXCOORD13)

		E(color,		VertexSemantic::COLOR0)
		E(color1,		VertexSemantic::COLOR1)
		E(color2,		VertexSemantic::COLOR2)
		E(color3,		VertexSemantic::COLOR3)

		E(uv,			VertexSemantic::TEXCOORD0)
		E(uv1,			VertexSemantic::TEXCOORD1)
		E(uv2,			VertexSemantic::TEXCOORD2)
		E(uv3,			VertexSemantic::TEXCOORD3)
		E(uv4,			VertexSemantic::TEXCOORD4)
		E(uv5,			VertexSemantic::TEXCOORD5)
		E(uv6,			VertexSemantic::TEXCOORD6)
		E(uv7,			VertexSemantic::TEXCOORD7)
		E(uv8,			VertexSemantic::TEXCOORD8)

		E(normal,		VertexSemantic::NORMAL)
		E(tangent,		VertexSemantic::TANGENT)
		E(binormal,		VertexSemantic::BINORMAL)
#undef E
// -----

	throw SGE_ERROR("unknown VertexLayout_SemanticType {}", vkName);
}

}