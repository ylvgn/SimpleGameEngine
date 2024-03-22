#include "SampleRequest.h"

namespace sge {

void SampleRequest::reset(SampleType sampleType_ /*= SampleType::None*/) {
	sampleType				= sampleType_;
	curSkinningMethod		= 0;

	bWireFrame				= false;
	bShowCurrentPose		= false;
	bShowBindPose			= false;
	bShowRestPose			= false;
	bShowIKPose				= false;
	bShowModelMesh			= false;
	bShowEnvironment		= false;
	bShowGroundRayCast		= false;
	bShowAnkleRayCast		= false;
	bShowAnkle2ToeRayCast	= false;
	bShowToeRayCast			= false;
	bShowToeAdjustRayCast	= false;
	bIsCameraFollow			= false;

	camera.setProjectionType(Math::CameraProjectionType::Perspective);
	camera.setPos(0, 0,-5);
	camera.setAim(0, 0, 0);
	camera.setUp (0, 1, 0);
	camera.setFov(60.f);
	camera.setSize(1.f);
	camera.setNearClip(0.01f);
	camera.setFarClip(1000.0f);
}

}