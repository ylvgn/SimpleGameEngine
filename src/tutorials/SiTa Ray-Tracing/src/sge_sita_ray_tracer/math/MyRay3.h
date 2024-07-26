#pragma once

#include "MyMat4.h"
#include "MyMathGeometry.h"
#include "../render/MyMesh.h"

namespace sge {

template<typename T>
struct MyRay3 {
private:
	using MyVec2 = MyVec2<T>;
	using MyVec3 = MyVec3<T>;
	using MyVec4 = MyVec4<T>;
	using MyMat4 = MyMat4<T>;

	using MyPlane3		= Math::MyPlane3<T>;
	using MySphere3		= Math::MySphere3<T>;
	using MyTriangle3	= Math::MyTriangle3<T>;
	using MyAABB3		= Math::MyAABB3<T>;
public:
	MyVec3 origin, dir;

	struct HitResult {
		HitResult() {
			reset();
		}

		void reset() {
			distance = Math::inf<T>();
			hasResult = false;
		}

		void draw();

		T		distance;
		MyVec3	point;
		MyVec3	normal;
		bool	hasResult : 1;
	};

	constexpr MyRay3() = default;
	constexpr MyRay3(const MyVec3& origin_, const MyVec3& dir_) noexcept
		: origin(origin_), dir(dir_) {} // assume the dir_ is normalized

	constexpr static MyRay3 s_zero() { return MyRay3(MyVec3::s_zero(), MyVec3::s_zero()); }

	constexpr void set(const MyVec3& origin_, const MyVec3& dir_) noexcept {
		// assume the dir_ is normalized
		origin.set(origin_);
		dir.set(dir_);
	}

	static MyRay3 unprojectFromInverseMatrix(const MyMat4& invProj, const MyMat4& invModelview, const MyVec2& pointOnScreen, const MyVec2& screenSize);

	bool raycast(HitResult& outResult, const MyPlane3& plane, T maxDistance = Math::inf<T>());
	bool raycast(HitResult& outResult, const MySphere3& sphere, T maxDistance = Math::inf<T>());
	bool raycast(HitResult& outResult, const MyTriangle3& tri, T maxDistance = Math::inf<T>());
	bool raycast(HitResult& outResult, const MyMesh& mesh, T maxDistance = Math::inf<T>());

	bool raycast(const MyAABB3& aabb, T maxDistance = Math::inf<T>());

	void draw();
};

using MyRay3f = MyRay3<float>;
using MyRay3d = MyRay3<double>;

}