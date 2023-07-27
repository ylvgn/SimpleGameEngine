#include "CubicCurveExample.h"
#include <sge_game_anime_prog/opengl/DebugDraw.h>

namespace sge {

vec3f CubicCurveExample::Bezier::lerp(float t) const {
#if 0
	vec3f a = p1.lerp(c1, t);
	vec3f b = c1.lerp(c2, t);
	vec3f c = c2.lerp(p2, t);

	vec3f d = a.lerp(b, t);
	vec3f e = b.lerp(c, t);

	return d.lerp(e, t);
#else // better one
	float u   = 1 - t;
	float uu  = u * u;
	float uuu = uu * u;
	float tt  = t * t;
	float ttt = tt * t;

	return (_p1*uuu) + (_c1*3*uu*t) + (_c2*3*u*tt) + (_p2*ttt);
#endif
}

void CubicCurveExample::Bezier::factor(float t, UPtr<DebugDraw>& outPoints) const {
	float u   = 1 - t;
	float uu  = u * u;
	float uuu = uu * u;
	float tt  = t * t;
	float ttt = tt * t;

	outPoints->push_back(vec3f(t, uuu, 0));
	outPoints->push_back(vec3f(t, 3*uu*t, 0));
	outPoints->push_back(vec3f(t, 3*u*tt, 0));
	outPoints->push_back(vec3f(t, ttt, 0));
}

vec3f CubicCurveExample::Hermite::lerp(float t) const {
#if 0
	float u  = 1 - t;
	float uu = u * u;
	float tt = t * t;
	float t2 = 2 * t;
	return (_p1 * (1+t2)*uu) + (_tan1*t*uu) + (_p2*tt*(3-t2)) + (_tan2*tt*(t-1));
#else
	float tt = t * t;
	float tt2 = tt * 2;
	float tt3 = tt * 3;
	float ttt = tt * t;
	float ttt2 = ttt * 2;
	return _p1*(ttt2-tt3+1) + _tan1*(ttt-tt2+t) + _p2*(-ttt2+tt3) + _tan2*(ttt-tt);
#endif
}

void CubicCurveExample::Hermite::factor(float t, UPtr<DebugDraw>& outPoints) const {
	float u  = 1 - t;
	float uu = u * u;
	float tt = t * t;
	float t2 = 2 * t;

	outPoints->push_back(vec3f(t, (1+t2)*uu, 0));
	outPoints->push_back(vec3f(t, t*uu, 0));
	outPoints->push_back(vec3f(t, tt*(3-t2), 0));
	outPoints->push_back(vec3f(t, tt*(t-1), 0));
}

}
