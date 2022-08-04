#include "RenderTerrain.h"

#include <sge_render/Renderer.h>
#include "../vertex/Vertex.h"

#include "PatchIndexesManager.h"

constexpr const int k_PatchRowCount = 6;
constexpr const int k_PatchColCount = 5;
constexpr const int k_PatchCount = k_PatchRowCount * k_PatchColCount; // test

namespace sge {

constexpr static int dx[4] = { -1, 0, 1, 0 };
constexpr static int dy[4] = { 0, 1, 0, -1 };

struct MyTestHelper {
	constexpr static u8 s_lods[k_PatchCount] = {
	//--------------> y
		0, 0, 1, 0, 0,
		0, 1, 1, 1, 0,
		1, 2, 2, 2, 1,
		0, 1, 2, 1, 0,
		0, 0, 1, 0, 0,
		0, 0, 1, 0, 0,
	};

	static u8 s_getLod(int index) {
		if (index >= k_PatchCount || index < 0)
			throw SGE_ERROR("array index out of range");
		return s_lods[index];
	}
};

// ----------------------------
void RenderTerrain::create()
{
	auto* renderer = Renderer::instance();

	int maxLod = PatchIndexesManager::instance()->maxLod();

	{ // vertex buffer
		_vertexLayout = Vertex_Pos::layout();

		int row = (1 << maxLod) + 1; // row vertex count
		int col = row;

		_vertexPerRowCount = row;
		_vertexCount = row * col; // row * col

		Vector<Vertex_Pos> vertexData;
		vertexData.resize(_vertexCount);
		for (int i = 0; i < row; i ++)
			for (int j = 0; j < col; j++) {
				int index = j + i * row;
				vertexData[index].pos = Vec3f(static_cast<float>(j), 0, static_cast<float>(i));
			}

		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Vertex;
		desc.bufferSize = _vertexCount * _vertexLayout->stride;
		_vertexBuffer = renderer->createGpuBuffer(desc);
		_vertexBuffer->uploadToGpu(spanCast<u8>(vertexData.span()));
	}

	_patches.resize(k_PatchCount);
	for (int i = 0; i < k_PatchCount; i++) {
		_patches[i].create(this, i);
	}
}

void RenderTerrain::Patch::create(RenderTerrain* terrain, int index) {
	_terrain = terrain;
	_index = index;

	auto* renderer = Renderer::instance();
	auto shader = renderer->createShader("Assets/Shaders/terrain.shader");
	_material = renderer->createMaterial();
	_material->setShader(shader);
}

void RenderTerrain::Patch::render(RenderCommandBuffer& cmdBuf) {
// --------------
	auto model = Mat4f::s_identity();
	auto sge_matrix_mvp = _terrain->sge_matrix_proj * _terrain->sge_matrix_view;

	_material->setParam("sge_matrix_model", model);
	_material->setParam("sge_matrix_view", _terrain->sge_matrix_view);
	_material->setParam("sge_matrix_proj", _terrain->sge_matrix_proj);
	_material->setParam("sge_matrix_mvp", sge_matrix_mvp);

	_material->setParam("sge_camera_pos", _terrain->sge_camera_pos);

	_material->setParam("sge_light_pos", _terrain->sge_light_pos);
	_material->setParam("sge_light_dir", _terrain->sge_light_dir);
	_material->setParam("sge_light_power", _terrain->sge_light_power);
	_material->setParam("sge_light_color", _terrain->sge_light_color);
// --------------
	int maxLod = PatchIndexesManager::instance()->maxLod();
	u8 lod = MyTestHelper::s_getLod(_index);
	_material->setParam("patchIndex", _index);
	_material->setParam("patchMaxLod", maxLod);
	_material->setParam("patchLod", lod);
// -----

#if 0
	{ // test index buffer
		auto* renderer = Renderer::instance();
		ByteSpan indexData;
		Vector<u16> index16Data;

		_indexCount = 96; //  6(cube) * 16
		int line = _terrain->vertexPerRowCount();
		for (int i = 0; i < line - 1; i++) {
			for (int j = 0; j < line - 1; j ++) {
				int ind = j + i * line;

				index16Data.emplace_back(ind);
				index16Data.emplace_back(ind + line);
				index16Data.emplace_back(ind + 1);

				index16Data.emplace_back(ind + 1);
				index16Data.emplace_back(ind + line);
				index16Data.emplace_back(ind + 1 + line);
			}
		}

		indexData = spanCast<const u8, const u16>(index16Data);

		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = indexData.size();
		_indexBuffer = renderer->createGpuBuffer(desc);
		_indexBuffer->uploadToGpu(indexData);
	}

	auto passes = _material->passes();
	for (size_t i = 0; i < passes.size(); i++) {
		auto* cmd = cmdBuf.newCommand<RenderCommand_DrawCall>();
		cmd->material = _material;
		cmd->materialPassIndex = i;

		cmd->primitive		= RenderPrimitiveType::Triangles;
		cmd->vertexLayout	= _terrain->vertexLayout();
		cmd->vertexBuffer	= _terrain->vertexBuffer();
		cmd->vertexCount	= _terrain->vertexCount();
		cmd->indexBuffer	= _indexBuffer;
		cmd->indexType		= RenderDataType::UInt16;
		cmd->indexCount		= _indexCount;
	}
#else

	int x = _index / k_PatchColCount;
	int y = _index % k_PatchColCount;

	u8 tJoint = 0;
	//TempString tmpStr;
	for (u8 i = 0; i < 4; i++) {
		int a = x + dx[i];
		int b = y + dy[i];
		if (a < 0 || b < 0 || a >= k_PatchRowCount || b >= k_PatchColCount) {
			//FmtTo(tmpStr, "{}(*):{},", static_cast<DirType>(i), lod);
			continue;
		}
		int index = a * k_PatchColCount + b;
		u8 otherLod = MyTestHelper::s_getLod(index);
		//FmtTo(tmpStr, "{}({}):{},", static_cast<DirType>(i), index, otherLod);
		if (otherLod < lod)
			tJoint |= (1 << i);
	}
	//FmtTo(tmpStr, "\n\t({})", _index);
	//SGE_DUMP_VAR(tmpStr, tJoint, lod);
	auto* info = PatchIndexesManager::instance()->getInfo(lod, tJoint);

	auto passes = _material->passes();
	for (size_t i = 0; i < passes.size(); i++) {
		auto* cmd = cmdBuf.newCommand<RenderCommand_DrawCall>();
		cmd->material = _material;
		cmd->materialPassIndex = i;
		cmd->primitive		= RenderPrimitiveType::Triangles;
		cmd->vertexLayout	= _terrain->vertexLayout();
		cmd->vertexBuffer	= _terrain->vertexBuffer();
		cmd->vertexCount	= _terrain->vertexCount();
		cmd->indexBuffer	= info->indexBuffer();
		cmd->indexType		= info->indexType();
		cmd->indexCount		= info->indexCount();
	}
#endif
}

void RenderTerrain::render(RenderCommandBuffer& cmdBuf) {
	for (int i = 0; i < _patches.size(); i++) {
		auto& patch = _patches[i];
		patch.render(cmdBuf);
	}
}

} // namespace