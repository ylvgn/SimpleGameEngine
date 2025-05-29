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
#pragma mark ========= Shader_GL::MyComputeStage ============
#endif
Shader_GL::MyComputeStage::~MyComputeStage() noexcept {
	if (_handle) {
		glDeleteShader(_handle);
		_handle = 0;
	}
}

void Shader_GL::MyComputeStage::load(StrView passPath) {
	s_loadStageFile(passPath, Profile::GLSL_CS, shaderStageType(), _handle, _info);
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
	, _myVertexStage(this)
	, _myPixelStage(this)
	, _myComputeStage(this)
{
	_vertexStage	= &_myVertexStage;
	_pixelStage		= &_myPixelStage;
	_computeStage	= &_myComputeStage;
}

void Shader_GL::MyPass::onInit() {
	auto* proj = ProjectSettings::instance();

	TempString passPath;
	FmtTo(passPath, "{}/{}/glsl/pass{}", proj->importedPath(), shaderFilename(), _passIndex);

	if (hasVS()) { _myVertexStage.load(passPath); }
	if (hasPS()) { _myPixelStage.load(passPath); }
	if (hasCS()) { _myComputeStage.load(passPath); }

	_linkProgram();
}

void Shader_GL::MyPass::_linkProgram() {
	if (!_program) {
		_program = glCreateProgram();
	}

	TempString filename;
	FmtTo(filename, "{}-pass{}", shader()->filename(), passIndex());

	if (hasVS()) {
		glAttachShader(_program, _myVertexStage._handle);
		Util::throwIfError();
	}

	if (hasPS()) {
		glAttachShader(_program, _myPixelStage._handle);
		Util::throwIfError();
	}

	if (hasCS()) {
		glAttachShader(_program, _myComputeStage._handle);
		Util::throwIfError();
	}

	{
		glLinkProgram(_program);
		GLint linked;
		glGetProgramiv(_program, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE) {
			String errorMsg;
			Util::getProgramInfoLog(_program, errorMsg);

			TempString tmp;
			FmtTo(tmp, "Error: link shader\n  filename={}\n{}", filename, errorMsg);

			throw SGE_ERROR("{}", tmp.c_str());
		}
	}

	{
		glValidateProgram(_program);
		Util::throwIfError();

		GLint validated;
		glGetProgramiv(_program, GL_VALIDATE_STATUS, &validated);
		if (validated != GL_TRUE) {
			String errorMsg;
			Util::getProgramInfoLog(_program, errorMsg);

			TempString tmp;
			FmtTo(tmp, "Error: validated shader\n\tfilename={}\n\t{}", filename, errorMsg);

			throw SGE_ERROR("{}", tmp.c_str());
		}
		SGE_ASSERT(glIsProgram(_program) == GL_TRUE);
	}
}

void Shader_GL::MyPass::bind() {
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