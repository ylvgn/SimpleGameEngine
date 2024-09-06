#include "Render_GL3_Common.h"

#if SGE_RENDER_HAS_GL3

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

void GL3Util::convert(String& o, VertexSemantic i) {
	using SRC = VertexSemantic;

#define E(SGE_DEFINE, SGE_SEMANTIC) \
		if (i == SGE_SEMANTIC) { o = #SGE_DEFINE; return;} \
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

	throw SGE_ERROR("unknown input VertexLayout_SemanticType {}", i);
}

void GL3Util::convert(VertexSemantic& o, StrView i) {
	using SRC = VertexSemantic;

#define E(SGE_DEFINE, SGE_SEMANTIC) \
		if (i == #SGE_DEFINE) { o = SGE_SEMANTIC; return; } \
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
	throw SGE_ERROR("unknown input StrView {}", i);
}

const char* GL3Util::getGlSemanticName(VertexSemanticType v) {
	const char* s = enumStr(v);
	if (!s) {
		throw SGE_ERROR("unknown VertexLayout_SemanticType {}", v);
	}
	return s;
}

int GL3Util::getComponentCount(RenderDataType v) {
	using SRC = RenderDataType;

	switch (v) {
		case SRC::Int8:
		case SRC::Int16:
		case SRC::Int32:
		case SRC::Int64:
		case SRC::UInt8:
		case SRC::UInt16:
		case SRC::UInt32:
		case SRC::UInt64:
		case SRC::Float16:
		case SRC::Float32:
		case SRC::Float64:
		case SRC::SNorm8:
		case SRC::SNorm16:
		case SRC::SNorm32:
		case SRC::UNorm8:
		case SRC::UNorm16:
		case SRC::UNorm32: return 1;
// ---
		case SRC::Int8x2:
		case SRC::Int16x2:
		case SRC::Int32x2:
		case SRC::Int64x2:
		case SRC::UInt8x2:
		case SRC::UInt16x2:
		case SRC::UInt32x2:
		case SRC::UInt64x2:
		case SRC::Float16x2:
		case SRC::Float32x2:
		case SRC::Float64x2:
		case SRC::SNorm8x2:
		case SRC::SNorm16x2:
		case SRC::SNorm32x2:
		case SRC::UNorm8x2:
		case SRC::UNorm16x2:
		case SRC::UNorm32x2: return 2;
// ---
		case SRC::Int8x3:
		case SRC::Int16x3:
		case SRC::Int32x3:
		case SRC::Int64x3:
		case SRC::UInt8x3:
		case SRC::UInt16x3:
		case SRC::UInt32x3:
		case SRC::UInt64x3:
		case SRC::Float16x3:
		case SRC::Float32x3:
		case SRC::Float64x3:
		case SRC::SNorm8x3:
		case SRC::SNorm16x3:
		case SRC::SNorm32x3:
		case SRC::UNorm8x3:
		case SRC::UNorm16x3:
		case SRC::UNorm32x3: return 3;
// ---
		case SRC::Int8x4:
		case SRC::Int16x4:
		case SRC::Int32x4:
		case SRC::Int64x4:
		case SRC::UInt8x4:
		case SRC::UInt16x4:
		case SRC::UInt32x4:
		case SRC::UInt64x4:
		case SRC::Float16x4:
		case SRC::Float32x4:
		case SRC::Float64x4:
		case SRC::SNorm8x4:
		case SRC::SNorm16x4:
		case SRC::SNorm32x4:
		case SRC::UNorm8x4:
		case SRC::UNorm16x4:
		case SRC::UNorm32x4: return 4;
	// ---
		case SRC::Float32_4x4: return 16;
	// ---
		default: throw SGE_ERROR("unsupported RenderDataType {}", v);
	}
}

void GL3Util::dumpActiveAttrib(GLint program) {
	SGE_ASSERT(program != 0);
	SGE_ASSERT(glIsProgram(program) == GL_TRUE);

	GLint out_maxNameLen;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &out_maxNameLen);

	GLint activeCount;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeCount);

	TempString out_name;
	for (int i = 0; i < activeCount; ++i) {
		GLsizei out_nameLen;
		GLint   out_dataSize;
		GLenum  out_dataType;

		out_name.resize(out_maxNameLen);
		glGetActiveAttrib(program, GLuint(i), out_maxNameLen, &out_nameLen, &out_dataSize, &out_dataType, out_name.data());
		out_name.resize(out_nameLen);

		auto loc = glGetAttribLocation(program, out_name.c_str());
		SGE_ASSERT(loc >= 0);

		SGE_LOG("layout(location = {}) {}", loc, out_name.c_str());

		throwIfError();
	}
}

void GL3Util::dumpActiveUniforms(GLint program) {
	SGE_ASSERT(program != 0);
	SGE_ASSERT(glIsProgram(program) == GL_TRUE,
		"called glUseProgram(program) before call dumpActiveUniforms");

	GLint activeCount;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeCount);

	GLint out_maxNameLen;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &out_maxNameLen);

	TempString out_name;
	for (int i = 0; i < activeCount; ++i) {
		GLsizei out_nameLen;
		GLsizei out_dataSize;
		GLenum  out_dataType;

		out_name.resize(out_maxNameLen);
		glGetActiveUniform(program, GLuint(i), out_maxNameLen, &out_nameLen, &out_dataSize, &out_dataType, out_name.data());
		out_name.resize(out_nameLen);

		auto loc = glGetUniformLocation(program, out_name.c_str());
		if (loc < 0) continue;

		TempString tmp = out_name;
		size_t found = tmp.find('[');
		if (found != TempString::npos) { // uniform array
			tmp.erase(tmp.begin() + found, tmp.end());
			int j = 0;
			while (true) {
				auto uniformName = Fmt("{}[{}]", tmp.c_str(), j); ++j;

				auto elementLoc = glGetUniformLocation(program, uniformName.c_str());
				if (elementLoc < 0)
					break;

				SGE_LOG("uniform {} () (location = {})", uniformName, i, elementLoc);
			}
		} else {
			SGE_LOG("uniform {} ({})", out_name, i);
		}

		throwIfError();
	}
}

void GL3Util::dumpActiveUniformBlocks(GLint program) {
	SGE_ASSERT(program != 0);
	SGE_ASSERT(glIsProgram(program) == GL_TRUE,
		"called glUseProgram(program) before call dumpActiveUniformBlocks");

	GLint out_maxNameLen;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &out_maxNameLen);

	GLint activeCount;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &activeCount);

	TempString out_ubName;
	for (int i = 0; i < activeCount; ++i) {
		GLsizei out_nameLen;

		out_ubName.resize(out_maxNameLen);
		glGetActiveUniformBlockName(program, GLuint(i), out_maxNameLen, &out_nameLen, out_ubName.data());
		out_ubName.resize(out_nameLen);

		TempString	tmp;
		GLint		out;
	#define E(GL_UNIFORM_BLOCK_ENUM) \
		glGetActiveUniformBlockiv(program, GLuint(i), GL_UNIFORM_BLOCK_ENUM, &out); \
		FmtTo(tmp, "\t{}: {}\n", #GL_UNIFORM_BLOCK_ENUM, out); \
	// ----
			E(GL_UNIFORM_BLOCK_BINDING)
			E(GL_UNIFORM_BLOCK_DATA_SIZE)
			E(GL_UNIFORM_BLOCK_NAME_LENGTH)
		//---
			E(GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER)
			E(GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER)
			E(GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER)
			E(GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER)
			E(GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER)
			E(GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER)
	#undef E
		//---
		{
			Vector<GLint> outList;
			glGetActiveUniformBlockiv(program, GLuint(i), GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &out);

			outList.resize(out);
			glGetActiveUniformBlockiv(program, GLuint(i), GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, outList.data());
			FmtTo(tmp, "\t{}: {}\n", "GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS", out);

			TempString tmpIndices;
			for(auto& indices : outList) {
				if (!tmpIndices.empty()) tmpIndices.append(", ");
				FmtTo(tmpIndices, "{}", indices);
			}
			FmtTo(tmp, "\t{}: [{}]\n", "GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES", tmpIndices);
		}

		auto ubIndex = glGetUniformBlockIndex(program, out_ubName.c_str());
		SGE_ASSERT(ubIndex != GL_INVALID_INDEX);

		SGE_LOG("uniform block '{}' ({}):\n{}", out_ubName, ubIndex, tmp);
	}
}

}

#endif // SGE_RENDER_HAS_GL3