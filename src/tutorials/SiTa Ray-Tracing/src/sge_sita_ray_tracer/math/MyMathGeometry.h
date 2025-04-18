#pragma once

#include "MyVec3.h"

namespace sge {
namespace Math {

template<typename T>
struct MyLine3 {
	using ElementType = T;
	using MyVec3 = MyVec3<T>;

	constexpr MyLine3() = default;
	constexpr MyLine3(const MyVec3& start_, const MyVec3& end_) noexcept
		: start(start_)
		, end(end_)
	{}

	MyVec3 start;
	MyVec3 end;
};

template<typename T>
struct MyTriangle3 {
	using ElementType = T;
	using MyVec3 = MyVec3<T>;

	constexpr MyTriangle3() = default;
	constexpr MyTriangle3(const MyVec3& v0_, const MyVec3& v1_, const MyVec3& v2_) noexcept
		: v0(v0_)
		, v1(v1_)
		, v2(v2_)
	{}

	MyVec3 calcNormal() const {
		return (v1-v0).cross(v2-v0).normalize();
	}

	MyVec3 calcCenter() const {
		return (v0+v1+v2) / T(3);
	}

	void draw();

	MyVec3 v0, v1, v2;
};

template<typename T> inline
void MyTriangle3<T>::draw() {
	using MyVec3 = decltype(v0);

	my_glColor4(T(1), T(1), T(1), T(1));
	glLineWidth(1);

	{
		Scoped_glBegin begin(GL_LINE_LOOP);
		my_glVertex3v(v0.data);
		my_glVertex3v(v1.data);
		my_glVertex3v(v2.data);
	}

	{
		Scoped_glBegin begin(GL_LINES);
		auto c = calcCenter();
		auto nl = calcNormal();
		my_glVertex3v(c.data);
		my_glVertex3v((c+nl).data);
	}
}

template<typename T>
struct MyPlane3 {
private:
	using MyTriangle3 = MyTriangle3<T>;
	using MyVec3 = MyVec3<T>;
public:
	using ElementType = T;

	constexpr MyPlane3() = default;
	constexpr MyPlane3(const MyVec3& normal_, const T& distance_) noexcept
		: normal(normal_)
		, distance(distance_)
	{}

	constexpr MyPlane3(const MyVec3& normal_, const MyVec3& pos) noexcept
		: normal(normal_)
		, distance(normal_.dot(pos))
	{}

	constexpr MyPlane3(const MyTriangle3& tri) noexcept {
		normal = tri.calcNormal();
		distance = normal.dot(tri.v0);
	}

	void setByTriangle(const MyVec3& v0, const MyVec3& v1, const MyVec3& v2) {
		normal	 = (v1-v0).cross(v2-v0).normalize();
		distance = normal.dot(v0);
	}

	T dot(const MyVec3& pt) const {
		return normal.dot(pt) - distance;
	}

	void draw(T size = 5);

	T		distance = 0;
	MyVec3	normal;
};

template<typename T> inline
void MyPlane3<T>::draw(T size /*= 5*/) {
	using MyVec3 = decltype(normal);

	MyVec3 pt = normal * distance;

	MyVec3 a, b, c, d;
	MyVec3 tangent, binormal;

	auto s = normal.dot(MyVec3::s_right());
	if (Math::equals(Math::abs(s), T(1))) {
		tangent = normal.cross(MyVec3::s_up());
	} else {
		tangent = normal.cross(MyVec3::s_right());
	}
	tangent = tangent.normalize();
	binormal = normal.cross(tangent);

	a = pt + tangent  * size;
	b = pt + binormal * size;
	c = pt - tangent  * size;
	d = pt - binormal * size;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	my_glColor4(T(1), T(1), T(1), T(1));
	{
		Scoped_glBegin begin(GL_LINES);
		my_glVertex3v(pt.data);
		my_glVertex3v((pt + normal).data);
	}

	{
		Scoped_glBegin begin(GL_QUADS);
		my_glVertex3v(a.data);
		my_glVertex3v(b.data);
		my_glVertex3v(c.data);
		my_glVertex3v(d.data);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

template<typename T>
struct MySphere3 {
	using ElementType = T;
	using MyVec3 = MyVec3<T>;

	constexpr MySphere3() = default;
	constexpr MySphere3(const MyVec3& center_, const T& radius_) noexcept
		: center(center_)
		, radius(radius_)
	{}

	void draw(int subAxis = 16, int subHeight = 16);

	T		radius;
	MyVec3	center;
};

template<typename T>
void MySphere3<T>::draw(int subAxis /*= 16*/, int subHeight /*= 16*/) {
	using MyVec3 = decltype(center);

	if (subAxis < 2 || subHeight < 2)
		return;

	static T PI  = Math::PI<T>();
	static T PI2 = Math::PI<T>() * 2.f;

	const auto& c = center;

	if (radius <= 0)
		return;

	Scoped_glPushMatrix m;
	my_glTranslate(c.x, c.y, c.z);

	MyVec3 top		{ T(0), T( radius), T(0) };
	MyVec3 bottom	{ T(0), T(-radius), T(0) };

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glColor4f(1,1,1,1);

	{ // top
		Scoped_glBegin begin(GL_TRIANGLES);
		int y = 1;
		T ty0 = PI * y / subHeight;
		T r0, h0;
		Math::sincos(ty0, r0, h0);

		for (int a = 0; a < subAxis; ++a) {
			T th0 = PI2 * a / subAxis;
			T th1 = PI2 * (a + 1) / subAxis;

			MyVec3 p0, p1;
			p0.y = p1.y = 0;
			Math::sincos(th0, p0.x, p0.z);
			Math::sincos(th1, p1.x, p1.z);

			MyVec3 v0 = p0 * radius * r0;
			MyVec3 v1 = p1 * radius * r0;
			v0.y = v1.y = h0;

			my_glVertex3v(top.data);
			my_glVertex3v(v0.data);
			my_glVertex3v(v1.data);
		}
	}

	{ // middle
		Scoped_glBegin begin(GL_TRIANGLES);
		for (int y = 1; y < subHeight - 1; y++) {

			T ty0 = PI * y / subHeight;
			T ty1 = PI * (y + 1) / subHeight;

			T r0 = Math::sin(ty0);
			T r1 = Math::sin(ty1);

			for (int a = 0; a < subAxis; ++a) {
				T th0 = PI2 * a / subAxis;
				T th1 = PI2 * (a + 1) / subAxis;

				MyVec3 p0, p1;
				p0.y = p1.y = 0;
				Math::sincos(th0, p0.x, p0.z);
				Math::sincos(th1, p1.x, p1.z);

				MyVec3 v0 = p0 * radius * r0;
				MyVec3 v1 = p1 * radius * r0;

				MyVec3 v2 = p0 * radius * r1;
				MyVec3 v3 = p1 * radius * r1;

				v0.y = v1.y = Math::cos(ty0);
				v2.y = v3.y = Math::cos(ty1);

				my_glVertex3v(v0.data);
				my_glVertex3v(v3.data);
				my_glVertex3v(v1.data);

				my_glVertex3v(v0.data);
				my_glVertex3v(v2.data);
				my_glVertex3v(v3.data);
			}
		}
	}

	{ // bottom
		Scoped_glBegin begin(GL_TRIANGLES);

		int y = subHeight - 2;
		T ty1 = PI * (y + 1) / subHeight;
		T r1, h1;
		Math::sincos(ty1, r1, h1);

		for (int a = 0; a < subAxis; ++a) {
			T th0 = PI2 * a / subAxis;
			T th1 = PI2 * (a + 1) / subAxis;

			MyVec3 p0, p1;
			p0.y = p1.y = 0;
			Math::sincos(th0, p0.x, p0.z);
			Math::sincos(th1, p1.x, p1.z);

			MyVec3 v0 = p0 * radius * r1;
			MyVec3 v1 = p1 * radius * r1;
			v0.y = v1.y = h1;

			my_glVertex3v(bottom.data);
			my_glVertex3v(v1.data);
			my_glVertex3v(v0.data);
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}

template<typename T>
struct MyAABB3 {
	using ElementType = T;
	using MyVec3 = MyVec3<T>;

	constexpr MyAABB3() noexcept {
		setEmpty();
	}

	constexpr MyAABB3(const MyVec3& minPt_, const MyVec3& maxPt_) noexcept
		: minPt(minPt_)
		, maxPt(maxPt_)
	{}

	template<typename DST, typename SRC>
	static MyAABB3<DST> s_cast(const MyAABB3<SRC>& v) {
		MyAABB3<DST> o;
		o.minPt = v.minPt;
		o.maxPt = v.maxPt;
		return o;
	}

	template<>
	static MyAABB3<T> s_cast(const MyAABB3<T>& v) {
		return v;
	}

	constexpr void setEmpty() {
		minPt.set( 1, 1, 1);
		maxPt.set(-1,-1,-1);
	}

	bool empty() const {
		return maxPt.x < minPt.x;
	}

	void unionPoint(MyVec3 pt) {
		if (empty()) {
			minPt = pt;
			maxPt = pt;
		} else {
			minPt = Math::min(minPt, pt);
			maxPt = Math::max(maxPt, pt);
		}
	}

	MyVec3 center() const {
		if (empty())
			return MyVec3::s_zero();

		return minPt + (maxPt - minPt) / T(2);
	}
	
	bool isInside(const MyVec3& pt) const {
		if (empty()) return false;
		if (pt.x < minPt.x || pt.x > maxPt.x) return false;
		if (pt.y < minPt.y || pt.y > maxPt.y) return false;
		if (pt.z < minPt.z || pt.z > maxPt.z) return false;
		return true;
	}

	void draw() {
		if (empty())
			return;

		static MyVec3 kVertexs[] = {
			MyVec3(minPt.x, minPt.y, minPt.z),
			MyVec3(maxPt.x, minPt.y, minPt.z),
			MyVec3(maxPt.x, maxPt.y, minPt.z),
			MyVec3(minPt.x, maxPt.y, minPt.z),

			MyVec3(minPt.x, minPt.y, maxPt.z),
			MyVec3(maxPt.x, minPt.y, maxPt.z),
			MyVec3(maxPt.x, maxPt.y, maxPt.z),
			MyVec3(minPt.x, maxPt.y, maxPt.z),
		};

		{
			glPointSize(2);
			Scoped_glBegin begin(GL_POINTS);
			for (const MyVec3& v : kVertexs) {
				my_glVertex3(v.x, v.y, v.z);
			}
			glPointSize(1);
		}

		{
			glColor4f(1,1,0,1);
			//		  0--------1
			//		 /|       /|
			//		3--------2 |
			//      | |      | |
			//		| 4------|-5
			//      |/       |/
			//      7--------6

			static u16 kIndices[] = {
				0,1,
				1,2,
				2,3,
				3,0,
				
				4,5,
				5,6,
				6,7,
				7,4,

				1,5,
				2,6,
				3,7,
				0,4,
			};

			glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, kVertexs);
				glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, kIndices);
			glDisableClientState(GL_VERTEX_ARRAY);
			glColor4f(1,1,1,1);
		}
	}

	MyVec3 minPt;
	MyVec3 maxPt;
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

using MyAABB3f = MyAABB3<float>;
using MyAABB3d = MyAABB3<double>;

} // namespace Math
} // namespace sge