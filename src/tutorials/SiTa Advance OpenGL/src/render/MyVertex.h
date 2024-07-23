#pragma once

namespace sge {

struct alignas(16) MyVertex_PosColorUv {
	Vec3f	pos;
	Color4b color;
	Vec2f	uv;
	Vec3f	normal;
};

}