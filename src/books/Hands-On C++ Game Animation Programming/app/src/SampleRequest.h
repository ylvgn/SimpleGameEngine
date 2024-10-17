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

struct SampleRequest : public NonCopyable {
	using SampleType = MySampleType;

	SampleRequest(bool& bWireFrame_, Math::Camera3f& camera_)
		: sampleType(SampleType::None)
		, bWireFrame(bWireFrame_)
		, camera(camera_)
		, dt(0)
		, curSkinningMethod(0)
		, bShowCurrentPose(false)
		, bShowRestPose(false)
		, bShowBindPose(false)
		, bShowIKPose(false)
		, bShowModelMesh(true)
		, bShowEnvironment(true)
		, bShowGroundRayCast(false)
		, bShowAnkleRayCast(false)
		, bShowAnkle2ToeRayCast(false)
		, bShowToeRayCast(false)
		, bShowToeAdjustRayCast(false)
		, bIsCameraFollow(false)
	{}

	SampleType		sampleType = SampleType::None;

	bool&			bWireFrame;
	Math::Camera3f& camera;

	float			dt = 0;
	int				curSkinningMethod = 0;

	bool			bShowCurrentPose		: 1;
	bool			bShowRestPose			: 1;
	bool			bShowBindPose			: 1;
	bool			bShowIKPose				: 1;
	bool			bShowModelMesh			: 1;
	bool			bShowEnvironment		: 1;
	bool			bShowGroundRayCast		: 1;
	bool			bShowAnkleRayCast		: 1;
	bool			bShowAnkle2ToeRayCast	: 1;
	bool			bShowToeRayCast			: 1;
	bool			bShowToeAdjustRayCast	: 1;
	bool			bIsCameraFollow			: 1;

	void reset(SampleType sampleType_ = SampleType::None);
}; // SampleRequest

} // namespace sge