#include "MyRayTracer.h"

namespace sge {

void MyRayTracer::init(const MyVec2& screenSize, const MyMat4& projMatrix, const MyMat4& modelview) {
	_screenSize = screenSize;
	_projMatrix = projMatrix;
	_modelview  = modelview;

	_invProjMatrix = projMatrix.inverse();
	_invModelview  = modelview.inverse();
}

MyRayTracer::MyRay3 MyRayTracer::getRay(T x, T y) {
	return MyRay3::unprojectFromInverseMatrix(_invProjMatrix, _invModelview, { x,y }, _screenSize);
}

}

