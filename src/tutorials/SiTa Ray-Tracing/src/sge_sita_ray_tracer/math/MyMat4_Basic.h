#pragma once

#include "MyQuat4.h"

namespace sge {

template<typename T>
struct MyMat4_Basic_Data { // colomn-major
/*
* data[0 1 2 3 4 ... 14 15 16] flatten in MyMat4_Basic_Data

Figure 1-1  	  c0 c1 c2 c3
			----|--------------
			 r0	| 0  4  8  12
			 r1	| 1  5  9  13
			 r2	| 2  6  10 14
			 r3	| 3  7  11 15

				  r0 r1 r2 r3
Figure 1-2	----|-------------- // cuz computer language is read from left-top to right-bottom
			 c0	|  0  1  2  3
			 c1	|  4  5  6  7
			 c2	|  8  9 10 11
			 c3	| 12 13 14 15

				  c0 c1 c2 c3
Figure 2-1	----|--------------
			 r0	| xx yx zx wx
			 r1	| xy yy zy wy
			 r2	| xz yz zz wz
			 r3	| xw yw zw ww

				  r0 r1 r2 r3
Figure 2-2	----|--------------
			 c0 | xx xy xz xw
			 c1 | yx yy yz yw
			 c2 | zx zy zz zw
			 c3 | wx wy wz ww

				  c0   c1   c2   c3
Figure 3-1	----|---------------------
			 r1 | cx.x cy.x cz.x cw.x
			 r2 | cx.y cy.y cz.y cw.y
			 r3 | cx.z cy.z cz.z cw.z
			 r4 | cx.w cy.w cz.w cw.w

				  r0   r1   r2   r3
Figure 3-2	----|---------------------
			 c0 | cx.x cx.y cx.z cx.w
			 c1 | cy.x cy.y cy.z cy.w
			 c2 | cz.x cz.y cz.z cz.w
			 c3 | cw.x cw.y cw.z cw.w

Figure 4-1	we always like 'row-col' numbers to describe matrix, so just see Figure 4-2 is fine.

				   r0  r1  r2  r3
Figure 4-2	----|---------------------
			 c0	| _00 _01 _02 _03
			 c1	| _10 _11 _12 _13
			 c2	| _20 _21 _22 _23
			 c3	| _30 _31 _32 _33
*/

	static constexpr size_t kElementCount = 16;

	using ElementType	= T;
	using MyVec4		= MyVec4<T>;

	union {
		T		data[kElementCount];
		MyVec4 _columns[4];

		struct { MyVec4 cx, cy, cz, cw; };

		struct { // Figure 2-2	// Figure 3-2
			T xx, xy, xz, xw;	// same as: T cx.x, cx.y, cx.z, cx.w;
			T yx, yy, yz, yw;	// same as: T cy.x, cy.y, cy.z, cy.w;
			T zx, zy, zz, zw;	// same as: T cz.x, cz.y, cz.z, cz.w;
			T wx, wy, wz, ww;	// same as: T cw.x, cw.y, cw.z, cw.w;
		};
		struct { // Figure 4-2
			T _00, _01, _02, _03; // same as: _columns[0 ] _columns[1 ] _columns[2 ] _columns[3	]
			T _10, _11, _12, _13; // same as: _columns[4 ] _columns[5 ] _columns[6 ] _columns[7	]
			T _20, _21, _22, _23; // same as: _columns[8 ] _columns[9 ] _columns[10] _columns[11]
			T _30, _31, _32, _33; // same as: _columns[12] _columns[13] _columns[14] _columns[15]
		};
		struct {
			T c0r0, c0r1, c0r2, c0r3;
			T c1r0, c1r1, c1r2, c1r3;
			T c2r0, c2r1, c2r2, c2r3;
			T c3r0, c3r1, c3r2, c3r3;
		};
		struct {
			T r0c0, r1c0, r2c0, r3c0;
			T r0c1, r1c1, r2c1, r3c1;
			T r0c2, r1c2, r2c2, r3c2;
			T r0c3, r1c3, r2c3, r3c3;
		};
	};

	constexpr MyMat4_Basic_Data() = default;
	constexpr MyMat4_Basic_Data(const MyVec4& cx_, const MyVec4& cy_, const MyVec4& cz_, const MyVec4& cw_)
		: cx(cx_), cy(cy_), cz(cz_), cw(cw_) {}

	// Figure 2-2
	constexpr MyMat4_Basic_Data(const T* v)
		: xx(v[0]),  xy(v[1]),  xz(v[2]),  xw(v[3])
		, yx(v[4]),  yy(v[5]),  yz(v[6]),  yw(v[7])
		, zx(v[8]),  zy(v[9]),  zz(v[10]), zw(v[11])
		, wx(v[12]), wy(v[13]), wz(v[14]), ww(v[15]) {}

	// Figure 4-2
	constexpr MyMat4_Basic_Data(T __00, T __01, T __02, T __03,
								T __10, T __11, T __12, T __13,
								T __20, T __21, T __22, T __23,
								T __30, T __31, T __32, T __33)
		: _00(__00), _01(__01), _02(__02), _03(__03)
		, _10(__10), _11(__11), _12(__12), _13(__13)
		, _20(__20), _21(__21), _22(__22), _23(__23)
		, _30(__30), _31(__31), _32(__32), _33(__33) {}


	void set(T __00, T __01, T __02, T __03,
			 T __10, T __11, T __12, T __13,
			 T __20, T __21, T __22, T __23,
			 T __30, T __31, T __32, T __33)
	{
		_00 = __00; _01 = __01; _02 = __02; _03 = __03;
		_10 = __10; _11 = __11; _12 = __12; _13 = __13;
		_20 = __20; _21 = __21; _22 = __22; _23 = __23;
		_30 = __30; _31 = __31; _32 = __32; _33 = __33;
	}

	void set(const MyVec4& cx_, const MyVec4& cy_, const MyVec4& cz_, const MyVec4& cw_) {
		cx.set(cx_);
		cy.set(cy_);
		cw.set(cw_);
		cz.set(cz_);
	}

	void set(const MyMat4_Basic_Data& v) {
		cx.set(v.cx);
		cy.set(v.cy);
		cz.set(v.cz);
		cw.set(v.cw);
	}

	void setAll(const T& v) {
		cx.setAll(v);
		cy.setAll(v);
		cz.setAll(v);
		cw.setAll(v);
	}
};

#define SGE_M4_MINOR3X3(c0,c1,c2, r0,r1,r2) \
	(data[c0*4+r0] * (data[c1*4+r1]*data[c2*4+r2] - data[c1*4+r2]*data[c2*4+r1]) - \
	 data[c1*4+r0] * (data[c0*4+r1]*data[c2*4+r2] - data[c0*4+r2]*data[c2*4+r1]) + \
	 data[c2*4+r0] * (data[c0*4+r1]*data[c1*4+r2] - data[c0*4+r2]*data[c1*4+r1]) ) \
//---- c*4+r --> MyMat4 array flatten index, but why c*4? because MyMat4 is column-major

template<typename T, class DATA = MyMat4_Basic_Data<T> >
struct MyMat4_Basic : public DATA {
	using MyMat4		= MyMat4_Basic;
	using MyVec3		= MyVec3<T>;
	using MyQuat4		= MyQuat4<T>;

	using typename DATA::ElementType;
	using typename DATA::MyVec4;

	using DATA::cx;
	using DATA::cy;
	using DATA::cz;
	using DATA::cw;
	using DATA::data;
	using DATA::kElementCount;

	using DATA::xx; using DATA::xy; using DATA::xz; using DATA::xw;
	using DATA::yx; using DATA::yy; using DATA::yz; using DATA::yw;
	using DATA::zx; using DATA::zy; using DATA::zz; using DATA::zw;
	using DATA::wx; using DATA::wy; using DATA::wz; using DATA::ww;

	using DATA::_00; using DATA::_01; using DATA::_02; using DATA::_03;
	using DATA::_10; using DATA::_11; using DATA::_12; using DATA::_13;
	using DATA::_20; using DATA::_21; using DATA::_22; using DATA::_23;
	using DATA::_30; using DATA::_31; using DATA::_32; using DATA::_33;

	using DATA::c0r0; using DATA::c0r1; using DATA::c0r2; using DATA::c0r3;
	using DATA::c1r0; using DATA::c1r1; using DATA::c1r2; using DATA::c1r3;
	using DATA::c2r0; using DATA::c2r1; using DATA::c2r2; using DATA::c2r3;
	using DATA::c3r0; using DATA::c3r1; using DATA::c3r2; using DATA::c3r3;

	using DATA::r0c0; using DATA::r1c0; using DATA::r2c0; using DATA::r3c0;
	using DATA::r0c1; using DATA::r1c1; using DATA::r2c1; using DATA::r3c1;
	using DATA::r0c2; using DATA::r1c2; using DATA::r2c2; using DATA::r3c2;
	using DATA::r0c3; using DATA::r1c3; using DATA::r2c3; using DATA::r3c3;

	constexpr MyMat4() = default;
	constexpr MyMat4(const T* v) : DATA(v) {}
	constexpr MyMat4(const MyVec4& cx_, const MyVec4& cy_, const MyVec4& cz_, const MyVec4& cw_) : DATA(cx_, cy_, cz_, cw_) {}
	constexpr MyMat4(T __00, T __01, T __02, T __03,
				     T __10, T __11, T __12, T __13,
				     T __20, T __21, T __22, T __23,
				     T __30, T __31, T __32, T __33)
		: DATA(__00, __01, __02, __03,
			   __10, __11, __12, __13,
			   __20, __21, __22, __23,
			   __30, __31, __32, __33) {}

	SGE_INLINE static MyMat4 s_identity() {
		return MyMat4(1,0,0,0,
			          0,1,0,0,
			          0,0,1,0,
			          0,0,0,1);
	}

	SGE_INLINE static MyMat4 s_frustum(T l, T r, T b, T t, T n, T f);
	SGE_INLINE static MyMat4 s_perspective(T fovy_rad, T aspect, T zNear, T zFar);
	SGE_INLINE static MyMat4 s_ortho(T l, T r, T b, T t, T n, T f);
	SGE_INLINE static MyMat4 s_lookAt(const MyVec3& eye, const MyVec3& aim, const MyVec3& up);
	SGE_INLINE static MyMat4 s_translate(const MyVec3& t);
	SGE_INLINE static MyMat4 s_quat(const MyQuat4& q);
	SGE_INLINE static MyMat4 s_scale(const MyVec3& s);
	SGE_INLINE static MyMat4 s_TRS(const MyVec3& t, const MyQuat4& r, const MyVec3& s);
	SGE_INLINE static MyMat4 s_TS(const MyVec3& t, const MyVec3& s);

	SGE_INLINE MyVec4 col(int i) const { SGE_ASSERT(i >= 0 && i < 4); return _columns[i]; }
	SGE_INLINE MyVec4 row(int i) const {
		SGE_ASSERT(i >= 0 && i < 4);
		return MyVec4(data[    i],
					  data[4  +i],
					  data[4*2+i],
					  data[4*3+i]);
	}

	SGE_INLINE void setCol(int i, const MyVec4& v) { SGE_ASSERT(i >= 0 && i < 4); _columns[i] = v; }
	SGE_INLINE void setRow(int i, const MyVec4& v) {
		SGE_ASSERT(i >= 0 && i < 4);
		data[    i] = v.x;
		data[4  +i] = v.y;
		data[4*2+i] = v.z;
		data[4*3+i] = v.w;
	}

	SGE_INLINE MyVec4 mulPoint(const MyVec4& v) const {
		// mul 4x4
		return MyVec4(v.x*cx.x + v.y*cy.x + v.z*cz.x + v.w*cw.x,
					  v.x*cx.y + v.y*cy.y + v.z*cz.y + v.w*cw.y,
					  v.x*cx.z + v.y*cy.z + v.z*cz.z + v.w*cw.z,
					  v.x*cx.w + v.y*cy.w + v.z*cz.w + v.w*cw.w);
	}

	SGE_INLINE MyVec3 mulPoint4x3(const MyVec3& v) const {
		// mul 4x3 (no projection)
		return MyVec3(v.x*cx.x + v.y*cy.x + v.z*cz.x + cw.x,
					  v.x*cx.y + v.y*cy.y + v.z*cz.y + cw.y,
					  v.x*cx.z + v.y*cy.z + v.z*cz.z + cw.z);
	}

	SGE_INLINE MyVec3 mulVector(const MyVec3& v) const {
		// mul 3x3
		return MyVec3(v.x*cx.x + v.y*cy.x + v.z*cz.x,
					  v.x*cx.y + v.y*cy.y + v.z*cz.y,
					  v.x*cx.z + v.y*cy.z + v.z*cz.z);
	}

	SGE_INLINE MyVec3 mulNormal(const MyVec3& v) const {
		// for normal non-uniform scale
		return inverse3x3Transpose().mulVector(v);
	}

	SGE_INLINE MyMat4 transpose() const {
		return MyMat4(xx, yx, zx, wx,
			          xy, yy, zy, wy,
			          xz, yz, zz, wz,
			          xw, yw, zw, ww);
	}

	SGE_INLINE void transposed() {
		swap(yx, xy);
		swap(zx, xz);
		swap(wx, xw);
		swap(zy, yz);
		swap(wy, yw);
		swap(wz, zw);
	}

	SGE_INLINE void getPosition(MyVec3& o) const {
		o.set(cw.x, cw.y, cw.z);
	}

	SGE_INLINE void getRotation(MyQuat4& o) const {
		const auto& u = _columns[1];
		const auto& f = _columns[2];

		MyVec3 up		= u.xyz().normalize();
		MyVec3 forward	= f.xyz().normalize();
		MyVec3 right	= up.cross(forward);
		MyVec3 orthoUp	= forward.cross(right);
		o.set(MyQuat4::s_lookRotation(forward, orthoUp));
	}

	SGE_INLINE void getLossyScale(MyVec3& o) const {
		MyMat4 rsMat(cx.x, cx.y, cx.z, T(0),
					 cy.x, cy.y, cy.z, T(0),
					 cz.x, cz.y, cz.z, T(0),
					 T(0), T(0), T(0), T(1));

		MyQuat4 q;
		getRotation(q);

		auto invRotMat4 = MyMat4::s_quat(q.inverse());
		auto scaleSkewMat4 = rsMat * invRotMat4;
		o.set(scaleSkewMat4.cx.x, scaleSkewMat4.cy.y, scaleSkewMat4.cz.z);
	}

	SGE_INLINE T determinant() const {
		return data[0]  * SGE_M4_MINOR3X3(1,2,3, 1,2,3)
			 - data[4]	* SGE_M4_MINOR3X3(0,2,3, 1,2,3)
			 + data[8]	* SGE_M4_MINOR3X3(0,1,3, 1,2,3)
			 - data[12] * SGE_M4_MINOR3X3(0,1,2, 1,2,3);
	}

	SGE_INLINE T determinant3x3() const {
		return xx*(yy*zz - zy*yz)
			 - yx*(xy*zz - zy*xz)
			 + zx*(xy*yz - yy*xz);
	}

	SGE_INLINE MyMat4 adjugate() const;
	SGE_INLINE MyMat4 inverse() const;
	SGE_INLINE MyMat4 inverse3x3() const;
	SGE_INLINE MyMat4 inverse3x3Transpose() const;

	SGE_INLINE bool equals (const MyMat4& r,	T epsilon = Math::epsilon<T>()) const;
	SGE_INLINE bool equals0(					T epsilon = Math::epsilon<T>()) const;

	SGE_INLINE			MyVec4& operator[](int i)		{ SGE_ASSERT(i >= 0 && i < 4); return _columns[i]; }
	SGE_INLINE const	MyVec4& operator[](int i) const	{ SGE_ASSERT(i >= 0 && i < 4); return _columns[i]; }

	SGE_INLINE MyMat4 operator+(const T& s) const		{ return MyMat4(cx+s, cy+s, cz+s, cw+s); }
	SGE_INLINE MyMat4 operator-(const T& s) const		{ return MyMat4(cx-s, cy-s, cz-s, cw-s); }
	SGE_INLINE MyMat4 operator*(const T& s) const		{ return MyMat4(cx*s, cy*s, cz*s, cw*s); }
	SGE_INLINE MyMat4 operator/(const T& s) const		{ return MyMat4(cx/s, cy/s, cz/s, cw/s); }

	SGE_INLINE void operator+=(const T& s)				{ _columns[0]+=s, _columns[1]+=s, _columns[2]+=s, _columns[3]+=s; }
	SGE_INLINE void operator-=(const T& s)				{ _columns[0]-=s, _columns[1]-=s, _columns[2]-=s, _columns[3]-=s; }
	SGE_INLINE void operator*=(const T& s)				{ _columns[0]*=s, _columns[1]*=s, _columns[2]*=s, _columns[3]*=s; }
	SGE_INLINE void operator/=(const T& s)				{ _columns[0]/=s, _columns[1]/=s, _columns[2]/=s, _columns[3]/=s; }

	SGE_INLINE MyMat4 operator+(const MyMat4& r) const	{ return MyMat4(cx+r.cx, cy+r.cy, cz+r.cz, cw+r.cw); }
	SGE_INLINE MyMat4 operator-(const MyMat4& r) const	{ return MyMat4(cx-r.cx, cy-r.cy, cz-r.cz, cw-r.cw); }

	SGE_INLINE void operator+=(const MyMat4& r)	{ _columns[0]+=r._columns[0], _columns[1]+=r._columns[1], _columns[2]+=r._columns[2], _columns[3]+=r._columns[3]; }
	SGE_INLINE void operator-=(const MyMat4& r)	{ _columns[0]-=r._columns[0], _columns[1]-=r._columns[1], _columns[2]-=r._columns[2], _columns[3]-=r._columns[3]; }

	SGE_INLINE MyMat4 operator*(const MyMat4& r) const;

	SGE_INLINE bool operator==(const MyMat4& r) const { return cx==r.cx && cy==r.cy && cz==r.cz && cw==r.cw; }
	SGE_INLINE bool operator!=(const MyMat4& r) const { return !(this->operator==(r)); }

	SGE_INLINE void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "(\n\t{},\n\t{},\n\t{},\n\t{})", cx, cy, cz, cw);
	}

	SGE_INLINE friend std::ostream& operator<< (std::ostream& o, const MyMat4& m) {
		o << "(" \
		<< "\n\t(" << m.cx.x << "," << m.cx.y << "," << m.cx.z << "," << m.cx.w << ")" \
		<< "\n\t(" << m.cy.x << "," << m.cy.y << "," << m.cy.z << "," << m.cy.w << ")" \
		<< "\n\t(" << m.cz.x << "," << m.cz.y << "," << m.cz.z << "," << m.cz.w << ")" \
		<< "\n\t(" << m.cw.x << "," << m.cw.y << "," << m.cw.z << "," << m.cw.w << ")" \
		<< "\n)";
		return o;
	}
};

using MyMat4_Basicf = MyMat4_Basic<float>;
using MyMat4_Basicd = MyMat4_Basic<double>;

SGE_FORMATTER_T( SGE_ARGS(typename T, class DATA), MyMat4_Basic< SGE_ARGS(T, DATA) >)

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_frustum(T l, T r, T b, T t, T n, T f) {
	if (Math::equals(l, r) || Math::equals(b, t) || Math::equals(n, f)) {
		return MyMat4::s_identity();
	}
	return MyMat4(T(2)*n/(r-l),		T(0),           T(0),            T(0),
				  T(0),				T(2)*n/(t-b),   T(0),            T(0),
				  (r+l)/(r-l),		(t+b)/(t-b),	-(f+n)/(f-n),	 T(-1),
				  T(0),				T(0),           -T(2)*f*n/(f-n), T(0));
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_perspective(T fovy_rad, T aspect, T zNear, T zFar) {
	if (Math::equals0(aspect) || Math::equals(zNear, zFar)) {
		return MyMat4::s_identity();
	}
#if 0
	T ymax = zNear * Math::tan(fovy_rad * T(0.5));
	T xmax = ymax  * aspect;
	return s_frustum(-xmax, xmax, -ymax, ymax, zNear, zFar);
#else
	T deltaZ = zFar - zNear;
	T tf = Math::tan(fovy_rad * T(0.5));

	return MyMat4(T(1)/(aspect*tf),	T(0),		T(0),						T(0),
				  T(0),				T(1)/tf,	T(0),						T(0),
				  T(0),				T(0),		-(zFar + zNear)/deltaZ,		T(-1),
				  T(0),				T(0),		T(-2)*zNear*zFar/deltaZ,	T(0));
#endif
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_ortho(T l, T r, T b, T t, T n, T f) {
	if (Math::equals(l, r) || Math::equals(b, t) || Math::equals(n, f)) {
		return MyMat4::s_identity();
	}

	T w = r-l;
	T h = t-b;
	T d = f-n;
	return MyMat4(T(2)/w,    T(0),        T(0),     T(0),
			      T(0),      T(2)/h,      T(0),     T(0),
			      T(0),      T(0),        -T(2)/d,  T(0),
			      -(r+l)/w,	 -(t+b)/h,	  -(f+n)/d,	T(1));
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_lookAt(const MyVec3& eye, const MyVec3& aim, const MyVec3& up) {
	MyVec3 f = (aim - eye).normalize();
	MyVec3 r = f.cross(up).normalize();
	if (r.equals0()) {
		return MyMat4::s_identity();
	}

	MyVec3 u = r.cross(f);
	return MyMat4(r.x,         u.x,         -f.x,       T(0),
				  r.y,         u.y,         -f.y,       T(0),
				  r.z,         u.z,         -f.z,       T(0),
				  -r.dot(eye), -u.dot(eye), f.dot(eye), T(1));
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_translate(const MyVec3& t) {
	return MyMat4(T(1), T(0), T(0), T(0),
				  T(0), T(1), T(0), T(0),
				  T(0), T(0), T(1), T(0),
				  t.x,  t.y,  t.z,  T(1));
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_quat(const MyQuat4& q) {
	T xx = q.x*q.x;
	T yy = q.y*q.y;
	T zz = q.z*q.z;
	T wx = q.w*q.x;
	T wy = q.w*q.y;
	T wz = q.w*q.z;
	T xy = q.x*q.y;
	T xz = q.x*q.z;
	T yz = q.y*q.z;

	return MyMat4(T(1)-T(2)*(yy+zz),	T(2)*(xy+wz),		T(2)*(xz-wy),		T(0),
				  T(2)*(xy-wz),			T(1)-T(2)*(xx+zz),	T(2)*(yz+wx),		T(0),
				  T(2)*(xz+wy),			T(2)*(yz-wx),		T(1)-T(2)*(xx+yy),  T(0),
				  T(0),					T(0),				T(0),				T(1));
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_scale(const MyVec3& s) {
	return MyMat4(s.x,	T(0), T(0), T(0),
				  T(0), s.y,  T(0),	T(0),
				  T(0), T(0), s.z,	T(0),
				  T(0), T(0), T(0), T(1));
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_TRS(const MyVec3& t, const MyQuat4& r, const MyVec3& s) {
	return s_translate(t) * s_quat(r) * s_scale(s);
}

template<class T, class DATA> SGE_INLINE 
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::s_TS(const MyVec3& t, const MyVec3& s) {
	return MyMat4(s.x,  T(0), T(0), T(0),
				  T(0), s.y,  T(0), T(0),
				  T(0), T(0), s.z,  T(0),
				  t.x,  t.y,  t.z,  T(1));
}

template<typename T, class DATA> SGE_INLINE
bool MyMat4_Basic<T, DATA>::equals(const MyMat4_Basic<T, DATA>& r, T epsilon) const {
	return cx.equals(r.cx, epsilon)
		&& cy.equals(r.cy, epsilon)
		&& cz.equals(r.cz, epsilon)
		&& cw.equals(r.cw, epsilon);
}

template<typename T, class DATA> SGE_INLINE
bool MyMat4_Basic<T, DATA>::equals0(T epsilon) const {
	return cx.equals0(epsilon)
		&& cy.equals0(epsilon)
		&& cz.equals0(epsilon)
		&& cw.equals0(epsilon);
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::operator*(const MyMat4& r) const {
#define E(ROW, COL) \
	data[    ROW] * r.data[4*COL  ] + \
	data[  4+ROW] * r.data[4*COL+1] + \
	data[2*4+ROW] * r.data[4*COL+2] + \
	data[3*4+ROW] * r.data[4*COL+3]   \
//----
	return MyMat4(E(0,0), E(1,0), E(2,0), E(3,0),
				  E(0,1), E(1,1), E(2,1), E(3,1),
				  E(0,2), E(1,2), E(2,2), E(3,2),
				  E(0,3), E(1,3), E(2,3), E(3,3));
#undef E
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::adjugate() const {
	MyMat4 m;

	m.data[0]  =  SGE_M4_MINOR3X3(1,2,3, 1,2,3);
	m.data[1]  = -SGE_M4_MINOR3X3(1,2,3, 0,2,3);
	m.data[2]  =  SGE_M4_MINOR3X3(1,2,3, 0,1,3);
	m.data[3]  = -SGE_M4_MINOR3X3(1,2,3, 0,1,2);
	m.data[4]  = -SGE_M4_MINOR3X3(0,2,3, 1,2,3);
	m.data[5]  =  SGE_M4_MINOR3X3(0,2,3, 0,2,3);
	m.data[6]  = -SGE_M4_MINOR3X3(0,2,3, 0,1,3);
	m.data[7]  =  SGE_M4_MINOR3X3(0,2,3, 0,1,2);
	m.data[8]  =  SGE_M4_MINOR3X3(0,1,3, 1,2,3);
	m.data[9]  = -SGE_M4_MINOR3X3(0,1,3, 0,2,3);
	m.data[10] =  SGE_M4_MINOR3X3(0,1,3, 0,1,3);
	m.data[11] = -SGE_M4_MINOR3X3(0,1,3, 0,1,2);
	m.data[12] = -SGE_M4_MINOR3X3(0,1,2, 1,2,3);
	m.data[13] =  SGE_M4_MINOR3X3(0,1,2, 0,2,3);
	m.data[14] = -SGE_M4_MINOR3X3(0,1,2, 0,1,3);
	m.data[15] =  SGE_M4_MINOR3X3(0,1,2, 0,1,2);

	m.transposed();
	return m;
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::inverse() const {
#if 0
	auto det = determinant();

	if (Math::equals0(det)) {
		return MyMat4::s_identity();
	}

	auto inv = T(1) / det;
	return adjugate() * inv;
#else

	T wtmp[4][8];
	T m0, m1, m2, m3, s;
	T *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = cx.x;
	r0[1] = cx.y;
	r0[2] = cx.z;
	r0[3] = cx.w;
	r0[4] = T(1);
	r0[5] = r0[6] = r0[7] = T(0);

	r1[0] = cy.x;
	r1[1] = cy.y;
	r1[2] = cy.z;
	r1[3] = cy.w;
	r1[5] = T(1);
	r1[4] = r1[6] = r1[7] = T(0);

	r2[0] = cz.x;
	r2[1] = cz.y;
	r2[2] = cz.z;
	r2[3] = cz.w;
	r2[6] = T(1);
	r2[4] = r2[5] = r2[7] = T(0);

	r3[0] = cw.x;
	r3[1] = cw.y;
	r3[2] = cw.z;
	r3[3] = cw.w;
	r3[7] = T(1);
	r3[4] = r3[5] = r3[6] = T(0);

	// choose pivot - or die
	if (Math::abs(r3[0]) > Math::abs(r2[0])) swap(r3, r2);
	if (Math::abs(r2[0]) > Math::abs(r1[0])) swap(r2, r1);
	if (Math::abs(r1[0]) > Math::abs(r0[0])) swap(r1, r0);
	if (Math::equals0(r0[0])) {
		SGE_ASSERT(false);
		return MyMat4::s_identity();
	}

	// eliminate first variable
	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];

	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4]; if (!Math::equals0(s)) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5]; if (!Math::equals0(s)) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6]; if (!Math::equals0(s)) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7]; if (!Math::equals0(s)) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	// choose pivot - or die
	if (Math::abs(r3[1]) > Math::abs(r2[1])) swap(r3, r2);
	if (Math::abs(r2[1]) > Math::abs(r1[1])) swap(r2, r1);
	if (Math::equals0(r1[1])) {
		SGE_ASSERT(false);
		return MyMat4::s_identity();
	}

	// eliminate second variable
	m2 = r2[1] / r1[1];
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];
	r3[3] -= m3 * r1[3];

	s = r1[4]; if (!Math::equals0(s)) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (!Math::equals0(s)) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (!Math::equals0(s)) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (!Math::equals0(s)) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	// choose pivot - or die
	if (Math::abs(r3[2]) > Math::abs(r2[2])) swap(r3, r2);
	if (Math::equals0(r2[2])) {
		SGE_ASSERT(false);
		return MyMat4::s_identity();
	}

	// eliminate third variable
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3];
	r3[4] -= m3 * r2[4],
		r3[5] -= m3 * r2[5];
	r3[6] -= m3 * r2[6],
		r3[7] -= m3 * r2[7];

	// last check
	if (Math::equals0(r3[3])) {
		SGE_ASSERT(false);
		return MyMat4::s_identity();
	}

	s = T(1) / r3[3];    // now back substitute row 3
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

	m2 = r2[3];          // now back substitute row 2
	s = T(1) / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1;
	r1[5] -= r3[5] * m1;
	r1[6] -= r3[6] * m1;
	r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0;
	r0[5] -= r3[5] * m0;
	r0[6] -= r3[6] * m0;
	r0[7] -= r3[7] * m0;

	m1 = r1[2];                 // now back substitute row 1
	s = T(1) / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1);
	r1[5] = s * (r1[5] - r2[5] * m1);
	r1[6] = s * (r1[6] - r2[6] * m1);
	r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0;
	r0[5] -= r2[5] * m0;
	r0[6] -= r2[6] * m0;
	r0[7] -= r2[7] * m0;

	m0 = r0[1];                 // now back substitute row 0
	s = T(1) / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0);
	r0[5] = s * (r0[5] - r1[5] * m0);
	r0[6] = s * (r0[6] - r1[6] * m0);
	r0[7] = s * (r0[7] - r1[7] * m0);

	return MyMat4(r0[4], r0[5], r0[6], r0[7],
				  r1[4], r1[5], r1[6], r1[7],
				  r2[4], r2[5], r2[6], r2[7],
				  r3[4], r3[5], r3[6], r3[7]);
#endif
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::inverse3x3() const {
	auto det = determinant3x3();
	auto inv = T(1) / det;

	return MyMat4((yy*zz-yz*zy) *  inv, (xy*zz-xz*zy) * -inv, (xy*yz-xz*yy) *  inv, T(0),
				  (yx*zz-zx*yz) * -inv, (xx*zz-xz*zx) *  inv, (xx*yz-xz*yx) * -inv, T(0),
				  (yx*zy-zx*yy) *  inv, (xx*zy-xy*zx) * -inv, (xx*yy-xy*yx) *  inv, T(0),
				  T(0),     			T(0),				  T(0),					T(1));
}

template<typename T, class DATA> SGE_INLINE
MyMat4_Basic<T, DATA> MyMat4_Basic<T, DATA>::inverse3x3Transpose() const {
	auto det = determinant3x3();
	auto inv = T(1) / det;

	return MyMat4((yy*zz-yz*zy) *  inv, (yx*zz-zx*yz) * -inv, (yx*zy-zx*yy) *  inv, T(0),
				  (xy*zz-xz*zy) * -inv, (xx*zz-xz*zx) *  inv, (xx*zy-xy*zx) * -inv, T(0),
				  (xy*yz-xz*yy) *  inv, (xx*yz-xz*yx) * -inv, (xx*yy-xy*yx) *  inv, T(0),
				  T(0),     			T(0),				  T(0),					T(1));
}

#undef SGE_M4_MINOR3X3
} // namespace sge