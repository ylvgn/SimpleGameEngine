#pragma once

#include "../RenderDataType.h"
#include "../Render_Common.h"
#include <sge_render/buffer/RenderGpuBuffer.h>

namespace sge {

// forward declare
class RenderMesh;
class EditMesh;
struct VertexLayout;

class RenderSubMesh {
public:
	void create(const EditMesh& src);
	void clear();

	template<class VertexT>
	VertexT* vertex(int i);

	void setVertexCount(size_t vc) {
		_vertexCount = vc;
		size_t byteSize = vertexLayout()->stride * _vertexCount;
		_vertexData.resize(byteSize);
	}

	void setVertexBuffer() { SGE_ASSERT(_vertexCount > 0 && !_vertexData.empty()); _setVertexBuffer(_vertexData); }

	void setIndexData(const Span<const u16> indexData)	{ _setIndexData(indexData); }
	void setIndexData(const Span<const u32> indexData)	{ _setIndexData(indexData); }

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

	template<class T, class ENABLE = std::enable_if_t< std::is_unsigned_v<T> > >
	void _setIndexData(const Span<const T> indexData);

	void _setVertexBuffer(ByteSpan vertexData);

	template<class VertexT>
	VertexT* _vertex(size_t offset);

	RenderMesh*				_mesh = nullptr;

	Vector<u8, 1024>		_vertexData;
	SPtr<RenderGpuBuffer>	_vertexBuffer;

	RenderDataType			_indexType = RenderDataType::None;
	SPtr<RenderGpuBuffer>	_indexBuffer;

	size_t _vertexCount = 0;
	size_t  _indexCount = 0;

	BBox3f _boundingBox;
}; // RenderSubMesh

class RenderMesh {
public:
	using SubMesh = RenderSubMesh;

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

}; // RenderMesh

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

template<class T, class ENABLE /*= std::enable_if_t< std::is_unsigned_v<T> >*/ > inline
void RenderSubMesh::_setIndexData(const Span<const T> indexData) {
	auto* renderer = Renderer::instance();
	auto byteSpan = ByteSpan_make(indexData);

	RenderGpuBuffer::CreateDesc desc;
	desc.type = RenderGpuBufferType::Index;
	desc.bufferSize = byteSpan.size();

	_indexType = RenderDataTypeUtil::get<T>();
	_indexCount = indexData.size();

	_indexBuffer = renderer->createGpuBuffer(desc);
	_indexBuffer->uploadToGpu(byteSpan);
}

template<> const TypeInfo* TypeOf<RenderMesh>();

} // namespace