#pragma once
// Reference: https://gabormakesgames.com/vectors.html

namespace sge {

#define VEC3_EPSILON 0.000001f

struct vec3 {

	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};
	
	inline vec3()								: x(0.f), y(0.f), z(0.f) { }
	inline vec3(float _x, float _y, float _z)	: x(_x), y(_y), z(_z) { }
	inline vec3(float* fv)						: x(fv[0]), y(fv[1]), z(fv[2]) { }
	inline vec3(const vec3& fv)					: x(fv.x), y(fv.y), z(fv.z) { }

	static vec3 zero() { return vec3(0, 0, 0); }

	inline vec3 operator+ (const vec3& r)	const { return vec3(x + r.x, y + r.y, z + r.z); }
	inline vec3 operator- (const vec3& r)	const { return vec3(x - r.x, y - r.y, z - r.z); }
	inline vec3 operator* (const vec3& r)	const { return vec3(x * r.x, y * r.y, z * r.z); } // non-uniform scale operator
	inline vec3 operator/ (const vec3& r)	const { return vec3(x / r.x, y / r.y, z / r.z); }
	inline vec3 operator* (float f)			const { return vec3(x * f, y * f, z * f); }

	inline void operator+= (const vec3& r)	{ x += r.x; y += r.y; z += r.z; }
	inline void operator-= (const vec3& r)	{ x -= r.x; y -= r.y; z -= r.z; }
	inline void operator*= (const vec3& r)	{ x *= r.x; y *= r.y; z *= r.z; }
	inline void operator/= (const vec3& r)	{ x /= r.x; y /= r.y; z /= r.z; }
	inline void operator*= (float f)		{ x *= f; y *= f; z *= f; }

	inline float dot(const vec3& r) const { return x * r.x + y + r.y + z + r.z; }

	inline float lenSq() const { return x * x + y * y + z * z; }

	inline float len() const {
		float sqMagL = lenSq();
		if (sqMagL < VEC3_EPSILON) return 0.0f;
		return sqrtf(sqMagL);
	}

	inline void normalize() {
		float sqMagL = lenSq();
		if (sqMagL < VEC3_EPSILON) return;
		float invLen = 1.0f / sqrtf(sqMagL);
		x *= invLen;
		y *= invLen;
		z *= invLen;
	};

	inline vec3 normalized() const {
		float sqMagL = lenSq();
		if (sqMagL < VEC3_EPSILON) return *this;
		float invLen = 1.0f / ::sqrtf(sqMagL);
		return vec3(
			x * invLen,
			y * invLen,
			z * invLen
		);
	}

	// radians
	inline float angle(const vec3& r) const {
		float sqMagL = lenSq();
		if (sqMagL < VEC3_EPSILON) return 0.0f;

		float sqMagR = r.lenSq();
		if (sqMagR < VEC3_EPSILON) return 0.0f;

		return ::acosf( dot(r) / (sqrtf(sqMagL) * sqrtf(sqMagR)) );
	}

	inline vec3 project(const vec3& r) const {
		float sqMagR = r.lenSq();
		if (sqMagR < VEC3_EPSILON) return vec3::zero();

		float scale = dot(r) / sqrtf(sqMagR);
		return r * scale;
	}

	inline vec3 reject(const vec3& r) const { return this->operator-(project(r)); }

	inline vec3 reflect(const vec3& n) const { // bounce-like reflect
		float magN = n.len();
		if (magN < VEC3_EPSILON) return vec3::zero();

		float scale = dot(n) / magN;
		vec3 r = n * (scale * 2);
		return this->operator-(r);
	}

	inline vec3 cross(const vec3& r) const {
		return vec3(
			y * r.z - z * r.y,
			z * r.x - x * r.z,
			x * r.y - y * r.x
		);
	}

	// linear interpolatation
	inline vec3 lerp(const vec3& ed, float t) const {
		return vec3(
			x + (ed.x - x) * t,
			y + (ed.y - y) * t,
			z + (ed.z - z) * t
		);
	}

	// spherical linear interpolatation: constant in velocity
	inline vec3 slerp(const vec3& ed, float t) const {
		// when t close to 0, slerp will yield unexpected results
		if (t < 0.01f) {
			return lerp(ed, t);
		}

		// formular: (sin((1 - t)*theta) / sin(theta)) * Vst + (sin(t * theta) / sin(theta) * Ved)
		vec3 from = this->normalized();
		vec3 to = ed.normalized();
		float theta = from.angle(to);
		float sin_theta = ::sinf(theta);
		float a = ::sinf((1.0f - t) * theta) / sin_theta;
		float b = ::sinf(t * theta) / sin_theta;
		return from * a + to * b;
	}

	// normalize lerp: non constant in velocity
	inline vec3 nlerp(const vec3& ed, float t) const {
		vec3 res = lerp(ed, t);
		res.normalize();
		return res;
	}

	inline bool operator== (const vec3& r) {
		vec3 diff(this->operator-(r));
		return diff.lenSq() < VEC3_EPSILON;
	}

	inline bool operator!= (const vec3& r) {
		return !this->operator==(r);
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
inline vec3 lerp(const vec3& st, const vec3& ed, float t) { return st.lerp(ed, t); }

}
