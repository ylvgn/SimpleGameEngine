#include "MyRayTracer.h"

namespace sge {

template<typename T>
void MyRayTracer<T>::init(const MyVec2& screenSize, const MyMat4& projMatrix, const MyMat4& modelview) {
	_canvasWidth  = static_cast<int>(screenSize.x);
	_canvasHeight = static_cast<int>(screenSize.y);

	_projMatrix = projMatrix;
	_modelview  = modelview;

	_invProjMatrix = projMatrix.inverse();
	_invModelview  = modelview.inverse();

	_pixels.resize(_canvasWidth * _canvasHeight);
}

template<typename T>
typename MyRayTracer<T>::MyRay3 MyRayTracer<T>::getRay(T x, T y) const {
	MyVec2 screenSize = MyVec2( T(_canvasWidth), T(_canvasHeight) );
	return MyRay3::unprojectFromInverseMatrix(_invProjMatrix, _invModelview, {x,y}, screenSize);
}

template<typename T>
void MyRayTracer<T>::render(const MyMesh& mesh) {
	MyTimer timer;
	MyRay3::HitResult out_hit;

	for (int y = 0; y < _canvasHeight; y++) {
		printf("\rrendering %d / %d                   ", y + 1, _canvasHeight);

		for (int x = 0; x < _canvasWidth; x++) {
			MyRay3 ray = getRay(T(x + 0.5), T(y + 0.5));
			out_hit.reset();

			Color4f color(0.2f, 0, 0, 1);

			//Math::MySphere3 sphere(MyVec3::s_zero(), 1);
			//if (ray.raycast(out_hit, sphere, out_hit.distance)) {
			//	color = Color4f(0,1,0,1);
			//}

			if (ray.raycast(out_hit, mesh, out_hit.distance)) {
				typename MyRay3::HitResult shadowHit;
				MyRay3 shadowRay(out_hit.point, (_lightPos - out_hit.point).normalize());
				
				color = onPixelShading(out_hit);
				if (shadowRay.raycast(shadowHit, mesh, shadowHit.distance)) {
					float s = 0.1f;
					color.r *= s;
					color.g *= s;
					color.b *= s;
				}
			}

			setPixel(x, y, color);
		}
	}

	SGE_LOG("render cost: {}s", timer.get());
}

template<typename T>
Color4f MyRayTracer<T>::onPixelShading(typename const MyRay3::HitResult& hit) const {
	auto lightDir = (_lightPos - hit.point).normalize();
	float d = static_cast<float>(lightDir.dot(hit.normal));
	return Color4f(d, d, d, 1);
}


// explicit specialization, and it will help that no need impl template member function in .h file
template class MyRayTracer<float>;
template class MyRayTracer<double>;

}

