#pragma once

namespace sge {

enum class MySampleType : u8;

struct SampleRequest /*: public NonCopyable*/ {
	using Type = MySampleType;

	const Type&		type;
	bool&			bWireFrame;
	Math::Camera3f& camera;

	float dt;
	int	  curSkinningMethod;

	bool bShowCurrentPose;
	bool bShowRestPose;
	bool bShowBindPose;
	bool bShowIKPose;
	bool bShowModelMesh;
	bool bShowEnvironment;
	bool bShowGroundRayCast;
	bool bShowAnkleRayCast;
	bool bShowAnkle2ToeRayCast;
	bool bShowToeRayCast;
	bool bShowToeAdjustRayCast;
	bool bIsCameraFollow;

	void reset();
};

}