#pragma once

#include "NeHeOGL_RenderDataType.h"

namespace sge {

class NeHeOGL_Vertex_PosColor {
public:
	Vec3f	pos;
	Color4b color;
};

class NeHeOGL_Vertex_PosColorUv {
public:
	Vec3f	pos;
	Color4b color;
	Vec2f	uv;
};

}