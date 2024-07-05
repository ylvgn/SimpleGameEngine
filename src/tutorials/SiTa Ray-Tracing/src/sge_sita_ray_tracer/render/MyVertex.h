#pragma once

#include <sge_core/graph/Color.h>
#include <sge_sita_ray_tracer/math/MyVec3.h>

namespace sge {

class alignas(4) MyVertex_PosColorUv {
public:
	MyVec3f pos;
	Color4b color;
	MyVec2f uv;
	MyVec3f normal;
};

}