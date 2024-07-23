#pragma once

#include "MyEditMesh.h"
#include "MyVertex.h"

namespace sge {

class MyVertexBuffer : public NonCopyable {
public:
	void create(const Span<const MyVertex_PosColorUv> data);
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
	using Test_VertexType = MyVertex_PosColorUv;

	void clear();

	void updateVBO(MyEditMesh& src);

	MyVertexBuffer vertexBuffer;
	MyIndexBuffer   indexBuffer;

	size_t vertexCount = 0;
	size_t indexCount  = 0;

	RenderPrimitiveType primitive = RenderPrimitiveType::Triangles;
};

}