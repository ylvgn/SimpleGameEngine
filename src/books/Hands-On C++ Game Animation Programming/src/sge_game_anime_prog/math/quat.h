#pragma once

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

	SGE_INLINE quat() = default;
	SGE_INLINE quat(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

	SGE_INLINE			float& operator[](int i)		{ SGE_ASSERT(i < kElementCount); return data[i]; }
	SGE_INLINE const	float& operator[](int i) const  { SGE_ASSERT(i < kElementCount); return data[i]; }

	SGE_INLINE void set(float x_, float y_, float z_, float w_) { x = x_; y = y_; z = z_; w = w_; }

	// Quaternions have a multiplicitive identity. This special identity quaternion represents no rotation
	SGE_INLINE static quat s_identity() { return quat(0,0,0,1); }
	SGE_INLINE static quat s_zero()		{ return quat(0,0,0,0); }

	SGE_INLINE static quat s_angleAxis(float angle, const vec3& axis);

	// The angle(radians) of rotation is double the inverse cosine of the real component.
	SGE_INLINE float angle() const { return 2.0f * Math::acos(w); }
	SGE_INLINE vec3  axis(bool isNomalized = false) const;

	SGE_INLINE bool equals(const quat& r, float epsilon = Math::epsilon<float>()) const;
	SGE_INLINE bool equals0(float epsilon = Math::epsilon<float>()) const;

	SGE_INLINE quat operator+ (const quat& r) const { return quat(x+r.x, y+r.y, z+r.z, w+r.w); }
	SGE_INLINE quat operator- (const quat& r) const { return quat(x-r.x, y-r.y, z-r.z, w-r.w); }

	SGE_INLINE quat operator+ (float s)	const	{ return quat(x+s, y+s, z+s, w+s); }
	SGE_INLINE quat operator- (float s)	const	{ return quat(x-s, y-s, z-s, w-s); }
	SGE_INLINE quat operator* (float s)	const	{ return quat(x*s, y*s, z*s, w*s); }
	SGE_INLINE quat operator/ (float s)	const	{ return quat(x/s, y/s, z/s, w/s); }

	SGE_INLINE void operator+= (float s)		{ x += s; y += s; z += s; w += s; }
	SGE_INLINE void operator-= (float s)		{ x -= s; y -= s; z -= s; w -= s; }
	SGE_INLINE void operator*= (float s)		{ x *= s; y *= s; z *= s; w *= s; }
	SGE_INLINE void operator/= (float s)		{ x /= s; y /= s; z /= s; w /= s; }

	SGE_INLINE void operator+= (const quat& r)	{ x+=r.x; y+=r.y; z+=r.z; w+=r.w; }
	SGE_INLINE void operator-= (const quat& r)	{ x-=r.x; y-=r.y; z-=r.z; w-=r.w; }

	SGE_INLINE quat operator* (const quat& r) const;
	SGE_INLINE vec3 operator* (const vec3& v) const;

	SGE_INLINE quat operator^ (float t) const;

	SGE_INLINE quat operator- () const { return quat(-x, -y, -z, -w); }

	SGE_INLINE bool operator== (const quat& r) const { return x == r.x && y == r.y && z == r.z && w == r.w; }
	SGE_INLINE bool operator!= (const quat& r) const { return !this->operator==(r); }

	// The conjugate of a quaternion flips its axis of rotation
	SGE_INLINE quat conjugate() const { return quat(-x, -y, -z, w); }

	// Like with vectors, the dot product measures how similar two quaternions are.
	SGE_INLINE float dot(const quat& r) const { return (x*r.x) + (y*r.y) + (z*r.z) + (w*r.w); }

	SGE_INLINE float sqrMagnitude()	const { return dot(*this); }
	SGE_INLINE float magnitude()	const { return Math::sqrt(sqrMagnitude()); }
	SGE_INLINE float lenSq()		const { return sqrMagnitude(); }
	SGE_INLINE float len()			const { return magnitude(); }

	SGE_INLINE quat normalize() const;
	SGE_INLINE void normalized();

	SGE_INLINE quat inverse() const;

	// unitInverse must assume this quaternion is normalized(unit)
	// unitInverse: The inverse of a normalized quaternion is its conjugate. cuz reciprocal is 1
	SGE_INLINE quat unitInverse() const { return conjugate(); }

	// Remember that these functions(mix/nlerp/slerp) expect the quaternion to already be in its desired neighborhood.
	// desired neighborhood means that you need to choose short-arc or long-arc.
	// Generally, shortest arc is desirable.

	// This function achieves the same result as lerp does, but it's not really a lerp function
		// as the quaternion still travels on an arc, and arc is not linear.
		// To avoid any confusion, this function will be called mix, not lerp.
	SGE_INLINE quat mix(const quat& to, float t)	const { return *this + (to - *this) * t; }
	SGE_INLINE quat lerp(const quat& to, float t)	const { return mix(to, t); } // but i still like call it 'lerp' instand of 'mix'.
	SGE_INLINE quat nlerp(const quat& to, float t)	const { return mix(to, t).normalize(); }

	SGE_INLINE quat slerp(const quat& to, float t) const;
//	SGE_INLINE quat slerp(const quat& to, float t, bool shortestPath = true) const;

	// The sameOrientation function is not as useful
		// because the rotation that a quaternion takes can be changed if the quaternion is inverted.
	SGE_INLINE bool sameOrientation(const quat& r) const;

	SGE_INLINE static quat s_fromTo(const vec3& from, const vec3& to);
	SGE_INLINE static quat s_lookRotation(const vec3& dir, const vec3& up);
	static quat s_mat4(const mat4& m);

	// Pointer accessor for direct copying
	SGE_INLINE			float* ptr()		{ return data; }
	SGE_INLINE const	float* ptr() const	{ return data; }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "quat({}, {}, {}, {})", x, y, z, w);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const quat& q) {
		o << "quat(" \
		<< q.x << ", " \
		<< q.y << ", " \
		<< q.z << ", " \
		<< q.w << ")";
		return o;
	}
};

using quat4f = quat;

SGE_FORMATTER(quat)

#if 0
#pragma mark ================= static function ====================
#endif
SGE_INLINE
quat quat::s_angleAxis(float angle, const vec3& axis) {
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
	return quat(a.x * s, a.y * s, a.z * s, c);
}

SGE_INLINE
quat quat::s_fromTo(const vec3& from, const vec3& to) {
/*
	There is one edge case—what happens when v1 and v2 are parallel? Or if v1== -v2?
	The cross product that's used to find the axis of rotation would yield a 0 vector.
	If this edge case happens, find the most perpendicular vector between the two vectors to create a pure quaternion.
*/
	vec3 f = from.normalize();
	vec3 t = to.normalize();

	if (f.equals(t)) {
		return quat::s_identity(); // no rotation
	}
	else if (f.equals(-t)) { // 180 degree rotation
		// when opposites of each other, the most orthogonal axis of the from vector can be used to create a pure quaternion

		vec3 ortho = vec3::s_right();	// x-axis is the most orthogonal axis of f vector

		if (Math::abs(f.y) < Math::abs(f.x)) {
			ortho = vec3::s_up();		// y-axis is the most orthogonal axis of f vector
		}

		// when from vector is close to XOY-plane
		if (Math::abs(f.z) < Math::abs(f.y) && Math::abs(f.z) < Math::abs(f.x)) {
			ortho = vec3::s_forward(); // z-axis is the most orthogonal axis of f vector
		}

		vec3 axis = f.cross(ortho).normalize();
		return quat(axis.x, axis.y, axis.z, 0);
	}

	vec3 half = (f + t).normalize(); // θ/2 between f and t
	vec3 axis = f.cross(half);
	return quat(axis.x, axis.y, axis.z, f.dot(half));
//	The quat::s_fromTo function is one of the most intuitive ways of creating a quaternion.
}

SGE_INLINE
quat quat::s_lookRotation(const vec3& dir, const vec3& up) {
	vec3 f			= dir.normalize();		// object space forward vector
	vec3 desiredUp	= up.normalize();		// desired up vector
	vec3 r			= desiredUp.cross(f);	// object space right vector

	desiredUp		= f.cross(r);			// object space up vector(incorrect)

	quat f2d		= quat::s_fromTo(vec3::s_forward(), f);// From world forward to object forward
	vec3 objectUp	= f2d * vec3::s_up();					// what direction is the new object up?
	quat u2u		= quat::s_fromTo(objectUp, desiredUp);	// From object up to desired up

	quat result(f2d * u2u);
	return result.normalize();
/*
	The mat4::s_lookAt function creates a view matrix, which is the inverse of the camera's transformation.
	This means the rotation of mat4::s_lookAt and the result of quat::s_lookRotation are going to be the inverse of each other.
	So, quat::s_mat4(mat4::s_lookAt(camera.pos, aim.pos, camera.up)) == inverse(quat::s_lookRotation(aim.pos - camera.pos, camera.up))
*/
}

#if 0
#pragma mark ================= instanse function ====================
#endif
SGE_INLINE
bool quat::equals(const quat& r, float epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon)
		&& Math::equals(w, r.w, epsilon);
}

SGE_INLINE
bool quat::equals0(float epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon)
		&& Math::equals0(w, epsilon);
}

SGE_INLINE
vec3f quat::axis(bool isNomalized/*= false*/) const {
	float sqCos = 1.f - w * w;
	if (sqCos <= 0) { // when θ == 0
		return vec3::s_forward();
	}

	auto a = vec3(x, y, z);
	if (isNomalized) {
		return a.normalize();
	}
	float b = 1.f / Math::sqrt(sqCos);
	return a * b;
}

SGE_INLINE
quat quat::operator* (const quat& r) const {
#if 1
/*
	This implementation is a bit more performant
	since it doesn't need to invoke other functions.
	Rotating by the product lhs*rhs is the same as applying the two rotations in sequence:
		lhs first and then rhs, relative to the reference frame resulting from lhs rotation
		familiar to matrix,
			ex1: TRS, which real algorithm is M=SRT,
				apply scale first, then rotate, last is translate：Mresult = Mtranslate * Mrotate * Mscale
			ex2: MVP, Mresult = Mprojection * Mview * Mmodel;
			ex3: v=MVP*v1 = Mprojection * Mview * Mmodel * v1 -> apply Mmodel first, then Mview, and last is Mprojection
*/
	return quat(r.x*w + r.y*z - r.z*y + r.w*x,
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

	vec3 lv = vec3(x, y, z);		// v
	vec3 rv = vec3(r.x, r.y, r.z);	// v'

	float scalar = w * r.w - lv.dot(rv); // ss' - v.v'
	vec3 vector = rv.cross(lv) + (rv * w) + (lv * r.w); // vXv' + sv' + s'v

	return quat(vector.x, vector.y, vector.z, scalar);
#endif
}

SGE_INLINE
vec3f quat::operator* (const vec3& v) const {

#if 0 // FYI: https://gabormakesgames.com/blog_quats_multiply_vec.html
	// something wrong, but no why ??? when use q2 = q1 * vec3f::right(), then mat4::s_quat(q2)
	vec3 qv(x, y, z);
	return qv * 2.0f * qv.dot(v) +
		v * (w * w - qv.dot(qv)) +
		qv.cross(v) * 2.0f * w;

#endif

#if 0 // MS XNA framework implementation
	float num12 = x + x;
	float num2  = y + y;
	float num   = z + z;
	float num11 = w * num12;
	float num10 = w * num2;
	float num9  = w * num;
	float num8  = x * num12;
	float num7  = x * num2;
	float num6  = x * num;
	float num5  = y * num2;
	float num4  = y * num;
	float num3  = z * num;
	float num15 = ((v.x * ((1.f - num5) - num3)) + (v.y * (num7 - num9))) + (v.z * (num6 + num10));
	float num14 = ((v.x * (num7 + num9)) + (v.y * ((1.f - num8) - num3))) + (v.z * (num4 - num11));
	float num13 = ((v.x * (num6 - num10)) + (v.y * (num4 + num11))) + (v.z * ((1.f - num8) - num5));
	return vec3f(num15, num14, num13);
#endif

#if 1 // nVidia SDK implementation
	vec3 qv(x, y, z);
	auto uv = qv.cross(v);
	auto uuv = qv.cross(uv);
	return v + (uv * w + uuv) * 2;
#endif
}

SGE_INLINE
quat quat::normalize() const {
/*
	To normalize a quaternion, divide each component of the quaternion by its length.
	The resulting quaternion's length will be 1.
*/
	auto norm = sqrMagnitude();
	if (Math::equals0(norm)) {
		// return an invalid result to flag the error
		return s_zero();
	}
	auto m = Math::sqrt(norm);
	auto reciprocal = 1.0f / m;
	return *this * reciprocal;
}

SGE_INLINE
void quat::normalized() {
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

SGE_INLINE
quat quat::inverse() const {
	// inverse = conjugate()/sqrMagnitude();
	auto norm = sqrMagnitude();
	if (Math::equals0(norm)) {
		// return an invalid result to flag the error
		return quat::s_zero();
	}
	auto reciprocal = 1.0f / norm;
	return quat(-x * reciprocal, -y * reciprocal, -z * reciprocal, w * reciprocal);
}

SGE_INLINE
quat quat::operator^(float t) const {
	// FYI: https://gabormakesgames.com/blog_quats_interpolate.html
	// Instead of scaling, the only part of the quaternion that needs to change is it's angle
		// pow(quat, t) = quat(v*sin(t * θ/2), cos(t * θ/2))
		// v means the axis of quat, θ menas the angle of quat

	float theta = angle();
	vec3 a = vec3(x, y, z).normalize();

	float s, c;
	Math::sincos(t * (theta * 0.5f), s, c);
	return quat(a.x * s, a.y * s, a.z * s, c);
}

SGE_INLINE
quat quat::slerp(const quat& to, float t) const {
	// FYI: https://gabormakesgames.com/blog_quats_interpolate.html
	// If the from and to quaternions are very close together, means cos(θ) close to 1
	// slerp tends to produce unexpected results.
	float cosTheta = dot(to);

	if (Math::abs(cosTheta) > (1.0f - Math::epsilon<float>())) {
		// Linear interpolation
		return nlerp(to, t);
	}

	// Only needs to be normalized if inputs are not unit length.
	// slerp(a,b,t)=( (b * a^(-1) )^t )        * a
	//             =( (b * Inverse(a))^t )     * a
	//             =( pow(b * Inverse(a), t) ) * a
	quat delta = inverse() * to;
	return ((delta ^ t) * (*this)).normalize();
}

#if 0
SGE_INLINE
quat quat::slerp(const quat& to, float t, bool shortestPath = true) const {
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
	float theta  = Math::atan2(sinTheta, cosTheta); // angle (radians)
	float invSin = 1.0f / sinTheta;
	float coeffA = Math::sin((1.0f - t) * theta) * invSin;
	float coeffB = Math::sin(t * theta) * invSin;
	return (*this * coeffA) + (b * coeffB);
}
#endif

SGE_INLINE
bool quat::sameOrientation(const quat& r) const {
	return equals(r) || (Math::equals0(x + r.x)
		&& Math::equals0(y + r.y)
		&& Math::equals0(z + r.z)
		&& Math::equals0(w + r.w));
}

}