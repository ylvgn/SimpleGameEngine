#pragma once

#include "CRenderer.h"

namespace sge {

class CMeshRenderer : public CRenderer {
	SGE_OBJECT_TYPE(CMeshRenderer, CRenderer)
public:
	void setMesh(StrView filename);
	void setMesh(const EditMesh& mesh);
	void setMaterial(Material* material);

protected:
	virtual void onRender(RenderRequest& req) override;

	RenderMesh _renderMesh;
	SPtr<Material> _material;
};

}