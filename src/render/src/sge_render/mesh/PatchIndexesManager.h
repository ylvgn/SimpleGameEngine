#pragma once
#include "../buffer/RenderGpuBuffer.h"
#include "../RenderDataType.h"

namespace sge {

#define DirType_ENUM_LIST(E_) \
	E_(N,)  \
	E_(E,)  \
	E_(S,)  \
	E_(W,)  \
//----
SGE_ENUM_CLASS(DirType, u8)
SGE_ENUM_ALL_OPERATOR(DirType)

class PatchIndexesManager {
public:
	PatchIndexesManager();
	static PatchIndexesManager* instance();

	u8 maxLod() const { return _maxLod; }								// 0 1 2 3 ... 31

	int maxRowVertexCount() const { return maxRowVertexCount(_maxLod); }// 2 3 5 9 ... 2^31 + 1
	static int maxRowVertexCount(u8 lod) { return (1 << lod) + 1;}

	struct Info {
		friend class PatchIndexesManager;
	public:
		RenderGpuBuffer* indexBuffer()	{ return _indexBuffer; }
		RenderDataType indexType()		{ return _indexType; }
		size_t indexCount()				{ return _indexCount; }
	protected:
		SPtr<RenderGpuBuffer> _indexBuffer;
		RenderDataType _indexType = RenderDataType::UInt16;
		size_t _indexCount = 0;
	};

	Info* getInfo(u8 lod, u8 tJoint);

private:

	static Vector<u16> s_calcN(u8 lod, u8 maxLod, bool isTjoint);
	static Vector<u16> s_calcE(u8 lod, u8 maxLod, bool isTjoint);
	static Vector<u16> s_calcS(u8 lod, u8 maxLod, bool isTjoint);
	static Vector<u16> s_calcW(u8 lod, u8 maxLod, bool isTjoint);

	u8 get(u8 lod, u8 isTjoint);
	Map<u8, Info> _table;
	u8 _maxLod = 0;

}; // PatchIndexesManager

} // namespace