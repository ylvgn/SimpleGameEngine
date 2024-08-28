#pragma once

#include "../RenderDataType.h"
#include "../Render_Common.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

class RenderMesh;
class EditMesh;
struct VertexLayout;

#if 0
#pragma mark ========= RenderSubMesh ============
#endif	
class RenderSubMesh {
public:
	void create(const EditMesh& src);
	void clear();

	template<class VertexT>
	VertexT* vertex(int i);

	void setVertexCount(size_t vc);
	void  setIndexCount(size_t ic);

	void setVertexBuffer(bool isKeepData = false);
	void  setIndexBuffer(bool isKeepData = false);

	void setIndexData(const Span<const u16> indexData);
	void setIndexData(const Span<const u32> indexData);

	RenderPrimitiveType primitive()		const;
	const VertexLayout* vertexLayout()	const;

	RenderGpuBuffer*	vertexBuffer()	const	{ return constCast(_vertexBuffer); }
	RenderGpuBuffer*	indexBuffer()	const	{ return constCast(_indexBuffer); }
	size_t				vertexCount()	const	{ return _vertexCount; }
	size_t				indexCount()	const	{ return _indexCount; }
	RenderDataType		indexType()		const	{ return _indexType; }

	const BBox3f&		boundingBox()	const	{ return _boundingBox; }

friend class RenderMesh;
protected:
	void _createVB(const EditMesh& src, size_t vertexCount);
	void _createIB(const EditMesh& src, size_t  indexCount);

	void _setVertexBuffer(ByteSpan vertexData);
	void  _setIndexBuffer(ByteSpan indexData);

	template<class VertexT>
	VertexT* _vertex(size_t offset);

	template<class DST, class SRC>
	void _setIndexData(const Span<const SRC>& indexData);

	RenderMesh*				_mesh = nullptr;

	RenderDataType			_indexType = RenderDataType::None;

	Vector<u8>				_vertexData;
	Vector<u8>				_indexData;

	SPtr<RenderGpuBuffer>	_vertexBuffer;
	SPtr<RenderGpuBuffer>	_indexBuffer;

	size_t _vertexCount = 0;
	size_t  _indexCount = 0;

	BBox3f _boundingBox;
};

#if 0
#pragma mark ========= RenderMesh ============
#endif	
class RenderMesh {
public:
	using SubMesh = RenderSubMesh;

	static constexpr size_t kSubMeshMaxVertexCount = 0x8000;

	void create(const EditMesh& src);
	void clear();

	RenderPrimitiveType primitive()		const		{ return _primitive; }
	const VertexLayout* vertexLayout()	const		{ return _vertexLayout; }

	Span<      SubMesh>	subMeshes()					{ return _subMeshes; }
	Span<const SubMesh>	subMeshes()		const		{ return _subMeshes; }

	void setPrimitiveType(RenderPrimitiveType t)	{ _primitive = t; }
	void setVertexLayout(const VertexLayout* v)		{ _vertexLayout = v; }
	void setSubMeshCount(size_t newSize);

private:
	const VertexLayout* _vertexLayout = nullptr;

	Vector<SubMesh, 1>	_subMeshes;
	RenderPrimitiveType _primitive = RenderPrimitiveType::Triangles;
};

SGE_INLINE RenderPrimitiveType RenderSubMesh::primitive() const {
	return _mesh->primitive();
}

SGE_INLINE const VertexLayout* RenderSubMesh::vertexLayout() const {
	return _mesh->vertexLayout();
}

template<class VertexT> inline
VertexT* RenderSubMesh::vertex(int i) {
	static_assert(std::is_base_of<VertexBase, VertexT>::value, "");

	size_t offset = vertexLayout()->stride * i;
	return _vertex<VertexT>(offset);
}

template<class VertexT> inline
VertexT* RenderSubMesh::_vertex(size_t offset) {
	if (offset < 0 || offset >= _vertexData.size())
		throw SGE_ERROR("out of range");

	void* buf = _vertexData.data() + offset;
	auto* res = new (buf) VertexT();
	return res;
}

template<class DST, class SRC> inline
void RenderSubMesh::_setIndexData(const Span<const SRC>& indexData) {
	size_t stride = sizeof(DST);

	u8* p = _indexData.begin();
	for (const SRC& src : indexData) {
		*reinterpret_cast<DST*>(p) = static_cast<DST>(src);
		p += stride;
	}
	SGE_ASSERT(_indexData.end() == p);
}

template<> const TypeInfo* TypeOf<RenderMesh>();

} // namespace