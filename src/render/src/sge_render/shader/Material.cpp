#include "Material.h"

namespace sge {

void Material::setShader(Shader* shader)
{
	if (_shader == shader) return;

	_passes.reserve(shader->passes().size());
	for (auto& shaderPass : shader->passes()) {
		UPtr<Pass> pass = onCreatePass(shaderPass.get());
		_passes.emplace_back(std::move(pass));
	}

	onSetShader();
}

MaterialPass_Stage::MaterialPass_Stage(MaterialPass* pass, ShaderStage* shaderStage)
	: _pass(pass)
	, _shaderStage(shaderStage)
{

}

} // namespace
