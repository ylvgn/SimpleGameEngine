#pragma once

#include "MyRender_Common.h"

namespace sge {

class MyEditMesh : public NonCopyable {
public:
	static constexpr const u8 kUvCountMax = 8;

	void clear();

	RenderPrimitiveType	primitive = RenderPrimitiveType::Triangles;

	Vector<u32>		indices;

	Vector<Tuple3f>	pos;
	Vector<Tuple2f>	uv[kUvCountMax];
	Vector<Color4b>	color;
	Vector<Tuple3f>	normal;
};

}
