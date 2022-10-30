#pragma once

#include "CRenderer.h"

namespace sge {

class CMeshRenderer : public CRenderer {
	SGE_OBJECT_TYPE(CMeshRenderer, CRenderer)
public:
	void setMesh(StrView filename);
	void setMesh(const EditMesh& editMesh);
	void setMaterial(Material* material);

protected:
	virtual void onRender(RenderRequest& req) override;
	virtual const OBB onGetOBB() const override;

	RenderMesh _renderMesh;
	SPtr<Material> _material;
	AABB _aabb;
};

}