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

void AnimationAttribLocation::bindAttribAndDraw(Mesh& mesh) const {
	mesh.bind(pos, normal, uv, weights, joints);
	mesh.draw();
	mesh.unbind(pos, normal, uv, weights, joints);
}

void AnimationAttribLocation::bindAttribAndDrawInstanced(Mesh& mesh, size_t instanceCount) const {
	mesh.bind(pos, normal, uv, weights, joints);
	mesh.drawInstanced(instanceCount);
	mesh.unbind(pos, normal, uv, weights, joints);
}

}