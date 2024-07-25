#pragma once

#include "MyEditMesh.h"
#include "MyVertex.h"

namespace sge {

class MyVertexBuffer : public NonCopyable {
public:
	template<class MyVertexT>
	void create(const Span<const MyVertexT> data);

	void destroy();

	void bind	() const { glBindBuffer(GL_ARRAY_BUFFER, _p); }
	void unbind	() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
private:
	GLuint _p = 0;
};

class MyIndexBuffer : public NonCopyable {
public:
	using IndexType = MyEditMesh::IndexType;

	void create(const Span<const IndexType> data);
	void destroy();

	void bind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _p); }
	void unbind	() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
private:
	GLuint _p = 0;
};

class MyRenderMesh {
public:
	using Test_VertexType	= MyVertex_PosColorUvNormal;
	using TestCg_VertexType = MyVertexCg_PosColorUvNormal;
	using IndexType			= MyEditMesh::IndexType;

	void clear();

	void create(MyEditMesh& src);
	void createCg(MyEditMesh& src);

	RenderPrimitiveType primitive = RenderPrimitiveType::Triangles;

	MyVertexBuffer vertexBuffer;
	MyIndexBuffer   indexBuffer;

	size_t vertexCount = 0;
	size_t indexCount  = 0;

private:
	template<class MyVertexT>
	void _updateVBO(const Span<const MyVertexT> vertexData, const Span<const IndexType> indexData) {
		vertexBuffer.create(vertexData);
		indexBuffer.create(indexData);
	}
};

}