#include "Shader_GL.h"

namespace sge {

Shader_GL::Shader_GL(StrView filename)
	: Base(filename)
{
	auto* proj = ProjectSettings::instance();
	TempString passPath;

	_passes.clear();
	_passes.reserve(_info.passes.size());

	int i = 0;
	for (auto& info : _info.passes) {
		FmtTo(passPath, "{}/{}/glsl/pass{}", proj->importedPath(), filename, i);
		UPtr<Pass> pass = eastl::make_unique<MyPass>(this, passPath, info);
		_passes.emplace_back(std::move(pass));
		++i;
	}
}

void Shader_GL::_loadStageFile(StrView passPath, ShaderStageMask stageMask, Vector<u8>& outBytecode, ShaderStageInfo& outInfo) {
	auto* profile = Util::getGlStageProfile(stageMask);

	TempString filename;
	switch (stageMask)
	{
		case ShaderStageMask::Vertex:
			filename = Fmt("{}/vs_{}.spv.glsl", passPath, profile);
			break;
		case ShaderStageMask::Pixel:
			filename = Fmt("{}/ps_{}.spv.glsl", passPath, profile);
			break;
		default: throw SGE_ERROR("unsupported ShaderStageMask {}", stageMask);
	}

	File::readFile(filename, outBytecode);

	filename += ".json";
	JsonUtil::readFile(filename, outInfo);
}

Shader_GL::MyPass::MyPass(Shader_GL* shader, StrView passPath, ShaderInfo::Pass& info)
	: Base(shader, info)
{
	_vertexStage = &_myVertexStage;
	 _pixelStage = &_myPixelStage;

	if (info.vsFunc.size()) { _myVertexStage.load(this, passPath); }
	if (info.psFunc.size()) {  _myPixelStage.load(this, passPath); }

	_linkProgram();
}

void Shader_GL::MyVertexStage::destroy() {
	if (_shader) {
		glDeleteShader(_shader);
		_shader = 0;
	}
}

void Shader_GL::MyPixelStage::destroy() {
	if (_shader) {
		glDeleteShader(_shader);
		_shader = 0;
	}
}

void Shader_GL::MyVertexStage::load(MyPass* pass, StrView passPath) {
	_loadStageFile(passPath, stageMask(), _bytecode, _info);

	Util::compileShader(_shader, Util::getGlShaderType(stageMask()), _bytecode, passPath);
	Util::throwIfError();
}

void Shader_GL::MyPixelStage::load(MyPass* pass, StrView passPath) {
	_loadStageFile(passPath, stageMask(), _bytecode, _info);

	Util::compileShader(_shader, Util::getGlShaderType(stageMask()), _bytecode, passPath);
	Util::throwIfError();
}

void Shader_GL::MyVertexStage::bind(RenderContext_GL* ctx) {
	if (!_shader)
		throw SGE_ERROR("gl shader is null");
}

void Shader_GL::MyPixelStage::bind(RenderContext_GL* ctx) {
	if (!_shader)
		throw SGE_ERROR("gl shader is null");
}

void Shader_GL::MyPass::destroy() {
	if (_myProgram) {
		glDeleteProgram(_myProgram);
		_myProgram = 0;
	}
}

void Shader_GL::MyPass::bind() {
	if (!_myProgram)
		SGE_ASSERT(glIsProgram(_myProgram) == GL_TRUE);

	glUseProgram(_myProgram);
	Util::throwIfError();
}

void Shader_GL::MyPass::unbind() {
	glUseProgram(0);
}

void Shader_GL::MyPass::_linkProgram() {
	if (!_myProgram) {
		_myProgram = glCreateProgram();
	}
	if (_myVertexStage._shader) {
		glAttachShader(_myProgram, _myVertexStage._shader);
	}
	if (_myPixelStage._shader) {
		glAttachShader(_myProgram, _myPixelStage._shader);
	}
	Util::throwIfError();

	glLinkProgram(_myProgram);
	GLint linked;
	glGetProgramiv(_myProgram, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		String errmsg;
		_getProgramInfoLog(_myProgram, errmsg);

		TempString tmp;
		FmtTo(tmp, "Error link shader filename = {}\n{}", _shader->info()->name.c_str(), errmsg);

		throw SGE_ERROR("{}", tmp.c_str());
	}

	//----
	glValidateProgram(_myProgram);
	GLint validated;
	glGetProgramiv(_myProgram, GL_VALIDATE_STATUS, &validated);
	if (validated != GL_TRUE) {
		String errmsg;
		_getProgramInfoLog(_myProgram, errmsg);

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

}
