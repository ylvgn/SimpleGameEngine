#pragma once

#include "vec4.h"
#include "quat.h"

namespace sge {

struct Transform;
struct dual_quat;

struct mat4 {

	using vec3 = TVec3<float>;
	using vec4 = TVec4<float>;

	static const size_t kElementCount = 16;

	union {
		/* v[0 1 2 3 4 5 6 7 8 9 10 ...] in mat4
		    r0 r1 r2 r3
			0  1  2  3   c0
			4  5  6  7   c1
			8  9  10 11  c2
			12 13 14 15  c3
		*/

		/*
			r0 r1 r2 r3
			xx xy xz xw  c0
			yx yy yz yw  c1
			zx zy zz zw  c2
			wx wy wz ww  c3
		*/

		float v[kElementCount];
		vec4 _columns[4]; // colomn-major

		struct {
			float xx, xy, xz, xw; // x-basis vector aka cx
			float yx, yy, yz, yw; // y-basis vector aka cy
			float zx, zy, zz, zw; // z-basis vector aka cz
			float wx, wy, wz, ww; // w-basis vector aka cw
		};

		struct {
			//    r0    r1    r2    r3
			float c0r0, c0r1, c0r2, c0r3; // c0
			float c1r0, c1r1, c1r2, c1r3; // c1
			float c2r0, c2r1, c2r2, c2r3; // c2
			float c3r0, c3r1, c3r2, c3r3; // c3
		};

		struct {
			float r0c0, r1c0, r2c0, r3c0;
			float r0c1, r1c1, r2c1, r3c1;
			float r0c2, r1c2, r2c2, r3c2;
			float r0c3, r1c3, r2c3, r3c3;
		};
	};

	inline mat4() = default;

	inline mat4(const float* fv)
		: xx(fv[0]),  xy(fv[1]),  xz(fv[2]),  xw(fv[3])
		, yx(fv[4]),  yy(fv[5]),  yz(fv[6]),  yw(fv[7])
		, zx(fv[8]),  zy(fv[9]),  zz(fv[10]), zw(fv[11])
		, wx(fv[12]), wy(fv[13]), wz(fv[14]), ww(fv[15]) {}

	inline mat4(float _00, float _01, float _02, float _03,
				float _10, float _11, float _12, float _13,
				float _20, float _21, float _22, float _23,
				float _30, float _31, float _32, float _33)
		: xx(_00), xy(_01), xz(_02), xw(_03)
		, yx(_10), yy(_11), yz(_12), yw(_13)
		, zx(_20), zy(_21), zz(_22), zw(_23)
		, wx(_30), wy(_31), wz(_32), ww(_33) {}

	inline mat4(const vec4f& cx, const vec4f& cy, const vec4f& cz, const vec4f& cw) : _columns{ cx, cy, cz, cw } {}
	inline mat4(sge::Mat4f m) : mat4(m._elements) {}

	const vec4& right()			const { return _columns[0]; }
	const vec4& up()			const { return _columns[1]; }
	const vec4& forward()		const { return _columns[2]; }
	const vec4& projection()	const { return _columns[3]; }

	const vec4& cx()			const { return _columns[0]; }
	const vec4& cy()			const { return _columns[1]; }
	const vec4& cz()			const { return _columns[2]; }
	const vec4& cw()			const { return _columns[3]; }

	inline static mat4 s_identity() {
		return mat4(1,0,0,0,
			        0,1,0,0,
			        0,0,1,0,
			        0,0,0,1);
	}

	inline			vec4& operator[](int i)			{ SGE_ASSERT(i < kElementCount); return _columns[i]; }
	inline const	vec4& operator[](int i) const	{ SGE_ASSERT(i < kElementCount); return _columns[i]; }

	inline vec4 col(int i) const { SGE_ASSERT(i < 4); return _columns[i]; }
	inline vec4 row(int i) const {
		SGE_ASSERT(i < 4);
		return vec4(v[4*0+i],
					v[4*1+i],
					v[4*2+i],
					v[4*3+i]);
	}

	inline void setCol(int i, const vec4& col) { SGE_ASSERT(i < 4); _columns[i] = col; }
	inline void setRow(int i, const vec4& row) {
		SGE_ASSERT(i < 4);
		v[4*0+i] = row.x;
		v[4*1+i] = row.y;
		v[4*2+i] = row.z;
		v[4*3+i] = row.w;
	}

	inline bool equals (const mat4& r,	float epsilon = Math::epsilon<float>()) const;
	inline bool equals0(				float epsilon = Math::epsilon<float>()) const;

	// Scaling matrices and then adding them allows you to "lerp" or "mix" between two matrices,
	// so long as both matrices represent a linear transform.
	inline mat4 operator+(float s) const		{ return mat4(cx()+s, cy()+s, cz()+s, cw()+s); }
	inline mat4 operator-(float s) const		{ return mat4(cx()-s, cy()-s, cz()-s, cw()-s); }
	inline mat4 operator*(float s) const		{ return mat4(cx()*s, cy()*s, cz()*s, cw()*s); }
	inline mat4 operator/(float s) const		{ return mat4(cx()/s, cy()/s, cz()/s, cw()/s); }

	inline void operator+=(float s)				{ _columns[0]+=s, _columns[1]+=s, _columns[2]+=s, _columns[3]+=s; }
	inline void operator-=(float s)				{ _columns[0]-=s, _columns[1]-=s, _columns[2]-=s, _columns[3]-=s; }
	inline void operator*=(float s)				{ _columns[0]*=s, _columns[1]*=s, _columns[2]*=s, _columns[3]*=s; }
	inline void operator/=(float s)				{ _columns[0]/=s, _columns[1]/=s, _columns[2]/=s, _columns[3]/=s; }

	// Matrix addition can be used with scalar multiplication to interpolate or blend between multiple matrices.Later, you will learn how to use this property to implement animation skinning.
	inline mat4 operator+(const mat4& r) const	{ return mat4(cx()+r.cx(), cy()+r.cy(), cz()+r.cz(), cw()+r.cw()); }
	inline mat4 operator-(const mat4& r) const	{ return mat4(cx()-r.cx(), cy()-r.cy(), cz()-r.cz(), cw()-r.cw()); }

	inline void operator+=(const mat4& r)		{ _columns[0]+=r._columns[0], _columns[1]+=r._columns[1], _columns[2]+=r._columns[2], _columns[3]+=r._columns[3]; }
	inline void operator-=(const mat4& r)		{ _columns[0]-=r._columns[0], _columns[1]-=r._columns[1], _columns[2]-=r._columns[2], _columns[3]-=r._columns[3]; }

	// The resulting value for any element is the dot product of that row from the left matrix and that column form the right matrix.
	// For example, suppose you want to find the value of the element in row 2 column 3 when multiplying two matrices.
	// This means taking the dot product of row 2 from the left-hand side matrix and column 3 from the right-hand side matrix.
	/*
		The most important property of matrix multiplication is that
		it combines the transformation encoded in both matrices into a single matrix.
		This is useful because you can pre-multiply certain matrices to perform fewer multiplications per frame.
	*/
	inline mat4 operator*(const mat4& r) const {
#if 1
	#define E(ROW, COL) \
		v[0*4+ROW] * r.v[4*COL+0] + \
		v[1*4+ROW] * r.v[4*COL+1] + \
		v[2*4+ROW] * r.v[4*COL+2] + \
		v[3*4+ROW] * r.v[4*COL+3] \
// -------
//                  r0      r1      r2      r3
		return mat4(E(0,0), E(1,0), E(2,0), E(3,0), // c0
			        E(0,1), E(1,1), E(2,1), E(3,1), // c1
			        E(0,2), E(1,2), E(2,2), E(3,2), // c2
			        E(0,3), E(1,3), E(2,3), E(3,3)  // c3
		);
	#undef E
#else
		mat4 m;
#if 1 // per-row
		for (int i = 0; i < 4; ++i) {
			vec4 res;
			auto lr = row(i);
			for (int j = 0; j < 4; ++j) {
				auto rc = r.col(j);
				res[j] = lr.dot(rc);
			}
			m.setRow(i, res);
		}
#else // or per-col
		for (int i = 0; i < 4; ++i) {
			vec4 res;
			auto rc = r.col(i);
			for (int j = 0; j < 4; ++j) {
				auto lr = row(j);
				res[j] = lr.dot(rc);
			}
			m.setCol(i, res);
		}
#endif
		return m;
#endif
	}

#define M4V4D(ROW, x, y, z, w) \
	v[0*4+ROW]*x + \
	v[1*4+ROW]*y + \
	v[2*4+ROW]*z + \
	v[3*4+ROW]*w \
// --------

	inline vec4 operator* (const vec4& r) const {
#if 1
	#define E(ROW, V4) M4V4D(ROW, V4.x, V4.y, V4.z, V4.w)
		return vec4(E(0,r), E(1,r), E(2,r), E(3,r));
	#undef E
#else
		vec4 res;
		for (int i = 0; i < 4; ++i) {
			res[i] = row(i).dot(r);
		}
		return res;
#endif
	}

	inline bool operator==(const mat4& r) const { return cx()==r.cx() && cy()==r.cy() && cz()==r.cz() && cw()==r.cw(); }
	inline bool operator!=(const mat4& r) const { return !(this->operator==(r)); }

	/*
		Most of the data will be stored as three-component vectors, not four.
		There is no need to create a new four-component vector every time one needs to be transformed by a matrix.

		No need to declare 'inline vec3 operator* (const vec3& r) const',
		and use transformVector/transformPoint instead.
		This should help reduce ambiguity as to what the data being transformed is.
	*/
	inline vec3 transformVector(const vec3& vector) const {
#define E(ROW, V3) M4V4D(ROW, V3.x, V3.y, V3.z, 0)
		return vec3(E(0,vector), E(1,vector), E(2,vector));
#undef E
	}

	inline vec3 transformPoint(const vec3& point) const {
#define E(ROW, V3) M4V4D(ROW, V3.x, V3.y, V3.z, 1)
		return vec3(E(0,point), E(1,point), E(2,point));
#undef E
	}

	// The W component is a reference — it is a read-write.
	// After the function is executed,
	// the w component holds the value for W, if the input vector had been vec4
	inline vec3 transformPoint(const vec3& point, float& w) const {
		float _w = w;
		w = M4V4D(3, point.x, point.y, point.z, _w);
#define E(ROW, V3) M4V4D(ROW, V3.x, V3.y, V3.z, _w)
		return vec3(E(0,point), E(1,point), E(2,point));
#undef E
	}

	inline mat4 transpose() const {
		return mat4(xx, yx, zx, wx,
			        xy, yy, zy, wy,
			        xz, yz, zz, wz,
			        xw, yw, zw, ww);
	}

	// convert a matrix from row-major to column-major
	inline void transposed() {
		swap(yx, xy);
		swap(zx, xz);
		swap(wx, xw);
		swap(zy, yz);
		swap(wy, yw);
		swap(wz, zw);
	}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
	(v[c0*4+r0] * (v[c1*4+r1]*v[c2*4+r2] - v[c1*4+r2]*v[c2*4+r1]) - \
	 v[c1*4+r0] * (v[c0*4+r1]*v[c2*4+r2] - v[c0*4+r2]*v[c2*4+r1]) + \
	 v[c2*4+r0] * (v[c0*4+r1]*v[c1*4+r2] - v[c0*4+r2]*v[c1*4+r1]) ) \
// ---- c*4+r --> mat4 array flatten index, but why c*4? because mat4 is column-major

	inline float determinant() const {
		return v[0]  * M4_3X3MINOR(1,2,3, 1,2,3)
			 - v[4]	 * M4_3X3MINOR(0,2,3, 1,2,3)
			 + v[8]	 * M4_3X3MINOR(0,1,3, 1,2,3)
			 - v[12] * M4_3X3MINOR(0,1,2, 1,2,3);
	}

	inline mat4 adjugate() const {
		// FYI: https://zh.wikipedia.org/wiki/%E4%BC%B4%E9%9A%8F%E7%9F%A9%E9%98%B5
		// Cofactor(M[i,j]) = Minor(M[i,j]) * pow(-1, i+j)
		// Minor(M[i, j]) is determinant operator
		mat4 cofactor;

		cofactor.v[0]  =  M4_3X3MINOR(1,2,3, 1,2,3);
		cofactor.v[1]  = -M4_3X3MINOR(1,2,3, 0,2,3);
		cofactor.v[2]  =  M4_3X3MINOR(1,2,3, 0,1,3);
		cofactor.v[3]  = -M4_3X3MINOR(1,2,3, 0,1,2);
		cofactor.v[4]  = -M4_3X3MINOR(0,2,3, 1,2,3);
		cofactor.v[5]  =  M4_3X3MINOR(0,2,3, 0,2,3);
		cofactor.v[6]  = -M4_3X3MINOR(0,2,3, 0,1,3);
		cofactor.v[7]  =  M4_3X3MINOR(0,2,3, 0,1,2);
		cofactor.v[8]  =  M4_3X3MINOR(0,1,3, 1,2,3);
		cofactor.v[9]  = -M4_3X3MINOR(0,1,3, 0,2,3);
		cofactor.v[10] =  M4_3X3MINOR(0,1,3, 0,1,3);
		cofactor.v[11] = -M4_3X3MINOR(0,1,3, 0,1,2);
		cofactor.v[12] = -M4_3X3MINOR(0,1,2, 1,2,3);
		cofactor.v[13] =  M4_3X3MINOR(0,1,2, 0,2,3);
		cofactor.v[14] = -M4_3X3MINOR(0,1,2, 0,1,3);
		cofactor.v[15] =  M4_3X3MINOR(0,1,2, 0,1,2);

		// Adj(M[i,j]) = Cofactor(M[j,i])
		cofactor.transposed();
		return cofactor;
	}

	inline float determinant3x3() const {
		return xx*(yy*zz - zy*yz)
			 - yx*(xy*zz - zy*xz)
			 + zx*(xy*yz - yy*xz);
	}

	mat4 inverse() const;
	mat4 inverse3x3() const;

	// A frustum represents everything that is visible to the camera.
	// The frustum function can be used to construct a view frustum,
	// but the function parameters are not intuitive.
	// FYI: http://www.songho.ca/opengl/gl_projectionmatrix.html
	inline static mat4 s_frustum(float l, float r, float b, float t, float n, float f) {
		SGE_ASSERT(!Math::equals(l, r) && !Math::equals(t, b) && !Math::equals(n, f));

		return mat4(2*n/(r-l),   0,           0,            0,
			        0,           2*n/(t-b),   0,            0,
			        (r+l)/(r-l), (t+b)/(t-b), -(f+n)/(f-n), -1,
			        0,           0,           -2*f*n/(f-n), 0);
	}

	// A perspective matrix is built from
		// -a field of view (fov typically in degrees),
		// -an aspect ratio (the ratio of 'x:y', 'width:height')
		// -and near and far distances.
	inline static mat4 s_perspective(float fov, float aspect, float zNear, float zFar) {
		if (Math::equals0(aspect)) {
			return mat4::s_identity();
		}

		float ymax = zNear * tanf(Math::radians(fov / 2.0f));
		float xmax = ymax * aspect;

		return s_frustum(-xmax, xmax, -ymax, ymax, zNear, zFar);
//		It serves as an easy way to create a view frustum.
	}

	// An orthographic projection maps linearly to NDC space.
	// Orthographic view projections are generally useful for displaying UI or other two-dimensional elements.
	inline static mat4 s_ortho(float l, float r, float b, float t, float n, float f) {
		SGE_ASSERT(!Math::equals(l, r) && !Math::equals(t, b) && !Math::equals(n, f));
		
		return mat4(2/(r-l),      0,            0,            0,
			        0,            2/(t-b),      0,            0,
			        0,            0,            -2/(f-n),     0,
			        -(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), 1);
	}

	static mat4 s_lookAt(const vec3& eye, const vec3& aim, const vec3& up);

	static mat4 s_translate(const vec3& t);
	static mat4 s_quat(const quat& q);
	static mat4 s_scale(const vec3& s);

	static mat4 s_trs(const vec3& t, const quat& r, const vec3& s);

	static mat4 s_transform(const Transform& t);
	static mat4 s_dual_quat(const dual_quat& dq);
	void onFormat(fmt::format_context& ctx) const;
};

inline bool mat4::equals(const mat4& r, float epsilon) const {
	return cx().equals(r.cx(), epsilon)
		&& cy().equals(r.cy(), epsilon)
		&& cz().equals(r.cz(), epsilon)
		&& cw().equals(r.cw(), epsilon);
}

inline bool mat4::equals0(float epsilon) const {
	return cx().equals0(epsilon)
		&& cy().equals0(epsilon)
		&& cz().equals0(epsilon)
		&& cw().equals0(epsilon);
}

using mat4f = mat4;

SGE_FORMATTER(mat4)

}
