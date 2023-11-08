#pragma once

#include "MyCommon.h"

#include "CubicCurveExample.h"
#include "IKSolverExample.h"
#include "BallSocketConstraintExample.h"
#include "HingeSocketConstraintExample.h"

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

struct SampleRequest;
class DebugDraw;
class DebugDraw_PointLines;

class SampleContext : public NonCopyable {
	using Request		= SampleRequest;
	using Type			= MySampleType;
	using DebugDrawPL	= DebugDraw_PointLines;

#define RUN_SAMPLE__ITEM(E, SGE_FN, ...) \
	case Type::E: { \
		test_##E##_##SGE_FN(__VA_ARGS__); \
	} break; \
// ----------
#define RUN_SAMPLE__onCreate(E, ...) RUN_SAMPLE__ITEM(E, onCreate, req)
#define RUN_SAMPLE__onUpdate(E, ...) RUN_SAMPLE__ITEM(E, onUpdate, req)
#define RUN_SAMPLE__onRender(E, ...) RUN_SAMPLE__ITEM(E, onRender, req)
#define RUN_SAMPLE__onDrawUI(E, ...) RUN_SAMPLE__ITEM(E, onDrawUI, req)

#define RUN_SAMPLE(SGE_FN, ...) \
	switch (req.type) { \
		MySampleType_ENUM_LIST(RUN_SAMPLE__##SGE_FN) \
	} \
// ----------
public:

	static constexpr const Type kStartUpDefaultType = Type::None;

	void create(Request& req)	{ RUN_SAMPLE(onCreate) }
	void update(Request& req)	{ RUN_SAMPLE(onUpdate) }
	void render(Request& req)	{ RUN_SAMPLE(onRender) }
	void drawUI(Request& req)	{ RUN_SAMPLE(onDrawUI) }
#undef RUN_SAMPLE

private:
	void test_None_onCreate(Request& req);
	void test__END_onCreate(Request& req);
	void test_LitTexture_onCreate(Request& req);
	void test_AnimationScalarTrack_onCreate(Request& req);
	void test_BezierAndHermiteCurve_onCreate(Request& req);
	void test_AnimationClip_onCreate(Request& req);
	void test_MeshSkinning_onCreate(Request& req);
	void test_AnimationBlending_onCreate(Request& req);
	void test_Crossfading_onCreate(Request& req);
	void test_AdditiveBlending_onCreate(Request& req);
	void test_CCD_onCreate(Request& req);
	void test_FABRIK_onCreate(Request& req);
	void test_CCD_BallSocketConstraint_onCreate(Request& req);
	void test_FABRIK_BallSocketConstraint_onCreate(Request& req);
	void test_CCD_HingeSocketConstraint_onCreate(Request& req);
	void test_FABRIK_HingeSocketConstraint_onCreate(Request& req);
	void test_RayCastTriangle_onCreate(Request& req);
	void test_AlignFeetOnTheGround_onCreate(Request& req);
	void test_DualQuaterionMeshSkinning_onCreate(Request& req);
	void test_InstancedCrowds_onCreate(Request& req);

	void test_None_onUpdate(Request& req);
	void test__END_onUpdate(Request& req);
	void test_LitTexture_onUpdate(Request& req);
	void test_AnimationScalarTrack_onUpdate(Request& req);
	void test_BezierAndHermiteCurve_onUpdate(Request& req);
	void test_AnimationClip_onUpdate(Request& req);
	void test_MeshSkinning_onUpdate(Request& req);
	void test_AnimationBlending_onUpdate(Request& req);
	void test_Crossfading_onUpdate(Request& req);
	void test_AdditiveBlending_onUpdate(Request& req);
	void test_CCD_onUpdate(Request& req);
	void test_FABRIK_onUpdate(Request& req);
	void test_CCD_BallSocketConstraint_onUpdate(Request& req);
	void test_FABRIK_BallSocketConstraint_onUpdate(Request& req);
	void test_CCD_HingeSocketConstraint_onUpdate(Request& req);
	void test_FABRIK_HingeSocketConstraint_onUpdate(Request& req);
	void test_RayCastTriangle_onUpdate(Request& req);
	void test_AlignFeetOnTheGround_onUpdate(Request& req);
	void test_DualQuaterionMeshSkinning_onUpdate(Request& req);
	void test_InstancedCrowds_onUpdate(Request& req);

	void test_None_onRender(Request& req);
	void test__END_onRender(Request& req);
	void test_LitTexture_onRender(Request& req);
	void test_AnimationScalarTrack_onRender(Request& req);
	void test_BezierAndHermiteCurve_onRender(Request& req);
	void test_AnimationClip_onRender(Request& req);
	void test_MeshSkinning_onRender(Request& req);
	void test_AnimationBlending_onRender(Request& req);
	void test_Crossfading_onRender(Request& req);
	void test_AdditiveBlending_onRender(Request& req);
	void test_CCD_onRender(Request& req);
	void test_FABRIK_onRender(Request& req);
	void test_CCD_BallSocketConstraint_onRender(Request& req);
	void test_FABRIK_BallSocketConstraint_onRender(Request& req);
	void test_CCD_HingeSocketConstraint_onRender(Request& req);
	void test_FABRIK_HingeSocketConstraint_onRender(Request& req);
	void test_RayCastTriangle_onRender(Request& req);
	void test_AlignFeetOnTheGround_onRender(Request& req);
	void test_DualQuaterionMeshSkinning_onRender(Request& req);
	void test_InstancedCrowds_onRender(Request& req);

	void test_None_onDrawUI(Request& req);
	void test__END_onDrawUI(Request& req);
	void test_LitTexture_onDrawUI(Request& req);
	void test_AnimationScalarTrack_onDrawUI(Request& req);
	void test_BezierAndHermiteCurve_onDrawUI(Request& req);
	void test_AnimationClip_onDrawUI(Request& req);
	void test_MeshSkinning_onDrawUI(Request& req);
	void test_AnimationBlending_onDrawUI(Request& req);
	void test_Crossfading_onDrawUI(Request& req);
	void test_AdditiveBlending_onDrawUI(Request& req);
	void test_CCD_onDrawUI(Request& req);
	void test_FABRIK_onDrawUI(Request& req);
	void test_CCD_BallSocketConstraint_onDrawUI(Request& req);
	void test_FABRIK_BallSocketConstraint_onDrawUI(Request& req);
	void test_CCD_HingeSocketConstraint_onDrawUI(Request& req);
	void test_FABRIK_HingeSocketConstraint_onDrawUI(Request& req);
	void test_RayCastTriangle_onDrawUI(Request& req);
	void test_AlignFeetOnTheGround_onDrawUI(Request& req);
	void test_DualQuaterionMeshSkinning_onDrawUI(Request& req);
	void test_InstancedCrowds_onDrawUI(Request& req);

	void _onDrawGpuSkinningByDefault(Request& req);
	void _onDrawGpuSkinning(Request& req);
	void _onDrawStaticMesh(const mat4f& projection, const mat4f& view, Span<Mesh> meshes, SPtr<Texture> tex);

	void _populatePosePalette();

	void test_DualQuaterionMeshSkinning_onRender_Inner( Request& req,
														Shader* shader,
														const AnimationAttribLocation& attrLoc,
														const mat4f& model,
														bool isDualSkinning);

	void _loadExampleAsset();
	void _loadExampleAsset_AlignFeetOnTheGround();
	void _loadExampleAsset_DualQuaterionMeshSkinning();
	void _loadExampleAsset_InstancedCrowds();
	void _loadExampleShader();

	void _defaultSetAnimInfo();
	void _defaultSelectClip();
	void _defaultSetAdditiveBasePose();
	void _defaultSetPoseDebugDraw();
	void _defaultSetCamera(Request& req, const vec3f& pos = vec3f(0,0,0), const vec3f& aim = vec3f(0,3,7));

	void _loadOrBakeAnimTex();
	void _randomSetCrowdSize(size_t crowdCount);

private:
	float										_testRotation = 0.0f;

	SPtr<Shader>								_staticShader;
	SPtr<Texture>								_texture;

	UPtr< Attribute<vec3f> >					_vertexPositions;
	UPtr< Attribute<vec3f> >					_vertexNormals;
	UPtr< Attribute<vec2f> >					_vertexTexCoords;
	UPtr<IndexBuffer>							_indexBuffer;

	SPtr<DebugDraw>								_debugPoints;
	SPtr<DebugDraw>								_debugLines;

	Vector<ScalarTrack>							_scalarTracks;
	Vector<bool>								_scalarTracksIsLoop;

	SPtr<DebugDraw>								_scalarTrackLines;
	SPtr<DebugDraw>								_handlePoints;
	SPtr<DebugDraw>								_handleLines;
	SPtr<DebugDraw>								_referenceLines;

	SPtr<Skeleton>								_skeleton;
	Pose										_currentPose;
	Vector<Clip>								_clips;
	Vector<String>								_clipNames;
	int											_currentClip;
	float										_playbackTime;

	SPtr<DebugDraw>								_restPoseVisual;
	SPtr<DebugDraw>								_bindPoseVisual;
	SPtr<DebugDraw>								_currentPoseVisual;

	Vector<Mesh>								_cpuMeshes;
	AnimationInstance							_cpuAnimInfo;
	AnimationAttribLocation						_cpuAttribLoc;

	SPtr<Shader>								_skinnedShader;
	Vector<Mesh>								_gpuMeshes;
	AnimationInstance							_gpuAnimInfo;
	AnimationAttribLocation						_gpuAttribLoc;

	Vector<FastClip>							_fastClips;

	AnimationInstance							_blendAnimA;
	AnimationInstance							_blendAnimB;
	float										_elapsedBlendTime;
	bool										_isInvertBlend;

	float										_fadeTimer;

	float										_additiveTime;      // 0~1
	float										_additiveDirection; // -1,1

	UPtr< IKSolverExample<CCDSolver> >			_ccdSolver;
	UPtr< IKSolverExample<FABRIKSolver> >		_fabrikSolver;

	BallSocketConstraintExample<CCDSolver>*		_ccdBallSocketConstraint;
	BallSocketConstraintExample<FABRIKSolver>*	_fabrikBallSocketConstraint;

	HingeSocketConstraintExample<CCDSolver>*	_ccdHingeSocketConstraint;
	HingeSocketConstraintExample<FABRIKSolver>*	_fabrikHingeSocketConstraint;

	Vector<Mesh>								_ikCourse;
	Vector<Triangle>							_triangles;
	SPtr<Texture>								_ikCourseTexture;
	VectorTrack									_motionTrack;
	float										_walkingTime;
	float										_sinkIntoGround;
	SPtr<IKLeg>									_leftLeg;
	SPtr<IKLeg>									_rightLeg;
	float										_toeLength;
	float										_lastModelY;
	bool										_depthTest;
	SPtr<DebugDrawPL>							_animatedPoseDebugDraw;
	SPtr<DebugDrawPL>							_leftLegDebugDraw;
	SPtr<DebugDrawPL>							_rightLegDebugDraw;
	SPtr<DebugDrawPL>							_groundRayDebugDraw;
	SPtr<DebugDrawPL>							_ankleRayCastHitDebugDraw;
	SPtr<DebugDrawPL>							_ankleRayDebugDraw;
	SPtr<DebugDrawPL>							_ankle2ToeRayCastDebugDraw;
	SPtr<DebugDrawPL>							_toeRayCastHitDebugDraw;
	SPtr<DebugDrawPL>							_toeRayDebugDraw;
	SPtr<DebugDrawPL>							_ankleToCurrentToeDebugDraw;
	SPtr<DebugDrawPL>							_ankleToDesiredToeDebugDraw;

	SPtr<Shader>								_dqSkinnedShader;

	Vector<UPtr<Crowd>>							_crowds;
	SPtr<Shader>								_crowdSkinnedShader;
	Vector<SPtr<AnimTexture>>					_crowdAnimTextures;
};

} // namespace