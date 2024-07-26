#pragma once

#include <sge_sita_ray_tracer/math/MyMat4.h>
#include <sge_sita_ray_tracer/math/MyRay3.h>

namespace sge {

template<typename T>
class MyRayTracer : public NonCopyable {
public:
	using ElementType = T;
	using MyVec2 = MyVec2<T>;
	using MyVec3 = MyVec3<T>;
	using MyMat4 = MyMat4<T>;
	using MyRay3 = MyRay3<T>;

	void init(const MyVec2& screenSize, const MyMat4& projMatrix, const MyMat4& modelview);

	MyRay3 getRay(T x, T y) const;

	void render(const MyMesh& mesh);

	const MyVec3& lightPos() const { return _lightPos; }

			Span<Color4b>		pixels()		{ return _pixels; }
	const	Span<const Color4b> pixels() const	{ return _pixels; }

	Color4b* pixels_ptr() { return _pixels.data(); }

	Color4f onPixelShading(typename const MyRay3::HitResult& hit) const;

	int	canvasWidth () const { return _canvasWidth; }
	int	canvasHeight() const { return _canvasHeight;}

	void setPixel(int x, int y, const Color4f& color) {
		if (x < 0 || x >= _canvasWidth) return;
		if (y < 0 || y >= _canvasHeight) return;
		_pixels[y * _canvasWidth + x] = my_toColor4b(color);
	}

private:
	MyMat4 _projMatrix;
	MyMat4 _modelview;

	MyMat4 _invProjMatrix;
	MyMat4 _invModelview;

	int		_canvasWidth  = 0;
	int		_canvasHeight = 0;

	Vector<Color4b> _pixels;

	MyVec3 _lightPos { T(-1.5), T(2), T(1) };
};

using MyRayTracerf = MyRayTracer<float>;
using MyRayTracerd = MyRayTracer<double>;

}