#include "PatchIndexesManager.h"
#include <sge_render/Renderer.h>

namespace sge {

constexpr const int k_MaxLod = 2; // just for test 5x5

PatchIndexesManager* PatchIndexesManager::instance() {
	static PatchIndexesManager s;
	return &s;
}

PatchIndexesManager::PatchIndexesManager() {
	_maxLod = k_MaxLod;
	auto* renderer = Renderer::instance();

	for (u8 lod = _maxLod; lod > 0; lod--) {
		for (u8 tJoint = 0; tJoint < (1 << 4); tJoint++) {
			u8 key = get(lod, tJoint);
			//SGE_DUMP_VAR(lod, key);

			auto& newInfo = _table[key];

			Vector<u16> o1 = s_calcN(lod, _maxLod, tJoint >> 0 & 1);
			Vector<u16> o2 = s_calcE(lod, _maxLod, tJoint >> 1 & 1);
			Vector<u16> o3 = s_calcS(lod, _maxLod, tJoint >> 2 & 1);
			Vector<u16> o4 = s_calcW(lod, _maxLod, tJoint >> 3 & 1);

			newInfo._indexCount = o1.size() + o2.size() + o3.size() + o4.size();

			ByteSpan indexData;
			Vector<u16> index16Data;
			index16Data.appendRange(o1.span());
			index16Data.appendRange(o2.span());
			index16Data.appendRange(o3.span());
			index16Data.appendRange(o4.span());

			indexData = spanCast<const u8, const u16>(index16Data);

			RenderGpuBuffer::CreateDesc desc;
			desc.type = RenderGpuBufferType::Index;
			desc.bufferSize = indexData.size();
			newInfo._indexBuffer = renderer->createGpuBuffer(desc);
			newInfo._indexBuffer->uploadToGpu(indexData);

			SGE_DUMP_VAR(lod, key, newInfo._indexCount);
		}
	}

	// lod == 0
	for (u8 tJoint = 0; tJoint < (1 << 4); tJoint++) {
		u8 key = get(0, tJoint);
		SGE_DUMP_VAR(0, key);
		auto& newInfo = _table[key];
		newInfo._indexCount = 6;

		ByteSpan indexData;
		Vector<u16> index16Data;

		/*
			v0  v1
			* -- *
			! /  !
			* -- *
			v2   v3
		*/
		u16 v0 = 0;
		u16 v2 = 1 << _maxLod;
		u16 v3 = (v2+1)*(v2+1) - 1;
		u16 v1 = v3 - v2;

		index16Data.emplace_back(v0);
		index16Data.emplace_back(v1);
		index16Data.emplace_back(v2);

		index16Data.emplace_back(v1);
		index16Data.emplace_back(v3);
		index16Data.emplace_back(v2);

		indexData = spanCast<const u8, const u16>(index16Data);

		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Index;
		desc.bufferSize = indexData.size();
		newInfo._indexBuffer = renderer->createGpuBuffer(desc);
		newInfo._indexBuffer->uploadToGpu(indexData);
	}
}

u8 PatchIndexesManager::get(u8 lod, u8 tjoint) {
	// tjoint	(4 bit): WSEN 
	// lod		(4 bit): 0~15

	// ex:
	// 0000 0000
	// WSEN  lod

	return lod + (tjoint << 4);
}

PatchIndexesManager::Info* PatchIndexesManager::getInfo(u8 lod, u8 tjoint) {
	u8 key = get(lod, tjoint);
	auto it = _table.find(key);
	if (it == _table.end()) {
		return nullptr;
	}
	return &it->second;
}

struct MyHelper {

	MyHelper(Vec2i startXY_, int step_, Span<int> localDir_, bool isTjoint_)
		: moveTimes(-1)
		, step(step_)
		, startXY(startXY_)
		, localDir(localDir_)
		, isTjoint(isTjoint_)
	{ }

	void work(u8 lod, u8 maxLod, Vector<u16>& out) {
		if (!isTjoint) _calcIndexesInner1(lod, maxLod, out);
		else _calcIndexesInner2(lod, maxLod, out);
	}

private:

	int dx[4] = { 0, 1, 0, -1 };
	int dy[4] = { -1, 0, 1, 0 };

	// keep N E S W
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
		if (moveTimes % 4 == 0) { // 0 -> 1
			v0.set(moveE(v0));
			v2.set(moveW(v0));
			v1.set(moveS(v0));
		}
		else if (moveTimes % 4 == 1) { // 1 -> 2
			v1.set(moveE(v0));
			v2.set(moveS(v0));
		}
		else if (moveTimes % 4 == 2) { // 2 -> 3
			v0.set(moveE(v0));
			v1.set(moveS(v0));
			v2.set(moveW(v1));
		}
		else {                        // 3 -> 0
			v2.set(moveS(v0));
			v1.set(moveE(v2));
		}
		moveTimes++;
	}

	static int s_flatten(const Vec2i& p, int col) { return p.x * col + p.y; }

	void _calcIndexesInner1(u8 lod, u8 maxLod, Vector<u16>& out) {
		int maxRowCount = PatchIndexesManager::maxRowVertexCount(maxLod);
		for (u8 i = lod; i > 0; i--) {
			int cubeCount = PatchIndexesManager::maxRowVertexCount(i) - 1;
			int trangleCount = (cubeCount - 1) * 2; // ignore 1 cube
			resetV012();
			//SGE_DUMP_VAR(trangleCount, v0, v1, v2);
			for (int j = 0; j < trangleCount; j++) {
				moveNext();
				//SGE_DUMP_VAR(v0, v1, v2);
				out.emplace_back(s_flatten(v0, maxRowCount));
				out.emplace_back(s_flatten(v1, maxRowCount));
				out.emplace_back(s_flatten(v2, maxRowCount));
			}
		}
	}

	void _calcIndexesInner2(u8 lod, u8 maxLod, Vector<u16>& out) {
		int maxRowCount = PatchIndexesManager::maxRowVertexCount(maxLod);

		{ // first line
			int cubeCount = PatchIndexesManager::maxRowVertexCount(lod) - 1;
			int trangleCount = (cubeCount - 1) * 2; // ignore 1 cube
			resetV012();
			//SGE_DUMP_VAR(trangleCount, v0, v1, v2);
			for (int i = 0, j = 2; i < trangleCount; i++) {
				
				if (j == 2) { // tjoint
					Vec2i _v0(v0);
					
					moveNext(); // 0 -> 1
					moveNext(); // 1 -> 2
					Vec2i _v2(v2);
					Vec2i _v1(v1);

					//SGE_DUMP_VAR("big", _v0, _v1, _v2);
					out.emplace_back(s_flatten(_v0, maxRowCount));
					out.emplace_back(s_flatten(_v1, maxRowCount));
					out.emplace_back(s_flatten(_v2, maxRowCount));

					i += 2;
					j = 0;
				}
				else {
					moveNext(); // 2 -> 3, 3 -> 0
					//SGE_DUMP_VAR("small", v0, v1, v2);
					out.emplace_back(s_flatten(v0, maxRowCount));
					out.emplace_back(s_flatten(v1, maxRowCount));
					out.emplace_back(s_flatten(v2, maxRowCount));
					j ++;
				}
			}
		}

		_calcIndexesInner1(lod - 1, maxLod, out);
	}
	
	int moveTimes;
	int step;
	Vec2i startXY;
	Span<int> localDir;
	Vec2i v0, v1, v2;
	bool isTjoint;
};

Vector<u16> PatchIndexesManager::s_calcN(u8 lod, u8 maxLod, bool isTjoint) {
	int deltaLod = maxLod - lod;
	int step = 1 << deltaLod;
	//int rowCount = maxRowVertexCount(maxLod);

	// N E S W
	Vec2i startXY{ 0, 0 };
	int localDir[4] = { 0, 1, 2, 3 };
	Span<int> span(localDir, localDir + 4);
	auto helper = MyHelper(startXY, step, span, isTjoint);

	Vector<u16> res;
	helper.work(lod, maxLod, res);
	return res;
}

Vector<u16> PatchIndexesManager::s_calcE(u8 lod, u8 maxLod, bool isTjoint) {
	int deltaLod = maxLod - lod;
	int step = 1 << deltaLod;
	int rowCount = maxRowVertexCount(maxLod);

	// E S W N
	Vec2i startXY{ rowCount - 1, 0 };
	int localDir[4] = { 1, 2, 3, 0 };
	Span<int> span(localDir, localDir + 4);
	auto helper = MyHelper(startXY, step, span, isTjoint);
	
	Vector<u16> res;
	helper.work(lod, maxLod, res);
	return res;
}

Vector<u16> PatchIndexesManager::s_calcS(u8 lod, u8 maxLod, bool isTjoint) {
	int deltaLod = maxLod - lod;
	int step = 1 << deltaLod;
	int rowCount = maxRowVertexCount(maxLod);

	// S W N E
	Vec2i startXY{ rowCount - 1, rowCount - 1 };
	int localDir[4] = { 2, 3, 0, 1 };
	Span<int> span(localDir, localDir + 4);
	auto helper = MyHelper(startXY, step, span, isTjoint);

	Vector<u16> res;
	helper.work(lod, maxLod, res);
	return res;
}

Vector<u16> PatchIndexesManager::s_calcW(u8 lod, u8 maxLod, bool isTjoint) {
	int deltaLod = maxLod - lod;
	int step = 1 << deltaLod;
	int rowCount = maxRowVertexCount(maxLod);

	// S W N E
	Vec2i startXY{ 0, rowCount - 1 };
	int localDir[4] = { 3, 0, 1, 2 };
	Span<int> span(localDir, localDir + 4);
	auto helper = MyHelper(startXY, step, span, isTjoint);

	Vector<u16> res;
	helper.work(lod, maxLod, res);
	return res;
}

} // namespace