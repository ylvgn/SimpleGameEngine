#pragma once
#include "../command/RenderCommand.h"
#include "../shader/Material.h"

namespace sge {

class RenderTerrain : public NonCopyable {

class Patch {

public:
	void create(RenderTerrain* terrain, int index);
	void render(RenderCommandBuffer& cmdBuf);

private:
	int _index = -1;
	int _indexCount = 0;
	SPtr<RenderGpuBuffer> _indexBuffer;
	SPtr<Material> _material;
	RenderTerrain* _terrain = nullptr;
};

public:
	void create();
	void render(RenderCommandBuffer& cmdBuf);

// ----- tmp
	Mat4f sge_matrix_model = Mat4f::s_identity();
	Mat4f sge_matrix_view;
	Mat4f sge_matrix_proj;
	Vec3f sge_camera_pos;
	Vec3f sge_light_pos{10,10,0};
	Vec3f sge_light_dir{-5, -10, -1};
	float sge_light_power = 4.0f;
	Vec3f sge_light_color{ 1, 1, 1 };
// -------------

	const VertexLayout* vertexLayout() const { return _vertexLayout; }
	int vertexCount() { return _vertexCount; }
	RenderGpuBuffer* vertexBuffer() { return _vertexBuffer; }
	int vertexPerRowCount() { return _vertexPerRowCount; }

private:
	Vector<Patch> _patches;

	const VertexLayout* _vertexLayout = nullptr;
	SPtr<RenderGpuBuffer> _vertexBuffer;
	int _vertexCount = 0;
	int _vertexPerRowCount = 0;
};

} // namespace
