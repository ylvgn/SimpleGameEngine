#pragma once

#include "MyRender_Common.h"

namespace sge {

class MyEditMesh : public NonCopyable {
public:
	static constexpr const u8 kUvCountMax = 8;

	using IndexType = u16;

	void clear();

	void createGrid(int w, int h, float cellSize, const Color4b& c);
	void createOrigin();

	void updateIndex();

	size_t vertexCount() const { return pos.size(); }
	size_t  indexCount() const { return index.size(); }

			Span<IndexType>			indexData()			{ return _indexData.span(); }
	const	Span<const IndexType>	indexData() const	{ return _indexData.span(); }

	RenderPrimitiveType	primitive = RenderPrimitiveType::Triangles;

	Vector<u32>		index;
	Vector<Tuple3f>	pos;
	Vector<Tuple2f>	uv[kUvCountMax];
	Vector<Color4b>	color;
	Vector<Tuple3f>	normal;

	void draw(); // draw by fixed pipeline

	void setColor(const Color4b& c);

private:
	void _drawElement();
	void _drawArray();

	Vector<IndexType> _indexData;
};

}
