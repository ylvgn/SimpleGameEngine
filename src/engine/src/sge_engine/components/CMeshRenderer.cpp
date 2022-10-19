#include "CMeshRenderer.h"

namespace sge {

const TypeInfo* CMeshRenderer::s_getType() {
	class Ti : public CMeshRenderer::TI_Base {
	public:
		Ti() {
			static FieldInfo fi[] = {
				FieldInfo("renderMesh", &This::_renderMesh),
				//FieldInfo("material",	&This::_material),
			};
			setFields(fi);
		}
	};
	static Ti ti;
	return &ti;
}

void CMeshRenderer::setMesh(StrView filename) {
	EditMesh editMesh;
	WavefrontObjLoader::readFile(editMesh, filename);
	for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
		editMesh.color.emplace_back(255, 255, 255, 255);
	}
	setMesh(editMesh);
}

void CMeshRenderer::setMesh(EditMesh& mesh)
{
	_renderMesh.create(mesh);
}

void CMeshRenderer::setMaterial(Material* material) {
	_material.reset(material);
}

void CMeshRenderer::onRender(RenderRequest& req) {
	req.drawMesh(SGE_LOC, _renderMesh, _material);
}

}