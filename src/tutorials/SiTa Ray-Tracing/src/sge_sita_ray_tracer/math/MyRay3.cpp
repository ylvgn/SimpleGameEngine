#include "MyRay3.h"

namespace sge {

template<typename T>
void MyRay3<T>::HitResult::draw() {
	if (!hasResult)
		return;

	const auto& pt = point;
	const auto& nl = normal;

	{
		Scoped_glBegin begin(GL_POINTS);
		my_glColor4(T(1), T(0), T(0), T(1));
			my_glVertex3(pt.x, pt.y, pt.z);
		my_glColor4(T(1), T(1), T(1), T(1));
	}

	{
		Scoped_glBegin begin(GL_LINES);
		auto ptEd = pt + nl;
		my_glVertex3(pt.x, pt.y, pt.z);
		my_glVertex3(ptEd.x, ptEd.y, ptEd.z);
	}
}


template<typename T>
MyRay3<T> MyRay3<T>::unprojectFromInverseMatrix (const MyMat4& invProj,
												 const MyMat4& invModelview,
												 const MyVec2& pointOnScreen,
												 const MyVec2& screenSize)
{
	auto v2HCS = pointOnScreen / screenSize * T(2) - T(1);
	v2HCS.y = -v2HCS.y;

	auto v4PCS_st = invProj.mulPoint(MyVec4(v2HCS.x, v2HCS.y, T(0), T(1)));
	auto v4PCS_ed = invProj.mulPoint(MyVec4(v2HCS.x, v2HCS.y, T(1), T(1)));

	auto v4LCS_st = invModelview.mulPoint(v4PCS_st);
	auto v4LCS_ed = invModelview.mulPoint(v4PCS_ed);

	MyRay3 o;
	o.origin = v4LCS_st.toVec3();
	o.dir = (v4LCS_ed.toVec3() - o.origin).normalize();
	return o;
}

template<typename T>
bool MyRay3<T>::raycast(HitResult& outResult, const MyPlane3& plane, T maxDistance /*= Math::inf<T>()*/) {
	// https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
	T s = dir.dot(plane.normal);

	if (Math::equals0(s)) {
		return false; // parallel to the plane
	}

	if (s > 0) {
		return false; // back face
	}

	auto distance = -(origin.dot(plane.normal) - plane.distance) / s;
	if (distance < 0 || distance > maxDistance) {
		return false;
	}

	outResult.distance	= distance;
	outResult.point		= origin + dir * distance;
	outResult.normal	= plane.normal;
	outResult.hasResult = true;
	return true;
}

template<typename T>
bool MyRay3<T>::raycast(HitResult& outResult, const MySphere3& sphere, T maxDistance /*= Math::inf<T>()*/) {
	const T& r = sphere.radius;

#if false // no need check
	if (Math::equals0(r) || r < 0)
		return false;
#endif

	auto v = sphere.center - origin; // vector from origin to sphere center
	T t = v.dot(dir); // distance 't' to closest point to sphere center

	if (t < 0)
		return false;

	T d2 = v.dot(v) - (t * t); // squared distance between closest point to sphere center
	T r2 = r * r;

	if (r2 < d2)
		return false;

	T q = Math::sqrt(r2 - d2);

	T t0 = t + q;
	T t1 = t - q;

	bool hasResult = false;

	T dis = Math::inf<T>();
	if (t0 >= 0 && t0 < maxDistance) {
		dis = t0;
		hasResult = true;
	}

	if (t1 >= 0 && t1 < maxDistance) {
		if (!hasResult || t1 < dis) {
			dis = t1;
			hasResult = true;
		}
	}

	if (!hasResult) {
		return false;
	}

	auto pt = origin + dir * dis;

	outResult.distance	= dis;
	outResult.point		= pt;
	outResult.normal	= (pt - sphere.center).normalize();
	outResult.hasResult = true;
	return true;
}

template<typename T>
bool MyRay3<T>::raycast(HitResult& outResult, const MyTriangle3& tri, T maxDistance /*= Math::inf<T>()*/) {
	HitResult r;
	MyPlane3 plane(tri);
	if (!raycast(r, plane, maxDistance))
		return false;

	auto e0 = tri.v1 - tri.v0;
	auto e1 = tri.v2 - tri.v1;
	auto e2 = tri.v0 - tri.v2;

	auto t0 = r.point - tri.v0;
	auto t1 = r.point - tri.v1;
	auto t2 = r.point - tri.v2;

	if ((e0.cross(t0)).dot(r.normal) < 0) return false;
	if ((e1.cross(t1)).dot(r.normal) < 0) return false;
	if ((e2.cross(t2)).dot(r.normal) < 0) return false;

	outResult = r;
	return true;
}

template<typename T>
bool MyRay3<T>::raycast(HitResult& outResult, const MyMesh& mesh, T maxDistance /*= Math::inf<T>()*/) {
	MyAABB3 aabb = MyAABB3::s_cast<T>(mesh.aabb); // how to make it nice ??? cuz mesh aabb is only float
	if (!raycast(aabb, maxDistance)) {
		outResult.hasResult = false;
		return false;
	}
		
	size_t indiceCount = mesh.indices.size();
	size_t trangleCount = indiceCount / 3;

	if (trangleCount <= 0)
		return false;

	HitResult r;
	r.distance = maxDistance;

	MyTriangle3 tmpTri;
	auto* ind = mesh.indices.begin();
	for (int i = 0; i < trangleCount; ++i) {
		tmpTri.v0 = mesh.vertices[*ind].pos; ind++;
		tmpTri.v1 = mesh.vertices[*ind].pos; ind++;
		tmpTri.v2 = mesh.vertices[*ind].pos; ind++;
		raycast(r, tmpTri, r.distance);
	}

	if (!r.hasResult)
		return false;

	outResult = r;
	return true;
}

template<typename T>
bool MyRay3<T>::raycast(const MyAABB3& aabb, T maxDistance /*= Math::inf<T>()*/) {
	if (aabb.empty())
		return false;

	if (aabb.isInside(origin))
		return true;

	HitResult out_hit;
	MyPlane3  plane;

	//--- x ---
	{
		out_hit.reset();
		if (dir.x < 0) {
			plane.normal.set(1,0,0);
			plane.distance = aabb.maxPt.x;
		} 
		else {
			plane.normal.set(-1,0,0);
			plane.distance = -aabb.minPt.x;
		}
		if (raycast(out_hit, plane, maxDistance)) {
			auto y = out_hit.point.y;
			auto z = out_hit.point.z;
			if (y >= aabb.minPt.y && y <= aabb.maxPt.y)
				if (z >= aabb.minPt.z && z <= aabb.maxPt.z)
					return true;
		}
	}

	//--- y ---
	{
		out_hit.reset();
		if (dir.y < 0) {
			plane.normal.set(0, 1, 0);
			plane.distance = aabb.maxPt.y;
		}
		else {
			plane.normal.set(0, -1, 0);
			plane.distance = -aabb.minPt.y;
		}
		if (raycast(out_hit, plane, maxDistance)) {
			auto x = out_hit.point.x;
			auto z = out_hit.point.z;
			if (x >= aabb.minPt.x && x <= aabb.maxPt.x)
				if (z >= aabb.minPt.z && z <= aabb.maxPt.z)
					return true;
		}
	}

	//--- z ---
	{
		out_hit.reset();
		if (dir.z < 0) {
			plane.normal.set(0, 0, 1);
			plane.distance = aabb.maxPt.z;
		}
		else {
			plane.normal.set(0, 0, -1);
			plane.distance = -aabb.minPt.z;
		}
		if (raycast(out_hit, plane, maxDistance)) {
			auto y = out_hit.point.y;
			auto x = out_hit.point.x;
			if (y >= aabb.minPt.y && y <= aabb.maxPt.y)
				if (x >= aabb.minPt.x && x <= aabb.maxPt.x)
					return true;
		}
	}

	return false;
}

template<typename T>
void MyRay3<T>::draw() {
	if (!dir.sqrMagnitude())
		return;

	auto start = origin;
	auto end   = origin + dir;

	{
		glColor4f(1,0,1,1);
		Scoped_glBegin begin(GL_POINTS);
		my_glVertex3v(start.data);
		glColor4f(1,1,1,1);
	}

	{
		Scoped_glBegin begin(GL_LINES);
		my_glVertex3v(start.data);
		my_glVertex3v(end.data);
	}
}


// explicit specialization, and it will help that no need impl template member function in .h file
template struct MyRay3<float>;
template struct MyRay3<double>;

}