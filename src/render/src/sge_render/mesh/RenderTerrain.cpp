#include "RenderTerrain.h"

#include <sge_render/Renderer.h>
#include "../vertex/Vertex.h"

namespace sge {

#define ENABLE_MY_TEST 0 // just for test 5x5

#if ENABLE_MY_TEST
	constexpr const int k_PatchRowCount = 5;
	constexpr const int k_PatchColCount = 5;
	constexpr const int k_PatchCount = k_PatchRowCount * k_PatchColCount;

	struct MyTestHelper {
		constexpr static int s_lods[k_PatchCount] = {
			//------------------> x
		#if 1
				4, 3, 2, 3, 4,
				3, 2, 1, 2, 3,
				2, 1, 0, 1, 2,
				3, 2, 1, 2, 3,
				4, 3, 2, 3, 4,
		#else
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
		#endif
		};

		static int s_getLevel(const Vec2i& index) {
			int x = index.x;
			int y = index.y;
			if (x < 0 || y < 0 || x >= k_PatchColCount || y >= k_PatchRowCount) {
				throw SGE_ERROR("array index out of range");
			}
			return s_lods[y * k_PatchColCount + x];
		}

		static int s_getLevel(int x, int y) {
			return s_getLevel({ x, y });
		}
	};
#endif


constexpr static int dx[4] = { 0, 1, 0, -1 };
constexpr static int dy[4] = { -1, 0, 1, 0 };

void RenderTerrain::createFromHeightMapFile(const Vec3f& terrainPos, const Vec2f& terrainSize, float terrainHeight, int maxLod, StrView heightMapFilename) {
	Image img;
	img.loadPngFile(heightMapFilename);
	createFromHeightMap(terrainPos, terrainSize, terrainHeight, maxLod, img);
}

void RenderTerrain::createFromHeightMap(const Vec3f& terrainPos, const Vec2f& terrainSize, float terrainHeight, int maxLod, const Image& heightMap) {
	destroy();

	if (heightMap.width() < 1 || heightMap.height() < 1)
		throw SGE_ERROR("invalid height map size");

	if (maxLod < 1)
		maxLod = 1;

	if (maxLod > 8) // 2^8 * 2^8 = 2^16 = 65536, so vertex index > uint16
		throw SGE_ERROR("reach max lod limit");

	_heightmapResolution.set(heightMap.width(), heightMap.height());
	_terrainPos  = terrainPos;
	_terrainSize = terrainSize;
	_terrainHeight = terrainHeight;

	_maxLod = maxLod;

	_patchCount = (_heightmapResolution - 1) / patchCellsPerRow(); // (2049-1) / patchCellsPerRow

#if ENABLE_MY_TEST
	_patchCount.set(5, 5); // just for test
#endif

	_patchLevelIndices.resize(maxLod);
	for (int lv = 0; lv < _patchLevelIndices.size(); lv++) {
		_patchLevelIndices[lv].create(this, lv);
	}

	{ // Patches
		_patches.resize(_patchCount.x * _patchCount.y);
#if ENABLE_MY_TEST
		auto shader = Renderer::instance()->createShader("Assets/Shaders/terrain_test.shader");
#else
		auto shader = Renderer::instance()->createShader("Assets/Shaders/terrain.shader");
#endif
		auto* p = _patches.begin();
		for (int y = 0; y < _patchCount.y; y++) {
			for (int x = 0; x < _patchCount.x; x++) {
				p->create(this, Vec2i(x, y), shader);
				p++;
			}
		}
		SGE_ASSERT(p == _patches.end());
	}

	auto* renderer = Renderer::instance();

	{ // vertex buffer
		int verticesPerRow = patchVerticesPerRow();
#if ENABLE_MY_TEST
		_vertexLayout = Vertex_Pos::s_layout();
		Vector<Vertex_Pos> vertexData;
		_vertexCount = verticesPerRow * verticesPerRow;
		vertexData.resize(_vertexCount);
		for (int y = 0; y < verticesPerRow; y++)
			for (int x = 0; x < verticesPerRow; x++) {
				int index = y * verticesPerRow + x;
				vertexData[index].pos = Vec3f(static_cast<float>(x), 0, static_cast<float>(y)); // xz
			}
#else
		Vector<Vertex>	vertexData;
		_vertexLayout = Vertex::s_layout();
		_vertexCount = verticesPerRow * verticesPerRow;
		vertexData.resize(_vertexCount);

		auto scale = patchSize();
		int cellsPerRow = patchCellsPerRow();
		auto* dst = vertexData.begin();
		for (int y = 0; y < verticesPerRow; y++) {
			for (int x = 0; x < verticesPerRow; x++) {
				dst->pos = Vec2f::s_cast(Vec2i(x, y)) * scale / static_cast<float>(cellsPerRow);
				dst++;
			}
		}
		SGE_ASSERT(dst == vertexData.end());
#endif
		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Vertex;
		desc.bufferSize = _vertexCount * _vertexLayout->stride;
		_vertexBuffer = renderer->createGpuBuffer(desc);
		_vertexBuffer->uploadToGpu(spanCast<u8>(vertexData.span()));
	}

	{ // hight map
		SamplerState samplerState;
		samplerState.filter = TextureFilter::Point;
		samplerState.wrapU = TextureWrap::Clamp;
		samplerState.wrapV = TextureWrap::Clamp;
		_heightMapTexture = renderer->createTexture2D(heightMap, samplerState);
	}
}

void RenderTerrain::Patch::create(Terrain* terrain, const Vec2i& index, Shader* shader) {
	_terrain = terrain;
	_index = index;

	auto* renderer = Renderer::instance();
	_material = renderer->createMaterial();
	_material->setShader(shader);
}

void RenderTerrain::Patch::setDisplayLevelByViewPos(const Vec3f& viewPos) {
	auto distance = (worldCenterPos() - viewPos).length();
	auto d = _terrain->patchSize().x * 5;
	_displayLevel = static_cast<int>(distance / d);
}

void RenderTerrain::render(RenderRequest& req) {
#if !ENABLE_MY_TEST
	for (auto& p : _patches) {
		p.setDisplayLevelByViewPos(req.cameraPos);
	}
#endif

	for (auto& p : _patches) {
		p.render(req);
	}
}

void RenderTerrain::Patch::render(RenderRequest& req) {
#if ENABLE_MY_TEST
	int lv = MyTestHelper::s_getLevel(_index);
#else
	int lv = Math::clamp(_displayLevel, int(0), _terrain->maxLod() - 1);
#endif

	ZoneMask zoneMask = ZoneMask::None;
	for (int i = 0; i < 4; i++) {
		int a = _index.x + dx[i];
		int b = _index.y + dy[i];
#if ENABLE_MY_TEST
		if (a < 0 || b < 0 || a >= k_PatchRowCount || b >= k_PatchColCount) {
			//FmtTo(tmpStr, "{}(*),", dir);
			continue;
		}
		int displayLevel = MyTestHelper::s_getLevel(a, b);
		if (displayLevel > lv)
			zoneMask |= static_cast<ZoneMask>(1 << i);
#else
		auto* adj = _terrain->patch(a, b);
		if (!adj) {
			continue;
		}
		if (adj->displayLevel() > lv) {
			zoneMask |= static_cast<ZoneMask>(1 << i);
		}
#endif
	}

	auto* pi = _terrain->patchIndices(lv, zoneMask);
	if (!pi) { SGE_ASSERT(false); return; }

	if (!_material) { SGE_ASSERT(false); return; }
	req.setMaterialCommonParams(_material);
#if !ENABLE_MY_TEST
	_material->setParam("terrainHeightMap", _terrain->heightMapTexture());
	_material->setParam("terrainPos",		_terrain->terrainPos());
	_material->setParam("terrainSize",		_terrain->terrainSize());
	_material->setParam("terrainHeight",	_terrain->terrainHeight());
	_material->setParam("patchSize",		_terrain->patchSize());
#endif

	_material->setParam("patchCellsPerRow", _terrain->patchCellsPerRow());
	_material->setParam("patchIndex", _index);

	auto passes = _material->passes();
	for (int i = 0; i < passes.size(); i++) {
		auto* cmd				= req.addDrawCall();
#if _DEBUG
		cmd->debugLoc			= SGE_LOC;
#endif
		cmd->material			= _material;
		cmd->materialPassIndex	= i;
		cmd->primitive			= RenderPrimitiveType::Triangles;
		cmd->vertexLayout		= _terrain->vertexLayout();
		cmd->vertexBuffer		= _terrain->vertexBuffer();
		cmd->vertexCount		= _terrain->vertexCount();
		cmd->indexBuffer		= pi->indexBuffer();
		cmd->indexType			= pi->indexType();
		cmd->indexCount			= pi->indexCount();
	}
}


void RenderTerrain::destroy() {
	_terrainSize.set(0, 0);
	_terrainHeight = 0;
	_heightmapResolution.set(0, 0);
	_maxLod = 1;
	_patchCount.set(0, 0);
	_patchLevelIndices.clear();

	_vertexBuffer = nullptr;
	_vertexLayout = nullptr;
	_vertexCount = 0;
}

void RenderTerrain::PatchLevelIndices::create(Terrain* terrain, int level) {
	_terrain = terrain;
	_level = level;

	_patchIndices.resize(s_patchMeshCount); // 1 << 4 == 16
	auto zoneMask = ZoneMask::None;
	for (auto& it : _patchIndices) {
		it.create(terrain, level, zoneMask);
		zoneMask += 1;
	}
}


//---------------
struct MyHelper {
	using ZoneMask = RenderTerrain3_ZoneMask;

	static int s_flatten(const Vec2i& p, int col) { return p.y * col + p.x; }

	static int maxRowVertexCount(int lod) { return (1 << lod) + 1; }

	MyHelper(Vec2i startXY_, int step_, Span<int> localDir_, bool isTjoint_)
		: moveTimes(-1)
		, step(step_)
		, startXY(startXY_)
		, localDir(localDir_)
		, isTjoint(isTjoint_)
	{ }

	void work(int lod, int maxLod, Vector<u16>& out) {
		int maxRowCount = maxRowVertexCount(maxLod);
		int maxCubeCount = maxRowVertexCount(lod) - 1;
		int maxTrangleCount = (maxCubeCount - 1) * 2; // ignore 1 cube

		if (isTjoint) {  // first row 
			resetV012();
			//SGE_DUMP_VAR(trangleCount, v0, v1, v2);
			for (int i = 0, j = 2; i < maxTrangleCount;) {

				if (j == 2) { // tjoint
					Vec2i _v0(v0);

					moveNext(); // 0 -> 1
					moveNext(); // 1 -> 2

					Vec2i _v2(v2);
					Vec2i _v1(v1);

					//SGE_DUMP_VAR("big", _v0, _v1, _v2);
					out.emplace_back(s_flatten(_v0, maxRowCount));
					out.emplace_back(s_flatten(_v2, maxRowCount));
					out.emplace_back(s_flatten(_v1, maxRowCount));

					i += 2;
					j = 0;
				}
				else {
					moveNext(); // 2 -> 3, 3 -> 0
					//SGE_DUMP_VAR("small", v0, v1, v2);
					out.emplace_back(s_flatten(v0, maxRowCount));
					out.emplace_back(s_flatten(v2, maxRowCount));
					out.emplace_back(s_flatten(v1, maxRowCount));
					i++;
					j++;
				}
			}
			maxTrangleCount -= 4; // next row
		}

		while (maxTrangleCount > 0) {
			resetV012();
			//SGE_DUMP_VAR(maxTrangleCount, v0, v1, v2);
			for (int j = 0; j < maxTrangleCount; j++) {
				moveNext();
				//SGE_DUMP_VAR(v0, v1, v2);
				out.emplace_back(s_flatten(v0, maxRowCount));
				out.emplace_back(s_flatten(v2, maxRowCount));
				out.emplace_back(s_flatten(v1, maxRowCount));
			}
			maxTrangleCount -= 4;
		}
	}

private:
	Vec2i moveN(const Vec2i& p) {
		Vec2i o;
		int index = localDir[0];
		o.x = p.x + dx[index] * step;
		o.y = p.y + dy[index] * step;
		return o;
	}

	Vec2i moveE(const Vec2i& p) {
		Vec2i o;
		int index = localDir[1];
		o.x = p.x + dx[index] * step;
		o.y = p.y + dy[index] * step;
		return o;
	}

	Vec2i moveS(const Vec2i& p) {
		Vec2i o;
		int index = localDir[2];
		o.x = p.x + dx[index] * step;
		o.y = p.y + dy[index] * step;
		return o;
	}

	Vec2i moveW(const Vec2i& p) {
		Vec2i o;
		int index = localDir[3];
		o.x = p.x + dx[index] * step;
		o.y = p.y + dy[index] * step;
		return o;
	}

	void resetV012()
	{
		/*
			v0(startXY_)
			*
			* *
			v1 v2
		*/
		if (moveTimes != -1) {
			startXY.set(moveE(startXY));
			startXY.set(moveS(startXY));
		}
		v0.set(startXY);
		v2.set(moveS(v0));
		v1.set(moveE(v2));
		moveTimes = 0; // reset to %4==0
	}

	void moveNext() {
		/*
				v0      v2 v0     v0 v1     v0    v0
				*   -->  * *  --> * * -->   * --> *
				* *        *      *       * *     * *
				v2 v1      v1     v2     v2 v1    v2 v1
				 [0]     [1]      [2]     [3]     [0]
		*/

		if (moveTimes % 4 == 0) {		// 0 -> 1
			v0.set(moveE(v0));
			v2.set(moveW(v0));
			v1.set(moveS(v0));
		}
		else if (moveTimes % 4 == 1) {	// 1 -> 2
			v1.set(moveE(v0));
			v2.set(moveS(v0));
		}
		else if (moveTimes % 4 == 2) {	// 2 -> 3
			v0.set(moveE(v0));
			v1.set(moveS(v0));
			v2.set(moveW(v1));
		}
		else {							// 3 -> 0
			v2.set(moveS(v0));
			v1.set(moveE(v2));
		}
		moveTimes++;
	}

	int moveTimes;
	int step;
	Vec2i startXY;
	Span<int> localDir;
	Vec2i v0, v1, v2;
	bool isTjoint;
};

void RenderTerrain::PatchIndices::create(Terrain* terrain, int level, ZoneMask zoneMask) {
	Vector<VertexIndex>	indexData;
	int verticesPerRow	= terrain->patchVerticesPerRow();
	int cellsPerRow		= terrain->patchCellsPerRow();
	int maxLod			= terrain->maxLod();

	int tmp = (maxLod - 1) - level;

	if (tmp == 0) {
		/*
				v0  v1
				* -- *
				! \  !
				* -- *
				v2   v3
		*/
		int v0 = 0;
		int v1 = cellsPerRow;
		int v3 = verticesPerRow * verticesPerRow - 1;
		int v2 = v3 - v1;

		indexData.resize(6);

		indexData[0] = static_cast<VertexIndex>(v0);
		indexData[1] = static_cast<VertexIndex>(v2);
		indexData[2] = static_cast<VertexIndex>(v3);

		indexData[3] = static_cast<VertexIndex>(v0);
		indexData[4] = static_cast<VertexIndex>(v3);
		indexData[5] = static_cast<VertexIndex>(v1);
		//SGE_DUMP_VAR(v0, v1, v2, v3);
	}

	else {
		int step = 1 << level;

		{ // north
			Vec2i startXY{ 0, 0 };
			Vector<int, 4> localDir = { 0, 1, 2, 3 };
			bool isTjoint = enumHas(zoneMask, ZoneMask::North);
			auto helper = MyHelper(startXY, step, localDir.span(), isTjoint);
			helper.work(tmp, maxLod - 1, indexData);
		}

		{ // east
			Vec2i startXY{ terrain->patchVerticesPerRow() - 1, 0 };
			Vector<int, 4> localDir = { 1, 2, 3, 0 };
			bool isTjoint = enumHas(zoneMask, ZoneMask::East);
			auto helper = MyHelper(startXY, step, localDir.span(), isTjoint);
			helper.work(tmp, maxLod - 1, indexData);
		}

		{ // south
			Vec2i startXY{ terrain->patchVerticesPerRow() - 1, terrain->patchVerticesPerRow() - 1 };
			Vector<int, 4> localDir = { 2, 3, 0, 1 };
			bool isTjoint = enumHas(zoneMask, ZoneMask::South);
			auto helper = MyHelper(startXY, step, localDir.span(), isTjoint);
			helper.work(tmp, maxLod - 1, indexData);
		}

		{ // west
			Vec2i startXY{ 0, terrain->patchVerticesPerRow() - 1 };
			Vector<int, 4> localDir = { 3, 0, 1, 2 };
			bool isTjoint = enumHas(zoneMask, ZoneMask::West);
			auto helper = MyHelper(startXY, step, localDir.span(), isTjoint);
			helper.work(tmp, maxLod - 1, indexData);
		}
	}

	{
		auto* renderer = Renderer::instance();

		auto byteSpan = spanCast<const u8>(indexData.span());

		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = byteSpan.size();

		_indexCount = indexData.size();
		_indexBuffer = renderer->createGpuBuffer(desc);
		_indexBuffer->uploadToGpu(byteSpan);
	}
}

} // namespace sge