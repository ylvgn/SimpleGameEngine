#pragma once
#include "MathGeometry.h"
#include "BBox3.h"

namespace sge { namespace Math {

template<class T>
struct Frustum3 {
	using Vec3	 = Vec3<T>;
	using Vec4	 = Vec4<T>;
	using Mat4   = Mat4<T>;
	using Plane3 = Plane3<T>;
	using BBox3  = BBox3<T>;
public:
	enum class Side : u8 {
		Near,
		Far,
		Left,
		Right,
		Top,
		Bottom,
		_END
	};

	constexpr static const int kSideNear   = enumInt(Side::Near  );
	constexpr static const int kSideFar    = enumInt(Side::Far   );
	constexpr static const int kSideLeft   = enumInt(Side::Left  );
	constexpr static const int kSideRight  = enumInt(Side::Right );
	constexpr static const int kSideTop    = enumInt(Side::Top   );
	constexpr static const int kSideBottom = enumInt(Side::Bottom);
	constexpr static const int kSideCount  = enumInt(Side::_END  );

	Vec3	points[8];
	Plane3	planes[kSideCount];

	void setByViewProjMatrix(const Mat4& matrix) { setByInvViewProjMatrix(matrix.inverse()); }
	void setByInvViewProjMatrix(const Mat4& invMatrix);

	void setByBBox(const BBox3& bbox, const Mat4& matrix);

	bool isOverlapped(const BBox3& bbox, const Mat4& matrix) const;
	bool isOverlapped(const Frustum3& f) const;

	Plane3&	nearPlane	()	{ return planes[kSideNear  ]; }
	Plane3&	farPlane	()	{ return planes[kSideFar   ]; }
	Plane3&	leftPlane	()	{ return planes[kSideLeft  ]; }
	Plane3&	rightPlane	()	{ return planes[kSideRight ]; }
	Plane3&	topPlane	()	{ return planes[kSideTop   ]; }
	Plane3&	bottomPlane	()	{ return planes[kSideBottom]; }

	const Plane3&	nearPlane	() const { return planes[kSideNear  ]; }
	const Plane3&	farPlane	() const { return planes[kSideFar   ]; }
	const Plane3&	leftPlane	() const { return planes[kSideLeft  ]; }
	const Plane3&	rightPlane	() const { return planes[kSideRight ]; }
	const Plane3&	topPlane	() const { return planes[kSideTop   ]; }
	const Plane3&	bottomPlane	() const { return planes[kSideBottom]; }

private:
	static bool _outsideOfPlane(const Plane3& p, const Vec3 points[8]);
	void _updatePlanesFromPoints();
};

template<class T> inline
bool Frustum3<T>::_outsideOfPlane(const Plane3& p, const Vec3 points[8]) {
	for (size_t i = 0; i < 8; i++) {
		if (p.dot(points[i]) < 0)
			return false;
	}
	return true;
}

template<class T> inline
bool Frustum3<T>::isOverlapped(const BBox3& bbox, const Mat4& matrix) const {
	Frustum3 tmp;
	tmp.setByBBox(bbox, matrix);
	return isOverlapped(tmp);
}

using Frustum3f = Frustum3<float>;
using Frustum3d = Frustum3<double>;

}} // namespace sge/Math