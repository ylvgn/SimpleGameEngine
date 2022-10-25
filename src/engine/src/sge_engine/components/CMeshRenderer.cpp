#include "CMeshRenderer.h"
#include "CTransform.h"
#include "../Entity.h"

namespace sge {

const TypeInfo* CMeshRenderer::s_getType() {
	class Ti : public CMeshRenderer::TI_Base {
	public:
		Ti() {
			//static FieldInfo fi[] = {
			//	FieldInfo("renderMesh", &This::_renderMesh),
			//	FieldInfo("material",	&This::_material),
			//};
			//setFields(fi);
		}
	};
	static Ti ti;
	return &ti;
}

void CMeshRenderer::setMesh(StrView filename) {
	EditMesh editMesh;
	WavefrontObjLoader::readFile(editMesh, filename); // tmp
	for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
		editMesh.color.emplace_back(255, 255, 255, 255);
	}
	setMesh(editMesh);
}

void CMeshRenderer::setMesh(const EditMesh& mesh) {
	_renderMesh.create(mesh);
}

void CMeshRenderer::setMaterial(Material* material) {
	_material.reset(material);
}

void CMeshRenderer::onRender(RenderRequest& req) {
	if (!_material) { SGE_ASSERT(false); return; }

	// tmp
	auto* t = _entity->transform();
	req.setMaterialCommonParams(_material);

	auto mvp = req.matrix_vp * t->worldMatrix();
	_material->setParam("sge_matrix_mvp", mvp);
	_material->setParam("test_color", Color4f(1, 1, 1, 1));

	for (auto& subMesh : _renderMesh.subMeshes()) {
		auto passes = _material->passes();
		for (size_t i = 0; i < passes.size(); i++) {
			auto* cmd = req.addDrawCall();
#if _DEBUG
			cmd->debugLoc = SGE_LOC;
#endif
			cmd->material			= _material;
			cmd->materialPassIndex	= i;
			cmd->primitive			= subMesh.primitive();
			cmd->vertexLayout		= subMesh.vertexLayout();
			cmd->vertexBuffer		= subMesh.vertexBuffer();
			cmd->vertexCount		= subMesh.vertexCount();
			cmd->indexBuffer		= subMesh.indexBuffer();
			cmd->indexType			= subMesh.indexType();
			cmd->indexCount			= subMesh.indexCount();
		}
	}
}

}