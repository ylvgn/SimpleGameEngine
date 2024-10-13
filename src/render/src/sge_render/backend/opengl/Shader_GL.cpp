#include "Shader_GL.h"

#if SGE_RENDER_HAS_OPENGL

namespace sge {
#if 0
#pragma mark ========= Shader_GL ============
#endif

sgeShader_InterfaceFunctions_Impl(GL);

Shader_GL::Shader_GL(StrView filename) : Base(filename) {
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

Shader_GL::MyPass::MyPass(Shader* shader, int passIndex) noexcept
	: Base(shader, passIndex)
{
	_vertexStage._pass = this;
	 _pixelStage._pass = this;

	Base::_vertexStage = &_vertexStage;
	 Base::_pixelStage = &_pixelStage;
}

void Shader_GL::MyPass::onInit() {
	auto* proj = ProjectSettings::instance();
	TempString passPath;
	FmtTo(passPath, "{}/{}/glsl/pass{}", proj->importedPath(), shaderFilename(), _passIndex);

	if (!_info->vsFunc.empty()) {
		TempString tmp;
		tmp = Fmt("{}/vs_{}.spv.glsl", passPath, Util::getGlStageProfile(ShaderStageMask::Vertex));
		Util::compileShader(_vertexStage._shader, GL_VERTEX_SHADER, tmp);

		tmp += ".json";
		JsonUtil::readFile(tmp, _vertexStage._info);
	}

	if (!_info->psFunc.empty()) {
		TempString tmp;
		tmp = Fmt("{}/ps_{}.spv.glsl", passPath, Util::getGlStageProfile(ShaderStageMask::Pixel));
		Util::compileShader(_pixelStage._shader, GL_FRAGMENT_SHADER, tmp);

		tmp += ".json";
		JsonUtil::readFile(tmp, _pixelStage._info);
	}

	if (!_program)
		Util::linkShader(_program, _vertexStage._shader, _pixelStage._shader);
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
#pragma mark ========= Shader_GL::MyPixelStage ============
#endif
Shader_GL::MyPixelStage::~MyPixelStage() noexcept {
	if (_shader) {
		glDeleteShader(_shader);
		_shader = 0;
	}
}

#if 0
#pragma mark ========= Shader_GL::MyVertexStage ============
#endif
Shader_GL::MyVertexStage::~MyVertexStage() noexcept {
	if (_shader) {
		glDeleteShader(_shader);
		_shader = 0;
	}
}

} // namespace sge

#endif // SGE_RENDER_HAS_OPENGL