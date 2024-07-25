#pragma once

namespace sge {

struct alignas(16) MyVertex_PosColorUvNormal {
	Vec3f	pos;
	Color4b color;
	Vec2f	uv;
	Vec3f	normal;
};

struct alignas(16) MyVertexCg_PosColorUvNormal {
	Vec4f	pos;
	Color4f color;
	Vec2f	uv;
	Vec4f	normal;
};

}