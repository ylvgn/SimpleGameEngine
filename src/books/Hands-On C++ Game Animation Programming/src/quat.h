#pragma once

#include "vec3.h"
#include "mat4.h"

namespace sge {

/* FYI:
	- https://gabormakesgames.com/quaternions.html
	- http://www.songho.ca/math/quaternion/quaternion.html#algebra
*/
struct quat {

	using vec3 = TVec3<float>;

	union {

		// x = sin(θ/2) * axisX
		// y = sin(θ/2) * axisY
		// z = sin(θ/2) * axisZ
		// w = cos(θ/2)

		struct { float x, y, z, w; };
		struct {
			vec3  vector; // vector part
			float scalar; // scalar/real part
		};
		
	};

	inline quat() : x(0), y(0), z(0), w(1) {}
	inline quat(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

	inline static quat s_identity() { return quat(0,0,0,1); }

	inline static quat s_angleAxis(float angle, const vec3& axis) {
		/*
			A quaternion can track two full rotations, which is 4π or 720 degrees.
			This makes the period of a quaternion 720 degrees.
			The period of sin/cos is 360 degrees.
			Dividing θ by 2 maps the range of a quaternion to the range of sin/cos.
		*/

		// angle is radians, not degrees

		vec3 na = axis.normalize();
		float s = ::sinf(angle * 0.5f);
		float c = ::cosf(angle * 0.5f);
		return quat(na.x * s, na.y * s, na.z * s, c);
	}

	// To retrieve the axis of rotation, normalize the vector part of the quaternion.
	inline vec3 axis() const { return vector.normalize(); }

	// The angle of rotation is double the inverse cosine of the real component.
	inline float angle() const { return 2.0f * ::acosf(w); }

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
//		NOT commutative: q1*q2 != q2*q1
// 		Quaternion multiplication is reversed (left side multiply)
//			let say combine 2 quat (q1 and q2) to 1 quat
//			if you want to apply q1 first, it would be "result=q2*q1"
#else
		quat result;
		result.scalar = r.scalar * scalar - r.vector.dot(vector);
		result.vector = (vector * r.scalar) + (r.vector * scalar) + r.vector.cross(vector);
		return result;
#endif
	}

	// Multiplying a vector by a quaternion will always yield a vector that is rotated by the quaternion.
	inline vec3 operator* (const vec3& v) const {
		const vec3& qv = vector;
		const float& s = w;

		return (qv * 2.0f*qv.dot(v)) +
			(v * (s*s - qv.dot(qv))) +
			(qv.cross(v) * 2.0f * s);
	}

	inline quat operator* (float s)	const { return quat(x*s, y*s, z*s, w*s); }
	inline quat operator/ (float s)	const { return quat(x/s, y/s, z/s, w/s); }

	inline quat operator- () const { return quat(-x,-y,-z,-w); }

	inline bool operator== (const quat& r) const { return x == r.x && y == r.y && z == r.z && w == r.w; }
	inline bool operator!= (const quat& r) const { return !(this->operator==(r)); }

	inline bool sameOrientation(const quat& r) const {
		return equals(r) || (Math::equals0(x+r.x)
						  && Math::equals0(y+r.y)
						  && Math::equals0(z+r.z)
						  && Math::equals0(w+r.w));
//		The sameOrientation function is not as useful
//		because the rotation that a quaternion takes can be changed if the quaternion is inverted.
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

	inline quat inverse() const {
//		inverse = conjugate()/sqrMagnitude();

		float d = (x*x) + (y*y) + (z*z) + (w*w);
		return quat(-x/d, -y/d, -z/d, w/d);
//		The inverse of a normalized quaternion is its conjugate.
	}

	// Remember that these functions(mix/nlerp/slerp) expect the quaternion to already be in its desired neighborhood.
	// desired neighborhood means that you need to choose short arc or long arc. Generally, shortest arc is desirable.

	// This function achieves the same result as lerp does, but it's not really a lerp function as the quaternion still travels on an arc, and arc is not linear.
	// To avoid any confusion, this function will be called mix, not lerp.
	inline quat mix(const quat& to, float t) const { return *this + (to-*this)*t; }

	inline quat nlerp(const quat& to, float t) const { return (*this + (to-*this)*t).normalize(); }

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
		vec3 _axis = axis();

		float halfCosTheta = ::cosf(t * theta*0.5f);
		float halfSinTheta = ::sinf(t * theta*0.5f);

		return quat(
			_axis.x * halfSinTheta,
			_axis.y * halfSinTheta,
			_axis.z * halfSinTheta,
			halfCosTheta
		);
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
//		1. create a delta quaternion between the two
//		2. adjust the angle of the delta quaternion, which means that raise it to the desired power.
//		3. then concatenate it with the starting quaternion using quaternion multiplication.
	}

	inline static quat s_fromTo(const vec3& from, const vec3& to) {
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
		else if (f.equals(t * -1.0f)) { // 180 degree rotation
			// when opposites of each other, the most orthogonal axis of the from vector can be used to create a pure quaternion

			vec3 ortho(1,0,0); // x-axis is the most orthogonal axis of f vector

			if (fabsf(f.y) < fabsf(f.x)) {
				ortho = vec3(0,1,0); // y-axis is the most orthogonal axis of f vector
			}

			// when from vector is close to XOY-plane
			if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
				ortho = vec3(0,0,1); // z-axis is the most orthogonal axis of f vector
			}

			vec3 axis = f.cross(ortho).normalize();
			return quat(axis.x, axis.y, axis.z, 0);
		}

		vec3 half = (f + t).normalize(); // θ/2 between f and t
		vec3 axis = f.cross(half);
		return quat(axis.x, axis.y, axis.z, f.dot(half));
//		The s_fromTo function is one of the most intuitive ways of creating a quaternion.
	}

	// FYI: https://gabormakesgames.com/blog_quats_create.html#:~:text=%7D-,Look%20At,-To%20implement%20a
	inline static quat s_lookRotation(const vec3& dir, const vec3& up) {
		vec3 f = dir.normalize();	// object space forward vector
		vec3 u = up.normalize();	// desired up vector
		vec3 r = u.cross(f);		// object space right vector
		u = f.cross(r);				// object space up vector(incorrect)

		// From world forward to object forward
		quat f2d = s_fromTo(vec3(0,0,1), f);

		// what direction is the new object up?
		vec3 objectUp = f2d * vec3(0,1,0);
		// From object up to desired up
		quat u2u = s_fromTo(objectUp, u);

		// Rotate to forward direction first, then twist to correct up.
		// Combine rotations (in reverse! forward applied first, then up)
		quat result = f2d * u2u;

		// should be: quat result = u2u * f2d; ??????????
		return result.normalize();
		/*
		The matrix lookAt function creates a view matrix, which is the inverse of the camera transform.
		This means the rotation of lookAtand the result of lookRotation are going to be the inverse of each other.
		*/
	}

	inline static mat4 s_quatToMat4(const quat& q) {
#if 1
		vec3 r = q * vec3(1,0,0);
		vec3 u = q * vec3(0,1,0);
		vec3 f = q * vec3(0,0,1);

		return mat4(
			r.x, r.y, r.z, 0,  // x basis vector
			u.x, u.y, u.z, 0,  // y basis vector
			f.x, f.y, f.z, 0,  // z basis vector
			0,   0,   0,   1); // w basis vector
#else
		// FYI: http://www.songho.ca/math/quaternion/quaternion.html#matrix
		float qxx(q.x * q.x);
		float qyy(q.y * q.y);
		float qzz(q.z * q.z);
		float qxz(q.x * q.z);
		float qxy(q.x * q.y);
		float qyz(q.y * q.z);
		float qwx(q.w * q.x);
		float qwy(q.w * q.y);
		float qwz(q.w * q.z);

		return mat4({ 1.0f - 2.0f * (qyy + qzz), 2.f * (qxy + qwz),       2.f * (qxz - qwy),       0.f },
					{ 2.f * (qxy - qwz),         1.f - 2.f * (qxx + qzz), 2.f * (qyz + qwx),       0.f },
					{ 2.f * (qxz + qwy),         2.f * (qyz - qwx),       1.f - 2.f * (qxx + qyy), 0.f },
					{ 0.f,					     0.f,					  0.f,                     1.f });
#endif
		/*
		Being able to convert quaternions to matrices will be useful later when you need to pass rotation data to a shader.
		Shaders don't know what a quaternion is, but they have built-in functionality to deal with matrices.
		*/
	}

	inline static quat s_mat4ToQuat(const mat4& m) {
		// Using only the forwardand up vectors, the lookRotation function can be used to convert a matrix into a quaternion.
		vec3 up = m.up.xyz().normalize();
		vec3 forward = m.forward.xyz().normalize();
		vec3 right = up.cross(forward);
		up = forward.cross(right);
		return s_lookRotation(forward, up);
		/*
			Converting matrices to quaternions is going to be useful for debugging
			and in the case where an external data source only provides rotations as matrices.
		*/
	}
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

}