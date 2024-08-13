#include "RenderMesh.h"
#include "EditMesh.h"

#include <sge_render/vertex/Vertex.h>
#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/Renderer.h>

namespace sge {

#if 0
#pragma mark ========= RenderMesh_InternalHelper ============
#endif
struct RenderMesh_InternalHelper {

static bool hasAttr(size_t arraySize, size_t vertexCount) {
	if (arraySize <= 0) return false;
	if (arraySize != vertexCount) {
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

	for (size_t i = 0; i < vertexCount; ++i) {
		*reinterpret_cast<T*>(p) = *src;
		src++;
		p += stride;
	}
}

}; // RenderMesh_InternalHelper

#if 0
#pragma mark ========= RenderMesh ============
#endif
void RenderMesh::create(const EditMesh& src) {
	using Helper = RenderMesh_InternalHelper;
	clear();

	size_t vertexCount = src.pos.size();
	if (vertexCount <= 0) return;

	u8 uvCount			= 0;
	u8 colorCount		= 0;
	u8 normalCount		= 0;
	u8 tangentCount		= 0;
	u8 binormalCount	= 0;

	_primitive = RenderPrimitiveType::Triangles;

	if (Helper::hasAttr(src.color.size(), vertexCount)) colorCount = 1;
	if (Helper::hasAttr(src.normal.size(), vertexCount)) normalCount = 1;
	if (Helper::hasAttr(src.tangent.size(),	vertexCount)) tangentCount = 1;
	if (Helper::hasAttr(src.binormal.size(), vertexCount)) binormalCount = 1;

	for (u8 i = 0; i < EditMesh::kUvCountMax; ++i) {
		if (Helper::hasAttr(src.uv[i].size(), vertexCount))
			uvCount = i + 1;
	}

	using PosType		= std::remove_extent_t<decltype(src.pos)>::value_type;
	using ColorType		= std::remove_extent_t<decltype(src.color)>::value_type;
	using UvType		= std::remove_extent_t<decltype(src.uv)>::value_type;
	using NormalType	= std::remove_extent_t<decltype(src.normal)>::value_type;

	auto vertexType = VertexTypeUtil::make(
						RenderDataTypeUtil::get<PosType>(),
						RenderDataTypeUtil::get<ColorType>(), colorCount,
						RenderDataTypeUtil::get<UvType>(), uvCount,
						RenderDataTypeUtil::get<NormalType>(), normalCount, tangentCount, binormalCount);

	_vertexLayout = VertexLayoutManager::instance()->getLayout(vertexType);
	if (!_vertexLayout) {
		throw SGE_ERROR("cannot find vertex Layout for mesh");
	}

#if 0
	setSubMeshCount(1);
	_subMeshes[0].create(src);
#else
	{
		int subMeshCount = Math::ceilToInt(static_cast<double>(vertexCount) / kSubMeshMaxVertexCount);
		setSubMeshCount(subMeshCount);

		// --- vertex buffer
		size_t offset = 0;
		for (int i = 0; i < subMeshCount - 1; ++i) {
			_subMeshes[i]._createVB(src, kSubMeshMaxVertexCount, offset);
			offset += kSubMeshMaxVertexCount;
		}
		_subMeshes[subMeshCount - 1]._createVB(src, vertexCount - offset, offset);

		// --- index buffer
		auto& subMesh = _subMeshes[0];
		subMesh._createIB(src, src.indices.size()); // TODO
		for (int i = 1; i < subMeshCount; ++i) {
			_subMeshes[i]._indexBuffer = subMesh._indexBuffer;
		}
	}
#endif
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

#if 0
#pragma mark ========= RenderSubMesh ============
#endif
void RenderSubMesh::create(const EditMesh& src) {
	_createVB(src, src.pos.size());
	_createIB(src, src.indices.size());
}

void RenderSubMesh::clear() {
	_vertexBuffer	= nullptr;
	_indexBuffer	= nullptr;
	_vertexCount	= 0;
	_indexCount		= 0;
	_indexType		= RenderDataType::None;
	_vertexData.clear();
	_indexData.clear();
}

void RenderSubMesh::setVertexCount(size_t vc) {
	_vertexCount = vc;
	_vertexData.resize(vertexLayout()->stride * _vertexCount);

	if (_vertexCount > std::numeric_limits<u16>::max()) {
		_indexType = RenderDataType::UInt32;
	} else {
		_indexType = RenderDataType::UInt16;
	}
}

void RenderSubMesh::setIndexCount(size_t ic) {
	using SRC	= RenderDataType;
	using T		= decltype(_indexData)::value_type;

	_indexCount = ic;

	switch (_indexType) {
		case SRC::UInt16: _indexData.resize(ic * sizeof(u16) / sizeof(T)); break;
		case SRC::UInt32: _indexData.resize(ic * sizeof(u32) / sizeof(T)); break;
		default: throw SGE_ERROR("unsupported indexType");
	}
}

void RenderSubMesh::setVertexBuffer(bool isKeepData /*= false*/) {
	_setVertexBuffer(_vertexData);
	if (!isKeepData) _vertexData.clear();
}

void RenderSubMesh::setIndexBuffer(bool isKeepData /*= false*/) {
	_setIndexBuffer(_indexData);
	if (!isKeepData) _indexData.clear();
}

void RenderSubMesh::setIndexData(const Span<const u16> indexData) {
	using SRC = RenderDataType;

	switch (_indexType) {
		case SRC::UInt16: {
			setIndexCount(indexData.size());
			auto byteSpan = ByteSpan_make(indexData);
			_indexData.assign(byteSpan.begin(), byteSpan.end());
			_indexCount = _indexData.size();
		} break;
		case SRC::UInt32: {
			using DST_T = u32;
			using SRC_T = decltype(indexData)::element_type;

			SGE_ASSERT(_vertexCount > UINT16_MAX);
			setIndexCount(indexData.size());
			_setIndexData<DST_T, SRC_T>(indexData);
		} break;
		default:
			throw SGE_ERROR("setIndexData unsupported indexType");
	}
}

void RenderSubMesh::setIndexData(const Span<const u32> indexData) {
	using SRC = RenderDataType;

	switch (_indexType) {
		case SRC::UInt16: {
			using DST_T = u16;
			using SRC_T = decltype(indexData)::element_type;

			SGE_ASSERT(_vertexCount <= UINT16_MAX);
			setIndexCount(indexData.size());
			_setIndexData<DST_T, SRC_T>(indexData);
		} break;
		case SRC::UInt32: {
			setIndexCount(indexData.size());

			auto byteSpan = ByteSpan_make(indexData);
			_indexData.assign(byteSpan.begin(), byteSpan.end());
		} break;
		default:
			throw SGE_ERROR("setIndexData unsupported indexType");
	}
}

void RenderSubMesh::_createVB(const EditMesh& src, size_t vc, size_t offet /*= 0*/) {
	using Helper = RenderMesh_InternalHelper;

	clear();

	if (vc <= 0)
		return;

	setVertexCount(vc);

	auto* _vertexLayout = vertexLayout();
	auto* pData = _vertexData.data();
	auto stride = _vertexLayout->stride;

	for (auto& e : _vertexLayout->elements) {
		using S	 = VertexSemantic;
		using ST = VertexSemanticType;
		using U	 = VertexSemanticUtil;

		auto semanticType  = U::getType(e.semantic);
		auto semanticIndex = U::getIndex(e.semantic);

		switch (semanticType) {
			case ST::TEXCOORD: {
				if (semanticIndex < EditMesh::kUvCountMax) {
					Helper::copyVertexData(pData, vc, e, stride, src.uv[semanticIndex].data() + offet); break;
				}
				continue;
			} break;
		}

		switch (e.semantic) {
			case S::POSITION: {
//				Helper::copyVertexData(pData, vc, e, stride, src.pos.data());
				const Tuple3f* srcData = src.pos.data();

				u8* dstData = pData + e.offset;
				for (size_t i = 0; i < vc; i++) {
					*reinterpret_cast<Tuple3f*>(dstData) = *srcData;

					_boundingBox.encapsulate(*srcData);

					++srcData;
					dstData += stride;
				}
			} break;
			case S::COLOR0:		Helper::copyVertexData(pData, vc, e, stride, src.color.data() + offet); break;
			case S::NORMAL:		Helper::copyVertexData(pData, vc, e, stride, src.normal.data() + offet); break;
			case S::TANGENT:	Helper::copyVertexData(pData, vc, e, stride, src.tangent.data() + offet); break;
			case S::BINORMAL:	Helper::copyVertexData(pData, vc, e, stride, src.binormal.data() + offet); break;
		}
	}

	setVertexBuffer();
}

void RenderSubMesh::_createIB(const EditMesh& src, size_t ic, size_t offset /*= 0*/) {
	if (ic <= 0)
		return;

	auto span = EditMeshUtil::subIndices(src, offset, ic);

	setIndexData(span);
	setIndexBuffer();
}

void RenderSubMesh::_setVertexBuffer(ByteSpan vertexData) {
	SGE_ASSERT(_vertexCount > 0);
	SGE_ASSERT(!vertexData.empty());

	RenderGpuBuffer::CreateDesc desc;
	desc.type		= RenderGpuBufferType::Vertex;
	desc.bufferSize = vertexData.size();

	_vertexBuffer = Renderer::instance()->createGpuBuffer(desc);
	_vertexBuffer->uploadToGpu(vertexData);
}

void RenderSubMesh::_setIndexBuffer(ByteSpan indexData) {
	SGE_ASSERT(_indexType != RenderDataType::None);
	SGE_ASSERT(_indexCount > 0);
	SGE_ASSERT(!indexData.empty());

	RenderGpuBuffer::CreateDesc desc;
	desc.type		= RenderGpuBufferType::Index;
	desc.bufferSize = indexData.size();

	_indexBuffer = Renderer::instance()->createGpuBuffer(desc);
	_indexBuffer->uploadToGpu(indexData);
}

}