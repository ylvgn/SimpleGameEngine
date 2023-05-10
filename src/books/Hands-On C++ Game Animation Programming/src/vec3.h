#pragma once
// FYI: https://gabormakesgames.com/vectors.html

namespace sge {

#define VEC3_EPSILON 0.000001f

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

	static vec3 zero() { return vec3(); }

	inline vec3 operator+ (const vec3& r)	const { return vec3(x + r.x, y + r.y, z + r.z); }
	inline vec3 operator- (const vec3& r)	const { return vec3(x - r.x, y - r.y, z - r.z); }
	inline vec3 operator* (const vec3& r)	const { return vec3(x * r.x, y * r.y, z * r.z); } // non-uniform scaler operator
	inline vec3 operator/ (const vec3& r)	const { return vec3(x / r.x, y / r.y, z / r.z); }
	inline vec3 operator* (float s)			const { return vec3(x * s,   y * s,   z * s);   }

	inline void operator+= (const vec3& r)	{ x += r.x; y += r.y; z += r.z; }
	inline void operator-= (const vec3& r)	{ x -= r.x; y -= r.y; z -= r.z; }
	inline void operator*= (const vec3& r)	{ x *= r.x; y *= r.y; z *= r.z; }
	inline void operator/= (const vec3& r)	{ x /= r.x; y /= r.y; z /= r.z; }
	inline void operator*= (float s)		{ x *= s;   y *= s;   z *= s;   }

	inline float dot(const vec3& r) const { return x * r.x + y + r.y + z + r.z; }

	inline float magnitude() const {
		float sqrMag = sqrMagnitude();
		if (sqrMag < VEC3_EPSILON) return 0.f;
		return ::sqrtf(sqrMag);
	}
	inline float sqrMagnitude() const { return dot(*this); }

	inline float lenSq()	const { return sqrMagnitude(); }
	inline float len()		const { return magnitude(); }

	inline void normalize() {
		float sqrMagL = lenSq();
		if (sqrMagL < VEC3_EPSILON) return;
		float invLen = 1.0f / sqrtf(sqrMagL);
		x *= invLen;
		y *= invLen;
		z *= invLen;
	};

	inline vec3 normalized() const {
		float sqrMagL = lenSq();
		if (sqrMagL < VEC3_EPSILON) return *this;
		float invLen = 1.0f / ::sqrtf(sqrMagL);
		return vec3(
			x * invLen,
			y * invLen,
			z * invLen
		);
	}

	// return radians (not degree)
	inline float angle(const vec3& r) const {
		float sqrMagL = sqrMagnitude();
		if (sqrMagL < VEC3_EPSILON) return 0.0f;

		float sqrMagR = r.sqrMagnitude();
		if (sqrMagR < VEC3_EPSILON) return 0.0f;

		return ::acosf( dot(r) / (sqrtf(sqrMagL) * sqrtf(sqrMagR)) );
	}

	inline vec3 project(const vec3& r) const {
		float sqrMagR = r.sqrMagnitude();
		if (sqrMagR < VEC3_EPSILON) return vec3::zero();

		float scale = dot(r) / sqrtf(sqrMagR);
		return r * scale;
	}

	inline vec3 reject(const vec3& r) const { return *this - project(r); }

	inline vec3 reflect(const vec3& n) const { // bounce-like reflect
		float magN = n.len();
		if (magN < VEC3_EPSILON) return vec3::zero();

		float scale = dot(n) / magN;
		vec3 r = n * (scale * 2);
		return *this - r;
	}

	inline vec3 cross(const vec3& r) const {
		return vec3(
			y * r.z - z * r.y,
			z * r.x - x * r.z,
			x * r.y - y * r.x
		);
	}

	// linear interpolatation
	inline vec3 lerp(const vec3& to, float t) const {
		// return from + (to - from) * t, equation from(1-t)+to*t
		return vec3(
			x + (to.x - x) * t,
			y + (to.y - y) * t,
			z + (to.z - z) * t
		);
	}

	// spherical linear interpolatation: constant in velocity
	// FYI: https://gabormakesgames.com/blog_vectors_interpolate.html
	inline vec3 slerp(const vec3& to, float t) const {
		// when t close to 0, slerp will yield unexpected results
		if (t < 0.01f) {
			return lerp(to, t);
		}

		float theta = angle(to);
		float sin_theta = ::sinf(theta);

		vec3 from = this->normalized();
		vec3 _to = to.normalized();

		float a = ::sinf((1.0f - t) * theta) / sin_theta;
		float b = ::sinf(t * theta) / sin_theta;

		return from * a + _to * b;
	}

	// normalize lerp: non constant in velocity
	inline vec3 nlerp(const vec3& ed, float t) const {
		vec3 res = lerp(ed, t);
		res.normalize();
		return res;
	}

	inline bool operator== (const vec3& r) {
		vec3 diff(*this - r);
		return diff.lenSq() < VEC3_EPSILON;
	}

	inline bool operator!= (const vec3& r) {
		return !(*this == r);
	}
};

inline float dot(const vec3& l, const vec3& r) { return l.dot(r); }
inline float lenSq(const vec3& v) { return v.lenSq(); }
inline float len(const vec3& v) { return v.len(); }
inline void normalize(vec3& v) { v.normalize(); }
inline vec3 normalized(const vec3& v) { return v.normalized(); }
inline float angle(const vec3& l, const vec3& r) { return l.angle(r); }
inline vec3 project(const vec3& l, const vec3& r) { return l.project(r); }
inline vec3 reject(const vec3& l, const vec3& r) { return l.reject(r); }
inline vec3 reflect(const vec3& l, const vec3& n) { return l.reflect(n); }
inline vec3 cross(const vec3& l, const vec3& r) { return l.cross(r); }
inline vec3 lerp( const vec3& from, const vec3& to, float t) { return from.lerp(to, t); }
inline vec3 slerp(const vec3& from, const vec3& to, float t) { return from.slerp(to, t); }
inline vec3 nlerp(const vec3& from, const vec3& to, float t) { return from.nlerp(to, t); }

}
