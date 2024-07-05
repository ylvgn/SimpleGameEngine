#pragma once

#include "MyQuat4.h"

namespace sge {

template<typename T>
struct MyLinearTransform_Basic {
	using ElementType		= T;
	using MyLinearTransform = MyLinearTransform_Basic;
	using MyVec3			= MyVec3<T>;
	using MyQuat4			= MyQuat4<T>;

	MyVec3  position { 0,0,0 };
	MyQuat4 rotation { 0,0,0,1 };
	MyVec3  scale	 { 1,1,1 };

	MyLinearTransform() = default;
	MyLinearTransform(const MyVec3& t, const MyQuat4& r, const MyVec3& s)
		: position(t)
		, rotation(r)
		, scale(s)
	{}

	MyVec3 forward() const { return rotation * MyVec3::s_forward(); }
	MyVec3 right()	 const { return rotation * MyVec3::s_right(); }
	MyVec3 up()		 const { return rotation * MyVec3::s_up(); }

	MyLinearTransform lerp(const MyLinearTransform& r, T t) const;
	MyLinearTransform inverse() const;

	MyVec3 mulPoint (const MyVec3& v) const { return position + (rotation * (scale * v)); }
	MyVec3 mulVector(const MyVec3& v) const { return rotation * (scale * v); }

	MyLinearTransform operator* (const MyLinearTransform& r);

	SGE_INLINE bool operator==(const MyLinearTransform& r) const { return position == r.position && rotation == r.rotation && scale == r.scale; }
	SGE_INLINE bool operator!=(const MyLinearTransform& r) const { return !this->operator==(r); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "(\n  {},\n  {},\n  {})", position, rotation, scale);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const MyLinearTransform& t) {
		o << "(" \
		<< "\n\t(" << t.position.x << "," << t.position.y << "," << t.position.z << ")" \
		<< "\n\t(" << t.rotation.x << "," << t.rotation.y << "," << t.rotation.z << "," << t.rotation.w << ")" \
		<< "\n\t(" << t.scale.x << "," << t.scale.y << "," << t.scale.z << ")" \
		<< "\n)";
		return o;
	}
};

using MyLinearTransform_Basicf = MyLinearTransform_Basic<float>;
using MyLinearTransform_Basicd = MyLinearTransform_Basic<double>;

SGE_FORMATTER_T(typename T, MyLinearTransform_Basic<T>)

template<typename T> inline
MyLinearTransform_Basic<T> MyLinearTransform_Basic<T>::lerp(const MyLinearTransform& r, T t) const {
	MyQuat4 rRot = r.rotation;
	if (rotation.dot(rRot) < 0) {
		rRot = -rRot;
	}

	return MyLinearTransform_Basic(position.lerp(r.position, t),
								   rotation.nlerp(rRot, t),
								   scale.lerp(r.scale, t));
}

template<typename T> inline
MyLinearTransform_Basic<T> MyLinearTransform_Basic<T>::inverse() const {
	MyLinearTransform_Basic inv;

	T sx, sy, sz;
	sx = sy = sz = T(0);

	inv.rotation = rotation.inverse();

	if (!Math::equals0(scale.x)) { sx = T(1) / scale.x; }
	if (!Math::equals0(scale.y)) { sy = T(1) / scale.y; }
	if (!Math::equals0(scale.z)) { sz = T(1) / scale.z; }
	inv.scale = MyVec3(sx, sy, sz);

	MyVec3 invTranslation = -position;
	inv.position = inv.rotation * (inv.scale * invTranslation);

	return inv;
}

template<typename T> inline
MyLinearTransform_Basic<T> MyLinearTransform_Basic<T>::operator*(const MyLinearTransform& r) {
	MyLinearTransform o;
	o.scale		= r.scale * scale;
	o.rotation	= r.rotation * rotation;
	o.position	= position + (rotation * (scale * r.position));
	return o;
}

}