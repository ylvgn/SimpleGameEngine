#include "MyRay3.h"

namespace sge {

template<typename T>
MyRay3<T> MyRay3<T>::unprojectFromInverseMatrix (
	const MyMat4& invProj,
	const MyMat4& invModelview,
	const MyVec2& pointOnScreen,
	const MyVec2& screenSize)
{
	auto v2HCS = pointOnScreen / screenSize * T(2) - T(1);
	v2HCS.y = -v2HCS.y;

	auto v4PCSSt = invProj.mulPoint(MyVec4(v2HCS.x, v2HCS.y, 0, 1));
	auto v4PCSEd = invProj.mulPoint(MyVec4(v2HCS.x, v2HCS.y, 1, 1));

	auto v4LCSSt = invModelview.mulPoint(v4PCSSt);
	auto v4LCSEd = invModelview.mulPoint(v4PCSEd);

	MyRay3 o;
	o.origin = v4LCSSt.toVec3();
	o.dir = (v4LCSEd.toVec3() - o.origin).normalize();
	return o;
}

}