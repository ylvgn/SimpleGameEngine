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
	E(DualQuaterionMeshSkinning,) \
	E(InstancedCrowds,) \
	E(_END,) \
// ----------
SGE_ENUM_CLASS(MySampleType, u8)
SGE_ENUM_ALL_OPERATOR(MySampleType)

struct SampleRequest {
	using Type = MySampleType;

	const Type&		type;
	bool&			bWireFrame;
	Math::Camera3f& camera;

	float dt;

	bool bShowCurrentPose;
	bool bShowRestPose;
	bool bShowBindPose;
	bool bShowIKPose;
	bool bShowLinearSkinning;
	bool bShowDualQuaternionSkinning;
	bool bShowModelMesh;
	bool bShowEnvironment;
	bool bShowGroundRayCast;
	bool bShowAnkleRayCast;
	bool bShowAnkle2ToeRayCast;
	bool bShowToeRayCast;
	bool bShowToeAdjustRayCast;
	bool bIsCameraFollow;

	void reset() {
		bWireFrame					= false;
		bShowCurrentPose			= false;
		bShowBindPose				= false;
		bShowRestPose				= false;
		bShowIKPose					= false;
		bShowLinearSkinning			= false;
		bShowDualQuaternionSkinning = false;
		bShowIKPose					= false;
		bShowCurrentPose			= false;
		bShowModelMesh				= false;
		bShowEnvironment			= false;
		bShowGroundRayCast			= false;
		bShowAnkleRayCast			= false;
		bShowAnkle2ToeRayCast		= false;
		bShowToeRayCast				= false;
		bShowToeAdjustRayCast		= false;
		bIsCameraFollow				= false;

		camera.setProjectionType(Math::CameraProjectionType::Perpective);
		camera.setPos(0, 0, -5);
		camera.setAim(0, 0,  0);
		camera.setUp (0, 1,  0);
		camera.setFov(60.f);
		camera.setSize(1.f);
		camera.setNearClip(0.01f);
		camera.setFarClip(1000.0f);
	}
};

}