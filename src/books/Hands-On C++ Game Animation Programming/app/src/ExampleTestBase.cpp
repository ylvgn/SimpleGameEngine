#include "ExampleTestBase.h"
#include "SampleRequest.h"

namespace sge {

void ExampleTestBase::_defaultSetCamera(Request& req) {
	static const vec3f kEye {
		kCamDist * Math::cos(Math::radians(kCamYaw)) * Math::sin(Math::radians(kCamPitch)),
		kCamDist * Math::cos(Math::radians(kCamPitch)),
		kCamDist * Math::sin(Math::radians(kCamYaw)) * Math::sin(Math::radians(kCamPitch))
	};

	//_matrixView = mat4f::s_lookAt(getEyesPos(), vec3f::s_zero(), vec3f::s_up());
	req.camera.setPos(kEye.x, kEye.y, kEye.z);
	req.camera.setAim(0,0,0);
	//_matrixProj = mat4f::s_perspective(60.0f, aspect, 0.01f, 100.0f);
	req.camera.setFov(60.f);
	req.camera.setNearClip(0.01f);
	req.camera.setFarClip(1000.0f);
}

}