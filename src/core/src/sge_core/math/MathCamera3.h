#pragma once

#include "Quat4.h"
#include "Ray3.h"
#include "Frustum3.h"

namespace sge { namespace Math {

enum class CameraProjectionType : u8 {
	Perspective,
	Orthogrphic,
};
SGE_ENUM_ALL_OPERATOR(CameraProjectionType)

template<class T>
struct Camera3 {
	using Type  = CameraProjectionType;

	using Vec2  = Vec2<T>;
	using Vec3  = Vec3<T>;
	using Vec4  = Vec4<T>;
	using Mat4  = Mat4<T>;
	using Quat4 = Quat4<T>;
	using Rect2 = Rect2<T>;
	using Ray3	= Ray3<T>;
	using Frustum3 = Frustum3<T>;

	void pan	(T x, T y);
	void orbit	(T x, T y);
	void move	(T x, T y, T z);
	void move	(const Vec3& v) { move(v.x, v.y, v.z); }

	void dolly	(T z);

	void setPos(T x, T y, T z)	{ setPos(Vec3(x,y,z)); }
	void setAim(T x, T y, T z)	{ setAim(Vec3(x,y,z)); }
	void setUp (T x, T y, T z)	{ setUp (Vec3(x,y,z)); }

	void setPos(const Vec3& pos)	{ _pos = pos; }
	void setAim(const Vec3& aim)	{ _aim = aim; }
	void setUp (const Vec3& up)		{ _up  = up;  }

	void setViewport(const Rect2& v)	{ _viewport = v;		}
	void setFov(const T& fov)			{ _fov		= fov;		}
	void setNearClip(const T& nearClip) { _nearClip = nearClip; }
	void setFarClip(const T& farClip)	{ _farClip	= farClip;	}
	void setProjectionType(Type type)	{ _type		= type;		}
	void setSize(const T& size)			{ _size		= size;		}

	const Vec3& pos()	const { return _pos; }
	const Vec3& aim()	const { return _aim; }
	const Vec3& up ()	const { return _up;  }
	const T&	fov()	const { return _fov; }
	const T& nearClip() const { return _nearClip; }
	const T& farClip()	const { return _farClip; }
	const Type& type()	const { return _type; }
	const T& size()		const { return _size; }

	const Rect2& viewport() const { return _viewport; }

	Ray3	getRay(const Vec2& screenPos) const;

	Mat4	viewMatrix() const;
	Mat4	projMatrix() const;
	Mat4	viewProjMatrix() const { return projMatrix() * viewMatrix(); }

	Frustum3 frustum() const { Frustum3 o; o.setByViewProjMatrix(viewProjMatrix()); return o; };

private:
	Type	_type		= Type::Perspective;
	T		_size		= T(1.0);
	T		_fov		= T(50.0);
	T		_nearClip	= T(0.1);
	T		_farClip	= T(10000.0);
	Rect2	_viewport;
	Vec3	_pos {150, 150, 200};
	Vec3	_aim {0,0,0};
	Vec3	_up  {0,1,0};
};

using Camera3f = Camera3<float>;
using Camera3d = Camera3<double>;

}} // namespace sge/Math