#pragma once

#include <sge_game_anime_prog/math/vec3.h>

namespace sge {

class DebugDraw;

struct CubicCurveExample {

#if 0
#pragma mark ================= Bezier ====================
#endif
class Bezier : public NonCopyable {
public:
	Bezier() = default;
	Bezier(const vec3f& p1_, const vec3f& c1_, const vec3f& c2_, const vec3f& p2_) 
		: p1(p1_)
		, c1(c1_)
		, p2(p2_)
		, c2(c2_) {}

	vec3f lerp(float t) const;
	void factor(float t, SPtr<DebugDraw> outPoints) const;

	vec3f p1 {0,0,0};
	vec3f p2 {0,0,0};
	vec3f c1 {0,0,0};
	vec3f c2 {0,0,0};
};

#if 0
#pragma mark ================= Hermite ====================
#endif

class Hermite : public NonCopyable {
public:
	Hermite() = default;
	Hermite(const vec3f& p1_, const vec3f& p2_, const vec3f& tan1_, const vec3f& tan2_)
		: p1(p1_)
		, p2(p2_)
		, tan1(tan1_)
		, tan2(tan2_) {}

	vec3f lerp(float t) const;
	void factor(float t, SPtr<DebugDraw> outPoints) const;

	vec3f p1   {0,0,0};
	vec3f p2   {0,0,0};
	vec3f tan1 {0,0,0};
	vec3f tan2 {0,0,0};
};

}; // namespace

}