#pragma once

#include "NeHeOGL_RenderDataType.h"

namespace sge {

enum class NeHeOGL_RenderPrimitiveType {
	None,
	Points,
	Lines,
	Triangles,
};

class NeHeOGL_Vertex_PosUv {
public:
	Vec3f pos;
	Vec2f uv;
};

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

class NeHeOGL_Vertex_PosColorUvNormal {
public:
	Vec3f	pos;
	Color4b color;
	Vec2f	uv;
	Vec3f	normal;
};

}