#pragma once

#include "MyMat4.h"

namespace sge {

template<typename T>
struct MyRay3 {
private:
	using MyVec2 = MyVec2<T>;
	using MyVec3 = MyVec3<T>;
	using MyVec4 = MyVec4<T>;
	using MyMat4 = MyMat4<T>;
public:

	MyVec3 origin, dir;

	constexpr MyRay3() noexcept = default;
	constexpr MyRay3(const MyVec3& origin_, const MyVec3& dir_) noexcept
		: origin(origin_), dir(dir_) {}

	constexpr static MyRay3 s_zero() { return MyRay3(MyRay3::s_zero(), MyRay3::s_zero()); }

	SGE_INLINE void set(const MyVec3& origin_, const MyVec3& dir_) noexcept {
		// assume the dir_ is normalized
		origin.set(origin_);
		dir.set(dir_);
	}

	static MyRay3 unprojectFromInverseMatrix(const MyMat4& invProj, const MyMat4& invModelview, const MyVec2& pointOnScreen, const MyVec2& screenSize);
};

using MyRay3f = MyRay3<float>;
using MyRay3d = MyRay3<double>;

}