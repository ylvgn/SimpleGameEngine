#include "Shader_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {
#if 0
#pragma mark ========= Shader_GL::MyVertexStage ============
#endif
Shader_GL::MyVertexStage::~MyVertexStage() noexcept {
	if (_handle) {
		glDeleteShader(_handle);
		_handle = 0;
	}
}

void Shader_GL::MyVertexStage::load(StrView passPath) {
	s_loadStageFile(passPath, Profile::GLSL_VS, shaderStageType(), _handle, _info);
}

#if 0
#pragma mark ========= Shader_GL::MyPixelStage ============
#endif
Shader_GL::MyPixelStage::~MyPixelStage() noexcept {
	if (_handle) {
		glDeleteShader(_handle);
		_handle = 0;
	}
}

void Shader_GL::MyPixelStage::load(StrView passPath) {
	s_loadStageFile(passPath, Profile::GLSL_PS, shaderStageType(), _handle, _info);
}

#if 0
#pragma mark ========= Shader_GL::MyPass ============
#endif
Shader_GL::MyPass::~MyPass() noexcept {
	if (_program) {
		glDeleteProgram(_program);
		_program = 0;
	}
}

Shader_GL::MyPass::MyPass(Shader_GL* shader, int passIndex) noexcept
	: Base(shader, passIndex)
	, _vertexStage(this)
	 , _pixelStage(this)
{
	Base::_vertexStage = &_vertexStage;
	 Base::_pixelStage = &_pixelStage;
}

void Shader_GL::MyPass::onInit() {
	auto* proj = ProjectSettings::instance();

	TempString passPath;
	FmtTo(passPath, "{}/{}/glsl/pass{}", proj->importedPath(), shaderFilename(), _passIndex);

	if (!_info->vsFunc.empty()) { _vertexStage.load(passPath); }
	if (!_info->psFunc.empty()) {  _pixelStage.load(passPath); }

	if (!_program)
		Util::linkShader(_program, _vertexStage._handle, _pixelStage._handle);
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

#if 0
#pragma mark ========= Shader_GL ============
#endif

sgeShader_InterfaceFunctions_Impl(GL);

void Shader_GL::s_loadStageFile(StrView passPath, StrView profile, GLenum shaderStageType, GLuint& handle, ShaderStageInfo& outInfo) {
	TempString tmp = Fmt("{}/{}.glsl", passPath, profile);
	Util::compileShader(handle, shaderStageType, tmp);

	tmp += ".json";
	JsonUtil::readFile(tmp, outInfo);
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL