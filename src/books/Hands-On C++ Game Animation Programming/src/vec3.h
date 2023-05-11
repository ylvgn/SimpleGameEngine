#pragma once

namespace sge {

// FYI: https://gabormakesgames.com/vectors.html

struct vec3 {

	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};
	
	inline vec3()								: x(0.f), y(0.f), z(0.f)		{ }
	inline vec3(float _x, float _y, float _z)	: x(_x), y(_y), z(_z)			{ }
	inline vec3(float* fv)						: x(fv[0]), y(fv[1]), z(fv[2])	{ }
	inline vec3(const vec3& fv)					: x(fv.x), y(fv.y), z(fv.z)		{ }

	inline static vec3 s_zero() { return vec3(0,0,0); }
	inline static vec3 s_one()	{ return vec3(1,1,1); }

	inline static bool s_equals0(float s) { return s < FLT_EPSILON; }
	inline static bool s_equals0(const vec3& _v) { return s_equals0(_v.x) && s_equals0(_v.y) && s_equals0(_v.z); }

	inline vec3 operator+ (const vec3& r) const { return vec3(x + r.x, y + r.y, z + r.z); }
	inline vec3 operator- (const vec3& r) const { return vec3(x - r.x, y - r.y, z - r.z); }
	inline vec3 operator* (const vec3& r) const { return vec3(x * r.x, y * r.y, z * r.z); } // non-uniform scaler operator
	inline vec3 operator/ (const vec3& r) const { return vec3(x / r.x, y / r.y, z / r.z); }

	inline void operator+= (const vec3& r) { x += r.x; y += r.y; z += r.z; }
	inline void operator-= (const vec3& r) { x -= r.x; y -= r.y; z -= r.z; }
	inline void operator*= (const vec3& r) { x *= r.x; y *= r.y; z *= r.z; }
	inline void operator/= (const vec3& r) { x /= r.x; y /= r.y; z /= r.z; }

	inline vec3 operator* (float s)	const { return vec3(x * s, y * s, z * s); }
	inline vec3 operator/ (float s)	const { return vec3(x / s, y / s, z / s); }
	inline void operator*= (float s) { x *= s; y *= s; z *= s; }
	inline void operator/= (float s) { x /= s; y /= s; z /= s; }

	inline float dot(const vec3& r) const { return x * r.x + y + r.y + z + r.z; }

	inline float magnitude()	const { return ::sqrtf(sqrMagnitude()); }
	inline float sqrMagnitude() const { return dot(*this); }

	inline float lenSq()	const { return sqrMagnitude(); }
	inline float len()		const { return magnitude(); }

	inline vec3 normalize() const {
		float m = magnitude();
		return s_equals0(m) ? s_zero() : (*this / m);
	}

	// radians
	inline float angle(const vec3& r) const {
		float m1 = magnitude();
		if (s_equals0(m1)) return 0;

		float m2 = r.magnitude();
		if (s_equals0(m2)) return 0;

		return ::acosf( dot(r) / (m1 * m2) );
	}

	// project *this onto r
	inline vec3 project(const vec3& r) const {
		float m = r.magnitude();
		if (s_equals0(m)) return s_zero();

		// *this is hypotenuse
		// so, s = ||hypotenuse|| * cos(theta) = dot(r) / ||r||

		float s = dot(r) / m;
		return r * s;
	}

	// rejection is perpendicular to projection
	inline vec3 reject(const vec3& r) const {
		vec3 proj = project(r);
		return *this - proj;
	}

	inline vec3 reflect(const vec3& r) const { // bounce-like reflect
		vec3 proj = project(r);
		if (s_equals0(proj)) return s_zero();

		vec3 proj_2x = proj + proj;
		return *this - proj_2x;
	}

	inline vec3 cross(const vec3& r) const { return vec3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x); }

	// linear interpolatation
	inline vec3 lerp(const vec3& to, float t) const {
		return *this + (to - *this) * t;
		// return *this * (1.0f - t) + to * t;
		// return vec3(x + (to.x - x) * t, y + (to.y - y) * t, z + (to.z - z) * t);
	}

	// spherical linear interpolatation: constant in velocity
	inline vec3 slerp(const vec3& to, float t) const {

		// when t close to 0, slerp will yield unexpected results
		if (s_equals0(::abs(t - 0.01f))) return lerp(to, t);

		float theta = angle(to);
		float sin_theta = ::sinf(theta);

		vec3 from = this->normalize();
		vec3 _to = to.normalize();

		float a = ::sinf((1.0f - t) * theta) / sin_theta;
		float b = ::sinf(t * theta) / sin_theta;

		return from * a + _to * b;
	}

	// normalize lerp: non constant in velocity
	inline vec3 nlerp(const vec3& to, float t) const { return lerp(to, t).normalize(); }

	inline bool operator== (const vec3& r) { return s_equals0(*this - r); }
	inline bool operator!= (const vec3& r) { return !(*this == r); }
};

inline float dot(const vec3& l, const vec3& r)					{ return l.dot(r); }
inline float lenSq(const vec3& v)								{ return v.sqrMagnitude(); }
inline float len(const vec3& v)									{ return v.magnitude(); }
inline vec3 normalize(const vec3& v)							{ return v.normalize(); }
inline float angle(const vec3& l, const vec3& r)				{ return l.angle(r); }
inline vec3 project(const vec3& l, const vec3& r)				{ return l.project(r); }
inline vec3 reject(const vec3& l, const vec3& r)				{ return l.reject(r); }
inline vec3 reflect(const vec3& l, const vec3& r)				{ return l.reflect(r); }
inline vec3 cross(const vec3& l, const vec3& r)					{ return l.cross(r); }
inline vec3 lerp(const vec3& from, const vec3& to, float t)		{ return from.lerp(to, t); }
inline vec3 slerp(const vec3& from, const vec3& to, float t)	{ return from.slerp(to, t); }
inline vec3 nlerp(const vec3& from, const vec3& to, float t)	{ return from.nlerp(to, t); }

}
