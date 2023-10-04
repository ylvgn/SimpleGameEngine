#include "MyCommon.h"

namespace sge {

void AnimationAttribLocation::setBySkinnedShader(const Shader* const shader) {
	pos		= shader->findAttributeByName("position");
	normal	= shader->findAttributeByName("normal");
	uv		= shader->findAttributeByName("texCoord");
	weights = shader->findAttributeByName("weights");
	joints	= shader->findAttributeByName("joints");
}

void AnimationAttribLocation::setByStaticShader(const Shader* const shader) {
	pos		= shader->findAttributeByName("position");
	normal	= shader->findAttributeByName("normal");
	uv		= shader->findAttributeByName("texCoord");
	weights = Mesh::kInvalidSlotIndex;
	joints  = Mesh::kInvalidSlotIndex;
}

}