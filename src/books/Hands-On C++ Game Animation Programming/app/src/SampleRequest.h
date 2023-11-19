#pragma once

namespace sge {

enum class MySampleType : u8;

struct SampleRequest : public NonCopyable {
	using Type = MySampleType;

	SampleRequest(const Type& type_, bool& bWireFrame_, Math::Camera3f& camera_)
		: type(type_)
		, bWireFrame(bWireFrame_)
		, camera(camera_) {}

	const Type&		type;
	bool&			bWireFrame;
	Math::Camera3f& camera;

	float dt = 0;
	int	  curSkinningMethod = 0;

	bool bShowCurrentPose : 1;
	bool bShowRestPose : 1;
	bool bShowBindPose : 1;
	bool bShowIKPose : 1;
	bool bShowModelMesh : 1;
	bool bShowEnvironment : 1;
	bool bShowGroundRayCast : 1;
	bool bShowAnkleRayCast : 1;
	bool bShowAnkle2ToeRayCast : 1;
	bool bShowToeRayCast : 1;
	bool bShowToeAdjustRayCast : 1;
	bool bIsCameraFollow : 1;

	void reset();
};

}