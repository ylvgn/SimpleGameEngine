#pragma once

#include "MyVec3.h"

namespace sge {
namespace Math {

template<typename T>
struct MyLine3 {
	using MyVec3 = MyVec3<T>;

	constexpr MyLine3() noexcept = default;
	constexpr MyLine3(const MyVec3& start_, const MyVec3& end_) noexcept
		: start(start_), end(end_) {}

	MyVec3 start;
	MyVec3 end;
};

template<typename T>
struct MyTriangle3 {
	using MyVec3 = MyVec3<T>;

	constexpr MyTriangle3() noexcept = default;
	constexpr MyTriangle3(const MyVec3& v0_, const MyVec3& v1_, const MyVec3& v2_) noexcept
		: v0(v0_), v1(v1_), v2(v2_) {}

	MyVec3 normal() const {
		MyVec3 v10(v1-v0);
		MyVec3 v20(v2-v0);
		return v10.cross(v20).normalize();
	}

	MyVec3 v0, v1, v2;
};

template<typename T>
struct MyPlane3 {
	using MyVec3 = MyVec3<T>;

	constexpr MyPlane3() noexcept = default;
	constexpr MyPlane3(const MyVec3& normal_, const T& distance_) noexcept
		: normal(normal_), distance(distance_) {}

	constexpr MyPlane3(const MyVec3& normal_, const MyVec3& pos) noexcept
		: normal(normal_), distance(normal_.dot(pos)) {}

	void setByTriangle(const MyVec3& v0, const MyVec3& v1, const MyVec3& v2) {
		MyVec3 v10(v1-v0);
		MyVec3 v20(v2-v0);

		normal	 = v10.cross(v20).normalize();
		distance = normal.dot(v0);
	}

	T dot(const MyVec3& pt) const { return normal.dot(pt) - distance; }

	T		distance;
	MyVec3	normal;
};

template<typename T>
struct MySphere3 {
	using MyVec3 = MyVec3<T>;

	constexpr MySphere3() noexcept = default;
	constexpr MySphere3(const MyVec3& center_, const T& radius_) noexcept
		: center(center_), radius(radius_) {}

	T		radius;
	MyVec3	center;
};

//-----------------
using MyLine3f = MyLine3<float>;
using MyLine3d = MyLine3<double>;

using MyTriangle3f = MyTriangle3<float>;
using MyTriangle3d = MyTriangle3<double>;

using MyPlane3f = MyPlane3<float>;
using MyPlane3d = MyPlane3<double>;

using MySphere3f = MySphere3<float>;
using MySphere3d = MySphere3<double>;

} // namespace Math
} // namespace sge