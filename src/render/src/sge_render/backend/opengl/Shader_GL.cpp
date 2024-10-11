#include "Shader_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {
#if 0
#pragma mark ========= Shader_GL ============
#endif
Shader_GL::Shader_GL(StrView filename)
	: Base(filename)
{
}

UPtr<ShaderPass> Shader_GL::onCreateShaderPass(Shader* shader, int passIndex) {
	return UPtr_make<MyPass>(static_cast<Shader_GL*>(shader), passIndex);
}

#if 0
#pragma mark ========= Shader_GL::MyPass ============
#endif
Shader_GL::MyPass::~MyPass() {
	if (_program) {
		glDeleteProgram(_program);
		_program = 0;
	}
}

Shader_GL::MyPass::MyPass(Shader* shader, int passIndex) noexcept
	: Base(shader, passIndex)
{
	_vertexStage._pass = this;
	 _pixelStage._pass = this;

	Base::_vertexStage = &_vertexStage;
	 Base::_pixelStage = &_pixelStage;
}

void Shader_GL::MyPass::onInit() {
	// TODO
	TempString profile;
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	FmtTo(profile, "{}{}0", major, minor);
	SGE_DUMP_VAR(profile);

	auto* proj = ProjectSettings::instance();
	TempString passPath;
	FmtTo(passPath, "{}/{}/glsl/pass{}", proj->importedPath(), shaderFilename(), _passIndex);

	if (!_info->vsFunc.empty()) {
		TempString tmp;
		tmp = Fmt("{}/vs_{}.spv.glsl", passPath, profile);
		s_compileStage(_vertexStage._shader, GL_VERTEX_SHADER, tmp);

		tmp += ".json";
		JsonUtil::readFile(tmp, _vertexStage._info);
	}

	if (!_info->psFunc.empty()) {
		TempString tmp;
		tmp = Fmt("{}/ps_{}.spv.glsl", passPath, profile);
		s_compileStage(_pixelStage._shader, GL_FRAGMENT_SHADER, tmp);

		tmp += ".json";
		JsonUtil::readFile(tmp, _pixelStage._info);
	}

	_linkProgram();
}

void Shader_GL::MyPass::s_compileStage(GLuint& shader, GLenum type, StrView filename) {
	MemMapFile mm;
	mm.open(filename);

	if (!shader) {
		shader = glCreateShader(type);
		Util::throwIfError();
	}

	StrView sourceCode  = StrView_make(mm.span());
	auto* const srcData = sourceCode.data();
	GLint srcLen		= static_cast<GLint>(sourceCode.size());

	glShaderSource(shader, 1, &srcData, &srcLen);
	glCompileShader(shader);
	Util::throwIfError();

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		String errmsg;
		s_getShaderInfoLog(shader, errmsg);

		throw SGE_ERROR("Error compile shader: {}\nfilename: {}", errmsg.c_str(), filename);
	}
}

void Shader_GL::MyPass::s_getShaderInfoLog(GLuint shader, String& outMsg) {
	outMsg.clear();
	if (!shader) return;

	GLsizei bufLen = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufLen);
	outMsg.resize(bufLen);

	GLsizei outLen = 0;
	glGetShaderInfoLog(shader, bufLen, &outLen, outMsg.begin());

	outMsg.resize(outLen);
}

void Shader_GL::MyPass::bind() {
	if (!_program)
		SGE_ASSERT(glIsProgram(_program) == GL_TRUE);

	glUseProgram(_program);
	Util::throwIfError();
}

void Shader_GL::MyPass::unbind() {
	glUseProgram(0);
}

void Shader_GL::MyPass::_linkProgram() {
	if (!_program) {
		_program = glCreateProgram();
	}
	if (_vertexStage._shader) {
		glAttachShader(_program, _vertexStage._shader);
	}
	if (_pixelStage._shader) {
		glAttachShader(_program, _pixelStage._shader);
	}
	Util::throwIfError();

	glLinkProgram(_program);
	GLint linked;
	glGetProgramiv(_program, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		String errmsg;
		_getProgramInfoLog(_program, errmsg);

		TempString tmp;
		FmtTo(tmp, "Error link shader filename = {}\n{}", _shader->info()->name.c_str(), errmsg);

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
		FmtTo(tmp, "Error validate shader filename = {}\n{}", _shader->info()->name.c_str(), errmsg);

		throw SGE_ERROR("{}", tmp.c_str());
	}
}

void Shader_GL::MyPass::_getProgramInfoLog(GLuint program, String& outMsg) {
	outMsg.clear();
	if (!program) return;

	GLsizei bufLen = 0;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLen);
	outMsg.resize(bufLen);

	GLsizei outLen = 0;
	glGetProgramInfoLog(program, bufLen, &outLen, outMsg.begin());

	outMsg.resize(outLen);
}

#if 0
#pragma mark ========= Shader_GL::MyPixelStage ============
#endif

Shader_GL::MyPixelStage::~MyPixelStage() {
	if (_shader) {
		glDeleteShader(_shader);
		_shader = 0;
	}
}

void Shader_GL::MyPixelStage::bind(RenderContext_GL* ctx) {
	if (!_shader)
		throw SGE_ERROR("gl shader is null");
}

#if 0
#pragma mark ========= Shader_GL::MyVertexStage ============
#endif
Shader_GL::MyVertexStage::~MyVertexStage() {
	if (_shader) {
		glDeleteShader(_shader);
		_shader = 0;
	}
}

void Shader_GL::MyVertexStage::bind(RenderContext_GL* ctx) {
	if (!_shader)
		throw SGE_ERROR("gl shader is null");
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL