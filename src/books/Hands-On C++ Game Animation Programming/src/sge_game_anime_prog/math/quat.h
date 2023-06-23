#pragma once

#include <sge_core/math/Math.h>
#include "vec3.h"

namespace sge {

/*
	FYI:
	- https://gabormakesgames.com/quaternions.html
	- http://www.songho.ca/math/quaternion/quaternion.html#algebra
*/

struct mat4;

struct quat {

	using vec3 = TVec3<float>;

	float x, y, z, w;

	inline quat() = default;
	inline quat(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

	inline static quat s_identity() { return quat(0,0,0,1); }
	inline static quat s_zero()		{ return quat(0,0,0,0); }

	inline static quat s_angleAxis(float angle, const vec3& axis) {
/*
		A quaternion can track two full rotations, which is 4π or 720 degrees.
		This makes the period of a quaternion 720 degrees.
		The period of sin/cos is 360 degrees.
		Dividing θ by 2 maps the range of a quaternion to the range of sin/cos.

		angle is radians, not degrees
*/
		float s, c;
		Math::sincos(angle * 0.5f, s, c);
		const vec3 a = axis.normalize();
		return quat(a.x*s, a.y*s, a.z*s, c);
	}

	// To retrieve the axis of rotation, normalize the vector part of the quaternion.
	inline vec3 axis() const { return vec3(x,y,z).normalize(); }

	// The angle of rotation is double the inverse cosine of the real component.
	inline float angle() const { return 2.0f*::acosf(w); }

	inline bool equals(const quat& r, float epsilon = Math::epsilon<float>()) const;
	inline bool equals0(              float epsilon = Math::epsilon<float>()) const;

	inline quat operator+ (const quat& r) const { return quat(x+r.x, y+r.y, z+r.z, w+r.z); }
	inline quat operator- (const quat& r) const { return quat(x-r.x, y-r.y, z-r.z, w-r.z); }
	inline quat operator* (const quat& r) const {
#if 1
		// This implementation is a bit more performant
		// since it doesn't need to invoke other functions.
		return quat( r.x*w + r.y*z - r.z*y + r.w*x,
					-r.x*z + r.y*w + r.z*x + r.w*y,
					 r.x*y - r.y*x + r.z*w + r.w*z,
					-r.x*x - r.y*y - r.z*z + r.w*w
		);
/*
		NOT commutative: q1*q2 != q2*q1
 		Quaternion multiplication is reversed (left side multiply)
			let say combine 2 quat (q1 and q2) to 1 quat
			if you want to apply q1 first, it would be "result=q2*q1"
*/
#else
		// FYI: https://gabormakesgames.com/blog_quats_multiply_quat.html
		vec3 lv = vec3(x,y,z);
		vec3 rv = vec3(r.x, r.y, r.z);
		float lw = w;
		float rw = r.w;

		float scalar = lw*rw - lv.dot(rv);
		vec3 vector = (lv*rw) + (rv*lw) + rv.cross(lv);

		quat result;
		result.w = scalar;
		result.x = vector.x;
		result.y = vector.y;
		result.z = vector.z;
		return result;
#endif
	}

	// Multiplying a vector by a quaternion will always yield a vector that is rotated by the quaternion.
	inline vec3 operator* (const vec3& r) const {
		vec3 qv(x,y,z);

		return (qv * 2.0f*qv.dot(r)) +
			(r * (w*w - qv.dot(qv))) +
			(qv.cross(r) * 2.0f * w);
	}

	inline quat operator* (float s)	const { return quat(x*s, y*s, z*s, w*s); }
	inline quat operator/ (float s)	const { return quat(x/s, y/s, z/s, w/s); }

	inline quat operator- () const { return quat(-x,-y,-z,-w); }

	inline bool operator== (const quat& r) const { return x==r.x && y==r.y && z==r.z && w==r.w; }
	inline bool operator!= (const quat& r) const { return !this->operator==(r); }

	inline bool sameOrientation(const quat& r) const {
		return equals(r) || (Math::equals0(x+r.x)
						  && Math::equals0(y+r.y)
						  && Math::equals0(z+r.z)
						  && Math::equals0(w+r.w));
/*
		The sameOrientation function is not as useful
		because the rotation that a quaternion takes can be changed if the quaternion is inverted.
*/
	}

	// Like with vectors, the dot product measures how similar two quaternions are.
	inline float dot(const quat& r) const { return (x*r.x) + (y*r.y) + (z*r.z) + (w*r.w); }

	inline float magnitude()	const { return Math::sqrt(sqrMagnitude()); }
	inline float sqrMagnitude()	const { return dot(*this); }
	inline float lenSq()		const { return sqrMagnitude(); }
	inline float len()			const { return magnitude(); }

	// To normalize a quaternion, divide each component of the quaternion by its length.The resulting quaternion's length will be 1.
	inline quat normalize() const { float m = magnitude(); return Math::equals0(m) ? s_identity() : (*this / m); }

	// The conjugate of a quaternion flips its axis of rotation
	inline quat conjugate() const { return quat(-x,-y,-z,w); }

//	inverse = conjugate()/sqrMagnitude();
	inline quat inverse() const {
		float d = (x*x) + (y*y) + (z*z) + (w*w);
		return quat(-x/d, -y/d, -z/d, w/d);
//		The inverse of a normalized quaternion is its conjugate.
	}

	// Remember that these functions(mix/nlerp/slerp) expect the quaternion to already be in its desired neighborhood.
	// desired neighborhood means that you need to choose short arc or long arc. Generally, shortest arc is desirable.

	// This function achieves the same result as lerp does, but it's not really a lerp function as the quaternion still travels on an arc, and arc is not linear.
	// To avoid any confusion, this function will be called mix, not lerp.
	inline quat mix(const quat& to, float t) const { return *this + (to-*this)*t; }
	inline quat lerp(const quat& to, float t) const { return mix(to, t); }

	inline quat nlerp(const quat& to, float t) const { return mix(to, t).normalize(); }

	inline quat nlerp_longway(const quat& to, float t) const {
		// FYI: https://gabormakesgames.com/blog_quats_neighborhood.html

		// If the dot product is negative, the longer arc will be taken.
		float cosTheta = dot(to);
		if (cosTheta < 0.0f) {
			return nlerp(to, t);
		}
		return nlerp(-to, t);
	}

	// pow(quat, t) = quat(v*sin(tθ/2), cos(tθ/2))
	// v means the axis of quat, θ menas the angle of quat
	inline quat operator^(float t) const {
		float theta = angle();
		vec3 a = axis();

		float s, c;
		Math::sincos(t * theta*0.5f, s, c);

		return quat(a.x*s, a.y*s, a.z*s, c);
	}

	// FYI: https://gabormakesgames.com/blog_quats_interpolate.html
	inline quat slerp(const quat& to, float t) const {
		// If the from and to quaternions are very close together, means cos(θ) close to 1
		// slerp tends to produce unexpected results.
		float cosTheta = dot(to);
		if (fabsf(cosTheta) > (1.0f - Math::epsilon<float>()) ) {
			return nlerp(to, t);
		}
		
		quat delta = inverse() * to;
		return ( quat(delta^t) * (*this) ).normalize();
/*
		1. create a delta quaternion between the two
		2. adjust the angle of the delta quaternion, which means that raise it to the desired power.
		3. then concatenate it with the starting quaternion using quaternion multiplication.
*/
	}

	static quat s_fromTo(const vec3& from, const vec3& to);
	static quat s_lookRotation(const vec3& dir, const vec3& up);
	static quat s_mat4(const mat4& m);

	void onFormat(fmt::format_context& ctx) const;
};

inline bool quat::equals(const quat& r, float epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon)
		&& Math::equals(w, r.w, epsilon);
}

inline bool quat::equals0(float epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon)
		&& Math::equals0(w, epsilon);
}

SGE_FORMATTER(quat)
}