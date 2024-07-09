#pragma once

#include "NeHeOGL_Vertex.h"

namespace sge {

class NeHeOGL_EditMesh : public NonCopyable {
public:
	using RenderDataType		= NeHeOGL_RenderDataType;
	using RenderPrimitiveType	= NeHeOGL_RenderPrimitiveType;

	static constexpr const u8 kUvCountMax = 4;

	void clear();

	RenderPrimitiveType	primitive = RenderPrimitiveType::Triangles;

	Vector<u32>		indices;

	Vector<Tuple3f>	pos;
	Vector<Tuple2f>	uv[kUvCountMax];
	Vector<Color4b>	color;
	Vector<Tuple3f>	normal;
};

}
