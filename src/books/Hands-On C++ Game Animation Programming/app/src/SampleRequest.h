#pragma once

namespace sge {

#define MySampleType_ENUM_LIST(E) \
	E(None,) \
	E(LitTexture,) \
	E(AnimationScalarTrack,) \
	E(BezierAndHermiteCurve, ) \
	E(AnimationClip,) \
	E(MeshSkinning,) \
	E(AnimationBlending,) \
	E(Crossfading,) \
	E(AdditiveBlending,) \
	E(CCD,) \
	E(FABRIK,) \
	E(CCD_BallSocketConstraint,) \
	E(FABRIK_BallSocketConstraint,) \
	E(CCD_HingeSocketConstraint,) \
	E(FABRIK_HingeSocketConstraint,) \
	E(RayCastTriangle,) \
	E(AlignFeetOnTheGround,) \
	E(_END,) \
// ----------
SGE_ENUM_CLASS(MySampleType, u8)
SGE_ENUM_ALL_OPERATOR(MySampleType)

class SampleRequest {
public:
	using Type = MySampleType;

	const Type& type;
	const float& aspect;
	bool& bWireFrame;
	Math::Camera3f& camera;

	float			dt;

	bool mShowRestPose;
	bool mShowCurrentPose = true;
	bool mShowBindPose;
	bool mShowIKPose;

	float mTimeMod = 1.0f;

	void reset() {
		bWireFrame = false;
		mTimeMod = 1.0f;

		// mat4f::s_lookAt(vec3f(0, 0, -5), vec3f::s_zero(), vec3f::s_up());
		camera.setPos(0, 0, -5);
		camera.setAim(0, 0, 0);

		// mat4f::s_perspective(60.0f, req.aspect, 0.01f, 1000.0f);
		camera.setFov(60.f);
		camera.setNearClip(0.01f);
		camera.setFarClip(1000.0f);
	}
};

}