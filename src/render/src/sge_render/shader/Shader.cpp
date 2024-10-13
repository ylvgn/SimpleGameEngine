#include "Shader.h"
#include <sge_render/Renderer.h>

namespace sge {
#if 0
#pragma mark ========= ShaderPass ============
#endif
ShaderPass::ShaderPass(Shader* shader, int passIndex) noexcept
	: _shader(shader)
	, _passIndex(passIndex)
	, _info(shader->passInfo(passIndex))
{
}

StrView ShaderPass::shaderFilename() const {
	SGE_ASSERT(_shader != nullptr);
	return _shader ? _shader->filename().c_str() : StrView();
}

#if 0
#pragma mark ========= Shader ============
#endif
Shader::Shader(StrView filename) noexcept : _filename(filename) {
}

void Shader::_internal_init() {
	auto* proj = ProjectSettings::instance();
	auto infoFilename = Fmt("{}/{}/info.json", proj->importedPath(), _filename);
	JsonUtil::readFile(infoFilename, _info);

	_passes.clear();
	auto n = passCount();
	_passes.reserve(n);
	for (int i = 0; i < n; ++i) {
		UPtr<ShaderPass> pass = onCreateShaderPass(i);
		pass->onInit();
		_passes.emplace_back(SGE_MOVE(pass));
	}
}

Shader::~Shader() {
	auto* renderer = Renderer::instance();
	renderer->onShaderDestory(this);
}

} // namespace sge