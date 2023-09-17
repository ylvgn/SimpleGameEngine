#pragma once

#include "vec3.h"

namespace sge {

struct Line {
	inline Line() = default;
	inline Line(const vec3f& start_, const vec3f& end_)
		: start(start_), end(end_) { }

	vec3f start;
	vec3f end;
};

struct Ray {
	inline Ray() = default;
	inline Ray(const vec3f& origin_, const vec3f& direction_)
		: origin(origin_), direction(direction_) { }

	vec3f origin;
	vec3f direction;
};

struct Triangle {
	inline Triangle() = default;
	inline Triangle(const vec3f& v0_, const vec3f& v1_, const vec3f& v2_)
		: v0(v0_), v1(v1_), v2(v2_) { }

	inline void set(const vec3f& v0_, const vec3f& v1_, const vec3f& v2_) {
		v0 = v0_;
		v1 = v1_;
		v2 = v2_;
	}

	inline bool equals(const Triangle& r, float epsilon = Math::epsilon<float>()) const {
		return v0.equals(r.v0, epsilon) && v1.equals(r.v1, epsilon) && v2.equals(r.v2, epsilon);
	}

	inline vec3f normal() const {
		vec3f v10(v1-v0);
		vec3f v20(v2-v0);
		return v10.cross(v20).normalize();
	}

	vec3f v0, v1, v2;
};

struct Plane {
	inline Plane() = default;
	inline Plane(const vec3f& normal_, const vec3f& pos)
		: normal(normal_)
		, distance(normal_.dot(pos)) { }

	inline Plane(const vec3f& normal_, float distance_)
		: normal(normal_)
		, distance(distance_) { }

	inline void setByTriangle(const vec3f& v0, const vec3f& v1, const vec3f& v2) {
		vec3f v10(v1-v0);
		vec3f v20(v2-v0);

		normal		= v10.cross(v20).normalize();
		distance	= normal.dot(v0);
	}

	inline float dot(const vec3f& pt) const { return normal.dot(pt) - distance; }

	vec3f normal;
	float distance;
};

}