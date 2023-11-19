#pragma once

#include "quat.h"

namespace sge {
/*
	FYI : https://cs.gmu.edu/~jmlien/teaching/cs451/uploads/Main/dual-quaternion.pdf

	If a dual-quaternion changes over time,
		it might become non-normalized due to a floating-point error.

	If the length of the real part of the dual-quaternion is not 1,
		the dual quaternion needs to be normalized.

	Non-unit dual quaternions can introduce an unwanted skew into the transformation represented by a dual quaternion.

	non-dual part aka real part
*/

struct Transform;

// DualQuaternion
struct dual_quat {

	using vec3 = TVec3<float>;

	static const size_t kElementCount = 8;

	union {
		// 2x4 matrix aka mat2x4
		struct {
			float x,  y,  z,  w;
			float dx, dy, dz, dw;
		};
		struct { quat real; quat dual; };
		float data[kElementCount];
	};
	
	SGE_INLINE dual_quat() = default;
	SGE_INLINE dual_quat(const quat& real_, const quat& dual_) : real(real_), dual(dual_) {}

	SGE_INLINE dual_quat(float x_,	float y_,  float z_,  float w_,
						 float dx_, float dy_, float dz_, float dw_)
		: x(x_),   y(y_),   z(z_),   w(w_)
		, dx(dx_), dy(dy_), dz(dz_), dw(dw_) {}

#if 0
	SGE_INLINE dual_quat(const quat& q, const vec3& t)
		: real(q)
		, dual(q * quat(t.x, t.y, t.z, 0) * 0.5f) {}
#else
	// dual part will calc: 'q * quat(t.x, t.y, t.z, 0)'
	// cuz quat(t.x, t.y, t.z, 0), its w == 0,
	// let's optimize it that no need calc the op when w == 0
	SGE_INLINE dual_quat(const quat& q, const vec3& t)
		: x(q.x)
		, y(q.y)
		, z(q.z)
		, w(q.w)
		, dx( 0.5f * ( t.x*w + t.y*z - t.z*y ))
		, dy( 0.5f * (-t.x*z + t.y*w + t.z*x ))
		, dz( 0.5f * ( t.x*y - t.y*x + t.z*w ))
		, dw(-0.5f * ( t.x*x + t.y*y + t.z*z )) {}
#endif

	SGE_INLINE static dual_quat s_identity() { return dual_quat(0,0,0,1, 0,0,0,0); }
	SGE_INLINE static dual_quat s_zero()	 { return dual_quat(0,0,0,0, 0,0,0,0); }

	SGE_INLINE void set(const quat& real_, const quat& dual_) { real = real_; dual = dual_; }

	SGE_INLINE void set(float x_,  float y_,  float z_,	 float w_,
						float dx_, float dy_, float dz_, float dw_)
	{
		setReal(x_,  y_,  z_,  w_);
		setDual(dx_, dy_, dz_, dw_);
	}

	SGE_INLINE void setReal(float x_,  float y_,  float z_,  float w_)	{ x = x_;	y = y_;   z = z_;	w = w_; }
	SGE_INLINE void setDual(float dx_, float dy_, float dz_, float dw_) { dx = dx_; dy = dy_; dz = dz_;	dw = dw_; }

	SGE_INLINE			float& operator[](int i)		{ SGE_ASSERT(i < kElementCount); return data[i]; }
	SGE_INLINE const	float& operator[](int i) const	{ SGE_ASSERT(i < kElementCount); return data[i]; }

	SGE_INLINE dual_quat operator+ (const dual_quat& r) const { return dual_quat(real+r.real, dual+r.dual); }
	SGE_INLINE dual_quat operator- (const dual_quat& r) const { return dual_quat(real-r.real, dual-r.dual); }

	SGE_INLINE dual_quat operator+ (float s) const { return dual_quat(real+s, dual+s); }
	SGE_INLINE dual_quat operator- (float s) const { return dual_quat(real-s, dual-s); }
	SGE_INLINE dual_quat operator* (float s) const { return dual_quat(real*s, dual*s); }
	SGE_INLINE dual_quat operator/ (float s) const { return dual_quat(real/s, dual/s); }

	SGE_INLINE void operator+= (const dual_quat& r) { real+=r.real, dual+=r.dual; }
	SGE_INLINE void operator-= (const dual_quat& r) { real-=r.real, dual-=r.dual; }

	SGE_INLINE void operator+= (float s) { real+=s, dual+=s; }
	SGE_INLINE void operator-= (float s) { real-=s, dual-=s; }
	SGE_INLINE void operator*= (float s) { real*=s, dual*=s; }
	SGE_INLINE void operator/= (float s) { real/=s, dual/=s; }

	// first making sure both dual quaternions are normalized
	// Remember, multiplication order is left to right (This is the opposite of matrix and quaternion)
	// Q1Q2 = Qr1Qr2 + (Qr1Qd2 + Qd1Qr2)e
	SGE_INLINE dual_quat operator* (const dual_quat& r) const {
		dual_quat lhs = this->normalize();
		dual_quat rhs = r.normalize();
		return dual_quat(lhs.real * rhs.real, (lhs.real * rhs.dual) + (lhs.dual * rhs.real));
	}

	SGE_INLINE bool equals(const dual_quat& r, float epsilon = Math::epsilon<float>()) const;
	SGE_INLINE bool equals0(float epsilon = Math::epsilon<float>()) const;

	SGE_INLINE bool operator== (const dual_quat& r) const { return real == r.real && dual == r.dual; }
	SGE_INLINE bool operator!= (const dual_quat& r) const { return !this->operator==(r); }

	// Since the dot product only considers direction, the dual part of the dual quaternion is not used.
	SGE_INLINE float dot(const dual_quat& r) const { return real.dot(r.real); }

	SGE_INLINE float sqrMagnitude()	const { return dot(*this); }
	SGE_INLINE float magnitude()	const { return Math::sqrt(sqrMagnitude()); }
	SGE_INLINE float lenSq()		const { return sqrMagnitude(); }
	SGE_INLINE float len()			const { return magnitude(); }

	// The dual quaternion conjugate operation is an extension of quaternion conjugates,
	// to find the conjugate of both real and dual parts respectively.
	SGE_INLINE dual_quat conjugate() const { return dual_quat(real.conjugate(), dual.conjugate()); }

	SGE_INLINE dual_quat normalize() const;
	SGE_INLINE void normalized();

	SGE_INLINE quat toRotation() const { return real; }
	SGE_INLINE vec3 toTranslation() const;
	SGE_INLINE void toRotationTranslation(quat& outQuat, vec3& outTranslation) const;
	SGE_INLINE vec3 transformVector(const vec3& vector) const;
	SGE_INLINE vec3 transformPoint(const vec3& point) const;

	// Pointer accessor for direct copying
	SGE_INLINE			float* ptr()		{ return data; } // same as return &x;
	SGE_INLINE const	float* ptr() const	{ return data; }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "dual_qual({}, {}, {}, {}, {}, {}, {}, {})", x, y, z, w, dx, dy, dz, dw);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const dual_quat& q) {
		o << "dual_quat(" \
		<< q.x << ", " \
		<< q.y << ", " \
		<< q.z << ", " \
		<< q.w << ", " \
		<< q.dx << ", " \
		<< q.dy << ", " \
		<< q.dz << ", " \
		<< q.dw << ")";
		return o;
	}

	static dual_quat s_Transform(const Transform& t);
};

using dual_quat4f = dual_quat;

SGE_FORMATTER(dual_quat)

#if 0
#pragma mark ------------------- instance functions -------------------
#endif
SGE_INLINE
bool dual_quat::equals(const dual_quat& r, float epsilon) const {
	return real.equals(r.real, epsilon)
		&& dual.equals(r.dual, epsilon);
}

SGE_INLINE
bool dual_quat::equals0(float epsilon) const {
	return real.equals0(epsilon)
		&& dual.equals0(epsilon);
}


SGE_INLINE
dual_quat dual_quat::normalize() const {
	auto norm = real.sqrMagnitude();
	if (Math::equals0(norm)) {
		// return an invalid result to flag the error
		return s_zero();
	}
	auto m = Math::sqrt(norm);
	auto reciprocal = 1.0f / m;
	return *this * reciprocal;
}

SGE_INLINE
void dual_quat::normalized() {
	auto norm = real.sqrMagnitude();
	if (Math::equals0(norm))
		return;
	auto m = Math::sqrt(norm);
	auto reciprocal = 1.0f / m;
	*this *= reciprocal;
}

SGE_INLINE
vec3f dual_quat::toTranslation() const {
	quat q = real.conjugate() * dual * 2.0f;
	return vec3(q.x, q.y, q.z);
}

SGE_INLINE
void dual_quat::toRotationTranslation(quat& outQuat, vec3& outTranslation) const {
#if 0
	outQuat = toRotation();
	outTranslation = toTranslation();
#else
	outQuat.set(x, y, z, w);

	// translation part will call `real.conjugate() * dual`
	// cuz it will return quat but we only need xyz
	// let's optimize it, only calc xyz component
	constexpr const float kTwo = 2.0f;
	outTranslation.x = kTwo * (-dw*x + dx*w - dy*z + dz*y);
	outTranslation.y = kTwo * (-dw*y + dx*z + dy*w - dz*x);
	outTranslation.z = kTwo * (-dw*z - dx*y + dy*x + dz*w);
#endif
}

SGE_INLINE
vec3f dual_quat::transformVector(const vec3& vector) const {
	return real * vector;
}

SGE_INLINE
vec3f dual_quat::transformPoint(const vec3& point) const {
	// assume this dual quaternion is normalized
	return real * point + toTranslation();
}

}