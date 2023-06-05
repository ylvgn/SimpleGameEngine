#pragma once

#include <sge_game_anime_prog/math/vec3.h>

namespace sge {

class DebugDraw;

struct CubicCurveExample {

	class Bezier : public NonCopyable {
	public:
		Bezier() = default;
		Bezier(const vec3f& p1, const vec3f& c1, const vec3f& c2, const vec3f& p2) { set(p1, c1, c2, p2); }

		vec3f lerp(float t) const;
		void factor(float t, UPtr<DebugDraw>& outPoints) const;

		inline const vec3f& p1() const { return _p1; }
		inline const vec3f& p2() const { return _p2; }
		inline const vec3f& c1() const { return _c1; }
		inline const vec3f& c2() const { return _c2; }

		inline void set(const vec3f& p1, const vec3f& c1, const vec3f& c2, const vec3f& p2) {
			_p1 = p1;
			_p2 = p2;
			_c1 = c1;
			_c2 = c2;
		}

	private:
		vec3f _p1 {0,0,0};
		vec3f _p2 {0,0,0};
		vec3f _c1 {0,0,0};
		vec3f _c2 {0,0,0};
	};


	class Hermite : public NonCopyable {
	public:
		Hermite() = default;
		Hermite(const vec3f& p1, const vec3f& p2, float tan1, float tan2) { set(p1, p2, tan1, tan2); }

		vec3f lerp(float t) const;
		void factor(float t, UPtr<DebugDraw>& outPoints) const;

		inline const vec3f& p1()	const { return _p1; }
		inline const vec3f& p2()	const { return _p2; }
		inline float tan1()			const { return _tan1; }
		inline float tan2()			const { return _tan2; }

		inline void set(const vec3f& p1, const vec3f& p2, float tan1, float tan2) {
			_p1   = p1;
			_p2   = p2;
			_tan1 = tan1;
			_tan2 = tan2;
		}

	private:
		vec3f _p1 {0,0,0};
		vec3f _p2 {0,0,0};;
		float _tan1 = 0;
		float _tan2 = 0;
	};
};




}