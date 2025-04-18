#include "MyShader.h"
#include "MyRender_Common.h"

namespace sge {

class Scoped_MyShader_VertexAttrib : public NonCopyable {
public:
	Scoped_MyShader_VertexAttrib(GLuint program, StrView name, GLint numComponets, GLenum type, bool normalized, GLsizei stride, intptr_t vertexBufferOffset) {
		_loc = glGetAttribLocation(program, name.data());
		if (_loc < 0)
			return;
		glVertexAttribPointer(_loc, numComponets, type, normalized, stride, reinterpret_cast<const void*>(vertexBufferOffset));
		glEnableVertexAttribArray(_loc);
	}

	~Scoped_MyShader_VertexAttrib() {
		if (_loc < 0)
			return;
		glDisableVertexAttribArray(_loc);
	}
private:
	GLint _loc = -1;
};

#if 0
#pragma mark ========= MyVertexArray ============
#endif
void MyVertexArray::create() {
	if (!_p) {
		glGenVertexArrays(1, &_p);
	}
}

void MyVertexArray::destroy() {
	if (_p) {
		glDeleteVertexArrays(1, &_p);
		_p = 0;
	}
}

#if 0
#pragma mark ========= MyShader ============
#endif
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

	SGE_ASSERT(_boundTexCount == 0);
	SGE_GL_CALL(glUseProgram(_program));
}

void MyShader::unbind() {
	for (int i = 0; i < _boundTexCount; ++i) {
		auto& t = _texUnits[i];
		t.sampler.unbind(i);
		t.tex->unbind();
	}

	_boundTexCount = 0;
	SGE_GL_CALL(glUseProgram(0));
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
	SGE_GL_CALL(glUniform1iv(loc, 1, &value));
}

void MyShader::setUniform(StrView name, const u32& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform1uiv(loc, 1, &value));
}

void MyShader::setUniform(StrView name, const float& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform1fv(loc, 1, &value));
}

void MyShader::setUniform(StrView name, const Tuple2i& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform2iv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Tuple3i& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform3iv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Tuple4i& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform4iv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Tuple2f& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform2fv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Tuple3f& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform3fv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Tuple4f& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform4fv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Color4f& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform4fv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Quat4f& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniform4fv(loc, 1, value.data));
}

void MyShader::setUniform(StrView name, const Mat4f& value) {
	auto loc = getUniformLoc(name);
	SGE_GL_CALL(glUniformMatrix4fv(loc, 1, false, value._elements));
}

void MyShader::setUniformCg(StrView name, const Mat4f& value) {
	auto loc = getUniformLoc(name);
	auto mat = value.transpose();
	SGE_GL_CALL(glUniform4fv(loc, 4, mat._elements));
}

void MyShader::setUniform(StrView name, MyTexture2D* value) {
	if (!value) { SGE_ASSERT(false); return; }

	if (_boundTexCount >= kMaxSamplers)
		throw SGE_ERROR("too many samplers");

	auto loc = getUniformLoc(name);	
	SGE_GL_CALL(glUniform1i(loc, _boundTexCount));

	SGE_GL_CALL(glActiveTexture(GL_TEXTURE0 + _boundTexCount));
	value->bind();

	{
		auto& t = _texUnits[_boundTexCount];
		t.sampler.create();
		t.sampler.bind(_boundTexCount);
		t.tex = value;
	}

	_boundTexCount++;
}

GLint MyShader::getAttribLoc(StrView name) {
	auto loc = glGetAttribLocation(_program, name.data());
	if (loc < 0) {
		// If the named attribute variable is not an active attribute in the specified program object or if name starts with the reserved prefix "gl_", a value of - 1 is returned.
		SGE_LOG_ERROR("cannot find shader attrib variable '{}'", name);
	}
	return loc;
}

void MyShader::draw(const MyRenderMesh& mesh) {
	if (!_program)
		return;

	if (!mesh.vertexCount)
		return;

	_vertexArray.create();

	_vertexArray.bind();
	mesh.vertexBuffer.bind();
	mesh.indexBuffer.bind();

	using VertexType = MyRenderMesh::Test_VertexType;

	GLsizei stride = static_cast<GLsizei>(sizeof(VertexType));
	Scoped_MyShader_VertexAttrib v_pos	 (_program, "v_pos",    3, GL_FLOAT,         true, stride, memberOffset(&VertexType::pos));
	Scoped_MyShader_VertexAttrib v_color (_program, "v_color",  4, GL_UNSIGNED_BYTE, true, stride, memberOffset(&VertexType::color));
	Scoped_MyShader_VertexAttrib v_uv	 (_program, "v_uv",     2, GL_FLOAT,         true, stride, memberOffset(&VertexType::uv));
	Scoped_MyShader_VertexAttrib v_normal(_program, "v_normal", 3, GL_FLOAT,         true, stride, memberOffset(&VertexType::normal));

	if (mesh.indexCount > 0) {
		const void* indexBufferOffset = 0;
		SGE_GL_CALL(glDrawElements(my_getGlPrimitiveTopology(mesh.primitive), static_cast<GLsizei>(mesh.indexCount), GL_UNSIGNED_SHORT, indexBufferOffset));
	}
	else {
		SGE_GL_CALL(glDrawArrays(my_getGlPrimitiveTopology(mesh.primitive), 0, static_cast<GLsizei>(mesh.vertexCount)));
	}

	mesh.vertexBuffer.unbind();
	mesh.indexBuffer.unbind();
	_vertexArray.unbind();
}

void MyShader::drawCg(const MyRenderMesh& mesh) {
	if (!_program)
		return;

	if (!mesh.vertexCount)
		return;

	_vertexArray.create();

	_vertexArray.bind();
	mesh.vertexBuffer.bind();
	mesh.indexBuffer.bind();

	using VertexType = MyRenderMesh::TestCg_VertexType;

	GLsizei stride = static_cast<GLsizei>(sizeof(VertexType));
	Scoped_MyShader_VertexAttrib v_pos	 (_program, "cg_Vertex",    4, GL_FLOAT,         true, stride, memberOffset(&VertexType::pos));
	Scoped_MyShader_VertexAttrib v_color (_program, "COLOR",		4, GL_FLOAT,		 true, stride, memberOffset(&VertexType::color));
	Scoped_MyShader_VertexAttrib v_uv	 (_program, "TEXCOORD0",	2, GL_FLOAT,         true, stride, memberOffset(&VertexType::uv));
	Scoped_MyShader_VertexAttrib v_normal(_program, "NORMAL",		4, GL_FLOAT,         true, stride, memberOffset(&VertexType::normal));

	if (mesh.indexCount > 0) {
		const void* indexBufferOffset = 0;
		SGE_GL_CALL(glDrawElements(my_getGlPrimitiveTopology(mesh.primitive), static_cast<GLsizei>(mesh.indexCount), GL_UNSIGNED_SHORT, indexBufferOffset));
	}
	else {
		SGE_GL_CALL(glDrawArrays(my_getGlPrimitiveTopology(mesh.primitive), 0, static_cast<GLsizei>(mesh.vertexCount)));
	}

	_vertexArray.unbind();
	mesh.vertexBuffer.unbind();
	mesh.indexBuffer.unbind();
}

void MyShader::dumpActiveAttrib() {
	GLint activeCount = 0;
	SGE_GL_CALL(glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &activeCount));

	static const size_t kszNameSize = 1024;
	for (int i = 0; i < activeCount; ++i) {
		char out_szName[kszNameSize + 1];
		GLsizei out_len = 0;
		GLint   out_dataSize = 0;
		GLenum  out_dataType = 0;

		SGE_GL_CALL(glGetActiveAttrib(_program, static_cast<GLuint>(i), kszNameSize, &out_len, &out_dataSize, &out_dataType, out_szName));
		out_szName[kszNameSize] = 0;

		auto loc = glGetAttribLocation(_program, out_szName);
		SGE_LOG("{}: {}", out_szName, loc);
	}
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
	SGE_GL_CALL(glShaderSource(shader, 1, &data, &dataSize));
	SGE_GL_CALL(glCompileShader(shader));

	GLint compiled;
	SGE_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
	if (compiled != GL_TRUE) {
		String errmsg;
		_getShaderInfoLog(shader, errmsg);

		TempString tmp;
		FmtTo(tmp, "Error compile shader filename = {}\n{}", filename, errmsg);

		throw SGE_ERROR("{}", tmp.c_str());
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

void MySampler::create() {
	if (!_p) {
		glGenSamplers(1, &_p);

		glSamplerParameterf(_p, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameterf(_p, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameterf(_p, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glSamplerParameterf(_p, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameterf(_p, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void MySampler::destroy() {
	if (_p) {
		glDeleteSamplers(1, &_p);
		_p = 0;
	}
}

}
