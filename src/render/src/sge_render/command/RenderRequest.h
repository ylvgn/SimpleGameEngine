#pragma once
#include "RenderCommand.h"
#include <sge_render/mesh/RenderMesh.h>

namespace sge {

class RenderContext;

class RenderRequest : public Object {
public:
//	Mat4f			matrix_model;
	Mat4f			matrix_view;
	Mat4f			matrix_proj;

	Vec3f			cameraPos;
	Math::Frustum3f cameraFrustum;

	Vec3f			light_pos;
	Vec3f			light_dir;
	float			light_power;
	Vec3f			light_color;

	RenderCommandBuffer	commandBuffer;

	SPtr<Material>	lineMaterial;

	RenderRequest();

	void reset(RenderContext* ctx, const Math::Camera3f& _camera);

	//TODO: move to separate cbuffer
	void setMaterialCommonParams(Material* mtl, const Mat4f& matrix = Mat4f::s_identity());

	void drawMesh	(const SrcLoc& debugLoc, const RenderMesh&    mesh,	   Material* material, const Mat4f& matrix = Mat4f::s_identity());
	void drawSubMesh(const SrcLoc& debugLoc, const RenderSubMesh& subMesh, Material* material, const Mat4f& matrix = Mat4f::s_identity());

	void drawLines(Span<Vertex_PosColor> points, Span<u16> indices);

	void drawFrustum(const Math::Frustum3f& frustum, const Color4b& color);

	void drawBoundingBox(const BBox3f&        bbox, const Color4b& color, const Mat4f& mat = Mat4f::s_identity());
	void drawBoundingBox(const RenderMesh&    mesh, const Color4b& color, const Mat4f& mat = Mat4f::s_identity());
	void drawBoundingBox(const RenderSubMesh& mesh, const Color4b& color, const Mat4f& mat = Mat4f::s_identity());

	RenderCommand_ClearFrameBuffers*	clearFrameBuffers() { return commandBuffer.clearFrameBuffers(); }
	RenderCommand_SwapBuffers*			swapBuffers()		{ return commandBuffer.swapBuffers(); }
	RenderCommand_DrawCall*				addDrawCall()		{ return commandBuffer.addDrawCall(); }

	SGE_NODISCARD	RenderScissorRectScope	scissorRectScope()	{ return RenderScissorRectScope(&commandBuffer); }
	SGE_INLINE		void setScissorRect(const Rect2f& rect)		{ commandBuffer.setScissorRect(rect); }

	void commit();

	struct Debug {
		bool drawBoundingBox = false;
	};
	Debug debug;

	const RenderContext* renderContext() const { return _renderContext; }

private:
	struct InlineDraw {
		Vector<u8>	vertexData;
		Vector<u8>	indexData;
		SPtr<RenderGpuBuffer>	vertexBuffer;
		SPtr<RenderGpuBuffer>	indexBuffer;

		void reset();
		void uploadToGpu();
	private:
		void _uploadToGpu(SPtr<RenderGpuBuffer>& buf, const Vector<u8>& data);
	};

	InlineDraw	_inlineDraw;

	RenderContext* _renderContext = nullptr;
};

} // namespace sge