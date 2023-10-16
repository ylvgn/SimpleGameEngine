#pragma once

#include <sge_core/math/Math.h>
#include "vec4.h"

namespace sge {

/*
	FYI:
	- https://gabormakesgames.com/quaternions.html
	- http://www.songho.ca/math/quaternion/quaternion.html#algebra
*/

struct mat4;

// Quaternion
struct quat {

	using vec3 = TVec3<float>;

	static const size_t kElementCount = 4;

	union {
		struct { float x, y, z, w; };
		float data[kElementCount];
	};

	inline quat() = default;
	inline quat(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

	inline void set(float x_, float y_, float z_, float w_) { x = x_; y = y_; z = z_; w = w_; }

	// Quaternions have a multiplicitive identity. This special identity quaternion represents no rotation
	inline static quat s_identity() { return quat(0,0,0,1); }
	inline static quat s_zero()		{ return quat(0,0,0,0); }

	inline			float& operator[](int i)		{ SGE_ASSERT(i < kElementCount); return data[i]; }
	inline const	float& operator[](int i) const	{ SGE_ASSERT(i < kElementCount); return data[i]; }

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
		vec3 a = axis.normalize();
		return quat(a.x*s, a.y*s, a.z*s, c);
	}

	// To retrieve the axis of rotation, normalize the vector part of the quaternion. maybe no need normalize ???
	inline vec3 axis() const { return vec3(x,y,z).normalize(); }

	// The angle(radians) of rotation is double the inverse cosine of the real component.
	inline float angle() const { return 2.0f * Math::acos(w); }

	inline bool equals (const quat& r,	float epsilon = Math::epsilon<float>()) const;
	inline bool equals0(				float epsilon = Math::epsilon<float>()) const;

	inline quat operator+ (const quat& r) const { return quat(x+r.x, y+r.y, z+r.z, w+r.w); }
	inline quat operator- (const quat& r) const { return quat(x-r.x, y-r.y, z-r.z, w-r.w); }

	inline quat operator* (const quat& r) const {
#if 1
		// This implementation is a bit more performant
		// since it doesn't need to invoke other functions.
		// Rotating by the product lhs*rhs is the same as applying the two rotations in sequence:
			// lhs first and then rhs, relative to the reference frame resulting from lhs rotation
			// familiar to matrix,
				// ex1: TRS, which real algorithm is M=SRT,
					// apply scale first, then rotate, last is translate：Mresult = Mtranslate * Mrotate * Mscale
				// ex2: MVP, Mresult = Mprojection * Mview * Mmodel;
				// ex3: v=MVP*v1 = Mprojection * Mview * Mmodel * v1 -> apply Mmodel first, then Mview, and last is Mprojection
		return quat( r.x*w + r.y*z - r.z*y + r.w*x,
					-r.x*z + r.y*w + r.z*x + r.w*y,
					 r.x*y - r.y*x + r.z*w + r.w*z,
					-r.x*x - r.y*y - r.z*z + r.w*w
		);
/*
		NOT commutative: q1*q2 != q2*q1
 		Quaternion multiplication is reversed (left side multiply)
			let's say combine 2 quat (q1 and q2) to 1 quat
			if you want to out-side apply q1 first, it would be "resultQuat = q2*q1"
				let's say out-side is vec3, such as v2=resultQuat*v1 = q2*q1*v1, so q1 is apply to v1 first
*/
#else
		// FYI: https://gabormakesgames.com/blog_quats_multiply_quat.html
		// qq' = [s,v]*[s',v'] = [(ss'-v.v'), vXv'+sv'+s'v]

		vec3 lv = vec3(x,y,z);                          // v
		vec3 rv = vec3(r.x, r.y, r.z);                  // v'

		float scalar = w*r.w - lv.dot(rv);              // ss' - v.v'
		vec3 vector = rv.cross(lv) + (rv*w) + (lv*r.w); // vXv' + sv' + s'v

		return quat(vector.x, vector.y, vector.z, scalar);
#endif
	}

	inline vec3 operator* (const vec3& v) const {
#if 0
		// something wrong, but no why ??? when use q2 = q1 * vec3f::right(), then mat4::s_quat(q2)
		// FYI: https://gabormakesgames.com/blog_quats_multiply_vec.html
		vec3 qv(x,y,z);
		return qv * 2.0f * qv.dot(v) +
			v * (w*w - qv.dot(qv)) +
			qv.cross(v) * 2.0f * w;
#endif

#if 0
		// MS XNA framework implementation
		float num12 = x + x;
		float num2	= y + y;
		float num	= z + z;
		float num11 = w * num12;
		float num10 = w * num2;
		float num9	= w * num;
		float num8	= x * num12;
		float num7	= x * num2;
		float num6	= x * num;
		float num5	= y * num2;
		float num4	= y * num;
		float num3	= z * num;
		float num15 = ((v.x * ((1.f - num5) - num3)) + (v.y * (num7 - num9))) + (v.z * (num6 + num10));
		float num14 = ((v.x * (num7 + num9)) + (v.y * ((1.f - num8) - num3))) + (v.z * (num4 - num11));
		float num13 = ((v.x * (num6 - num10)) + (v.y * (num4 + num11))) + (v.z * ((1.f - num8) - num5));
		return vec3(num15, num14, num13);
#endif

#if 1
		// nVidia SDK implementation
		vec3 qv(x, y, z);
		auto uv  = qv.cross(v);
		auto uuv = qv.cross(uv);
		return v + (uv * w + uuv) * 2;
#endif
	}

	inline quat operator+ (float s)	const	{ return quat(x+s, y+s, z+s, w+s); }
	inline quat operator- (float s)	const	{ return quat(x-s, y-s, z-s, w-s); }
	inline quat operator* (float s)	const	{ return quat(x*s, y*s, z*s, w*s); }
	inline quat operator/ (float s)	const	{ return quat(x/s, y/s, z/s, w/s); }

	inline void operator+= (float s)		{ x+=s; y+=s; z+=s; w+=s; }
	inline void operator-= (float s)		{ x-=s; y-=s; z-=s; w-=s; }
	inline void operator*= (float s)		{ x*=s; y*=s; z*=s; w*=s; }
	inline void operator/= (float s)		{ x/=s; y/=s; z/=s; w/=s; }

	inline void operator+= (const quat& r)  { x+=r.x; y+=r.y; z+=r.z; w+=r.w; }
	inline void operator-= (const quat& r)  { x-=r.x; y-=r.y; z-=r.z; w-=r.w; }

	inline quat operator-() const			{ return quat(-x,-y,-z,-w); }

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
	inline float dot(const quat& r) const	{ return (x*r.x) + (y*r.y) + (z*r.z) + (w*r.w); }

	inline float sqrMagnitude()	const		{ return dot(*this); }
	inline float magnitude() const			{ return Math::sqrt(sqrMagnitude()); }
	inline float lenSq() const				{ return sqrMagnitude(); }
	inline float len() const				{ return magnitude(); }

	// To normalize a quaternion, divide each component of the quaternion by its length.
	// The resulting quaternion's length will be 1.
	inline quat normalize() const {
		auto norm = sqrMagnitude();
		if (Math::equals0(norm)) {
			// return an invalid result to flag the error
			return s_zero();
		}
		auto m = Math::sqrt(norm);
		auto reciprocal = 1.0f / m;
		return *this * reciprocal;
	}

	inline void normalized() {
		// FYI: https://proofwiki.org/wiki/Definition:Field_Norm_of_Quaternion
		// The norm of a quaternion is it's squared length, so norm == sqrMagnitude
		auto norm = sqrMagnitude();
		if (Math::equals0(norm)) {
			return;
		}
		auto m = Math::sqrt(norm);
		auto reciprocal = 1.0f / m;
		 *this *= reciprocal;
	}

	// The conjugate of a quaternion flips its axis of rotation
	inline quat conjugate() const { return quat(-x,-y,-z,w); }

	inline quat inverse() const {
		// inverse = conjugate()/sqrMagnitude();
		auto norm = sqrMagnitude();
		if (Math::equals0(norm)) {
			// return an invalid result to flag the error
			return quat::s_zero();
		}
		auto reciprocal = 1.0f / norm;
		return quat(-x*reciprocal, -y*reciprocal, -z*reciprocal, w*reciprocal);
//		unitInverse: The inverse of a normalized quaternion is its conjugate. cuz reciprocal is 1
	}

	inline quat unitInverse() const {
		// assume this quaternion is normalized(unit)
		return conjugate();
	}

	// Remember that these functions(mix/nlerp/slerp) expect the quaternion to already be in its desired neighborhood.
	// desired neighborhood means that you need to choose short-arc or long-arc. Generally, shortest arc is desirable.

	// This function achieves the same result as lerp does, but it's not really a lerp function
	// as the quaternion still travels on an arc, and arc is not linear.
	// To avoid any confusion, this function will be called mix, not lerp.
	inline quat mix  (const quat& to, float t)	const { return *this + (to-*this)*t; }
	inline quat lerp (const quat& to, float t)	const { return mix(to, t); } // but i still like call it 'lerp' instand of 'mix'.
	inline quat nlerp(const quat& to, float t)	const { return mix(to, t).normalize(); }

	inline quat nlerp_longway(const quat& to, float t) const {
		// FYI: https://gabormakesgames.com/blog_quats_neighborhood.html

		// If the dot product is negative, the longer arc will be taken.
		float cosTheta = dot(to);
		if (cosTheta < 0) {
			return nlerp(to, t);
		}
		return nlerp(-to, t);
	}

	// pow(quat, t) = quat(v*sin(t * θ/2), cos(t * θ/2))
	// v means the axis of quat, θ menas the angle of quat
	inline quat operator^(float t) const {
		// FYI: https://gabormakesgames.com/blog_quats_interpolate.html
		// Instead of scaling, the only part of the quaternion that needs to change is it's angle
		float theta = angle();
		vec3 a		= vec3(x,y,z).normalize();

		float s, c;
		Math::sincos(t*(theta*0.5f), s, c);
		return quat(a.x*s, a.y*s, a.z*s, c);
	}

#if 1
	inline quat slerp(const quat& to, float t) const {
		// FYI: https://gabormakesgames.com/blog_quats_interpolate.html
		// If the from and to quaternions are very close together, means cos(θ) close to 1
		// slerp tends to produce unexpected results.
		float cosTheta = dot(to);
		
		if (Math::abs(cosTheta) > (1.0f - Math::epsilon<float>()) ) {
			// Linear interpolation
			return nlerp(to, t);
		}

		// Only needs to be normalized if inputs are not unit length.
		// slerp(a,b,t)=( (b * a^(-1) )^t )        * a
		//             =( (b * Inverse(a))^t )     * a
		//             =( pow(b * Inverse(a), t) ) * a
		quat delta = inverse() * to;
		return ( (delta^t) * (*this) ).normalize();
	}
#else
	inline quat slerp(const quat& to, float t, bool shortestPath = true) const {
		// FYI: https://github.com/ehsan/ogre/blob/069a43c4c4fcb5264c995fca65a28acd3154b230/OgreMain/src/OgreQuaternion.cpp#L424
		//                 sin((1-t)θ)            sint(tθ)
		// slerp(a,b,t)= --------------- * a + --------------- * b
        //                   sin(θ)                sin(θ)
		quat b = to;
		float cosTheta = dot(to);

		if (cosTheta < 0 && shortestPath) {
			b = -to;
			cosTheta = -cosTheta;
		}

		if (cosTheta > 1 - Math::epsilon<float>()) {
			// Linear interpolation
			return nlerp(b, t);
		}

		float sinTheta = Math::sqrt(1 - cosTheta * cosTheta);
		float theta    = Math::atan2(sinTheta, cosTheta); // angle (radians)
		float invSin   = 1.0f / sinTheta;
		float coeffA   = Math::sin((1.0f - t) * theta) * invSin;
		float coeffB   = Math::sin(t * theta) * invSin;
		return (*this * coeffA) + (b * coeffB);
	}
#endif

	void onFormat(fmt::format_context& ctx) const;

	inline friend std::ostream& operator<< (std::ostream& o, const quat& q) {
		o << "quat(" \
			<< q.x << ", " \
			<< q.y << ", " \
			<< q.z << ", " \
			<< q.w << ")";
		return o;
	}

	static quat s_fromTo(const vec3& from, const vec3& to);
	static quat s_lookRotation(const vec3& dir, const vec3& up);
	static quat s_mat4(const mat4& m);
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

using quat4f = quat;

SGE_FORMATTER(quat)

}