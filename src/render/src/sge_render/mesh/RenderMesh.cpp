#include "RenderMesh.h"
#include "EditMesh.h"

#include <sge_render/vertex/Vertex.h>
#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/Renderer.h>

namespace sge {

struct RenderMesh_InternalHelper {

static bool hasAttr(size_t arraySize, size_t vertexCount) {
	if (arraySize <= 0) return false;
	if (arraySize < vertexCount) {
		SGE_ASSERT(false);
		return false;
	}
	return true;
}

/*
	|--offset--|
	| position , uv , color || position , uv , color | ...
	|---------stride--------||---------stride--------| ...
	[        vertex1        ][        vertex2        ] ...
*/
template<class DST, class T> inline
static void copyVertexData(DST* dst
							, size_t vertexCount
							, const VertexLayout::Element& element
							, size_t stride
							, const T* src) 
{
	u8* p = dst + element.offset;

	for (size_t i = 0; i < vertexCount; i++) {
		*reinterpret_cast<T*>(p) = *src;
		src++;
		p += stride;
	}
}

}; // RenderMesh_InternalHelper


void RenderMesh::create(const EditMesh& src) {
	using Helper = RenderMesh_InternalHelper;
	clear();

	size_t vertexCount = src.pos.size();
	if (vertexCount <= 0) return;

	u8 uvCount = 0;
	u8 colorCount = 0;
	u8 normalCount = 0;
	u8 tangentCount = 0;
	u8 binormalCount = 0;

	_primitive = RenderPrimitiveType::Triangles;

	if (Helper::hasAttr(src.color.size(), vertexCount)) colorCount = 1;
	if (Helper::hasAttr(src.normal.size(), vertexCount)) normalCount = 1;
	if (Helper::hasAttr(src.tangent.size(),	vertexCount)) tangentCount = 1;
	if (Helper::hasAttr(src.binormal.size(), vertexCount)) binormalCount = 1;

	for (u8 i = 0; i < EditMesh::kUvCountMax; i++) {
		if (Helper::hasAttr(src.uv[i].size(), vertexCount)) uvCount = i + 1;
	}

	auto vertexType = VertexTypeUtil::make(
						RenderDataTypeUtil::get<Tuple3f>(),				// pos
						RenderDataTypeUtil::get<Color4b>(), colorCount, // color
						RenderDataTypeUtil::get<Tuple2f>(), uvCount,	// uv
						RenderDataTypeUtil::get<Tuple3f>(), normalCount, tangentCount, binormalCount); // normal, tangent, binormal

	_vertexLayout = VertexLayoutManager::instance()->getLayout(vertexType);
	if (!_vertexLayout) {
		throw SGE_ERROR("cannot find vertex Layout for mesh");
	}

	// tmp
	setSubMeshCount(1);
	_subMeshes[0].create(src);
}

void RenderMesh::clear() {
	_vertexLayout = nullptr;
	_subMeshes.clear();
}

void RenderMesh::setSubMeshCount(size_t newSize) {
	size_t oldSize = _subMeshes.size();
	_subMeshes.resize(newSize);
	for (size_t i = oldSize; i < newSize; i++) {
		_subMeshes[i]._mesh = this;
	}
}

void RenderSubMesh::create(const EditMesh& src) {
	using Helper = RenderMesh_InternalHelper;
	clear();

	size_t vc = src.pos.size();
	_indexCount = src.indices.size();

	if (vc <= 0) return;
	setVertexCount(vc);

	auto* _vertexLayout = vertexLayout();
	auto* pData = _vertexData.data();
	auto stride = _vertexLayout->stride;

	for (auto& e : _vertexLayout->elements) {
		using S		= VertexSemantic;
		using ST	= VertexSemanticType;
		using U		= VertexSemanticUtil;

		auto semanticType = U::getType(e.semantic);
		auto semanticIndex = U::getIndex(e.semantic);

		switch (semanticType) {
		case ST::TEXCOORD: {
			if (semanticIndex < EditMesh::kUvCountMax) {
				Helper::copyVertexData(pData, vc, e, stride, src.uv[semanticIndex].data()); break;
			}
			continue;
		} break;
		}

		switch (e.semantic) {
		case S::POSITION: {
//			Helper::copyVertexData(pData, vc, e, stride, src.pos.data());
			const Tuple3f* srcData = src.pos.data();

			u8* dstData = pData + e.offset;
			for (size_t i = 0; i < vc; i++) {
				*reinterpret_cast<Tuple3f*>(dstData) = *srcData;

				_boundingBox.encapsulate(*srcData);

				srcData++;
				dstData += stride;
			}
		} break;
		case S::COLOR0:		Helper::copyVertexData(pData, vc, e, stride, src.color.data()); break;
		case S::NORMAL:		Helper::copyVertexData(pData, vc, e, stride, src.normal.data()); break;
		case S::TANGENT:	Helper::copyVertexData(pData, vc, e, stride, src.tangent.data()); break;
		case S::BINORMAL:	Helper::copyVertexData(pData, vc, e, stride, src.binormal.data()); break;
		}
	}
	//------
	auto* renderer = Renderer::instance();
	setVertexBuffer();

	if (_indexCount > 0) {
		ByteSpan indexData;
		Vector<u16, 1024> index16Data;

		if (_vertexCount > UINT16_MAX) {
			_indexType = RenderDataType::UInt32;
			indexData = ByteSpan_make(src.indices.span());
		}
		else {
			_indexType = RenderDataType::UInt16;
			index16Data.resize(src.indices.size());
			for (size_t i = 0; i < src.indices.size(); i++) {
				u32 vi = src.indices[i];
				index16Data[i] = static_cast<u16>(vi);
			}
			indexData = ByteSpan_make(index16Data.span());
		}

		RenderGpuBuffer::CreateDesc desc;
		desc.type		= RenderGpuBufferType::Index;
		desc.bufferSize = indexData.size();

		_indexBuffer = renderer->createGpuBuffer(desc);
		_indexBuffer->uploadToGpu(indexData);
	}
}

void RenderSubMesh::clear() {
	_vertexBuffer	= nullptr;
	_indexBuffer	= nullptr;
	_vertexCount	= 0;
	_indexCount		= 0;
	_vertexData.clear();
}

void RenderSubMesh::_setVertexBuffer(ByteSpan vertexData) {
	auto* renderer = Renderer::instance();

	RenderGpuBuffer::CreateDesc desc;
	desc.type		= RenderGpuBufferType::Vertex;
	desc.bufferSize = vertexData.size();

	_vertexBuffer = renderer->createGpuBuffer(desc);
	_vertexBuffer->uploadToGpu(vertexData);
}

} // namespace