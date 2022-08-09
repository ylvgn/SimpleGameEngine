#pragma once
#include "../command/RenderCommand.h"
#include "../shader/Material.h"
#include "../command/RenderRequest.h"

namespace sge {

class RenderTerrain : public NonCopyable {

class Patch {

public:
	void create(RenderTerrain* terrain, int index);
	void render(RenderRequest& req);

private:
	int _index = -1;
	int _indexCount = 0;
	SPtr<RenderGpuBuffer> _indexBuffer;
	SPtr<Material> _material;
	RenderTerrain* _terrain = nullptr;
};

public:
	void create();
	void render(RenderRequest& req);

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
