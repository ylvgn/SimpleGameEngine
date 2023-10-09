#include <sge_game_anime_prog.h>
#include "MyCommon.h"

#include "IKSolverExample.h"
#include "BallSocketConstraintExample.h"
#include "HingeSocketConstraintExample.h"
#include "CubicCurveExample.h"

namespace sge {

// OpenGL: https://registry.khronos.org/OpenGL/api/GL/wglext.h
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

// V Synch
typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL		(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int			(WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

using DebugDrawPL	= DebugDraw_PointLines;

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

	static constexpr float s_HeaderHeight() {
		return (NuklearUI::Window::kTitleHeight+5) * NuklearUI::scaleFactor;
	}

	const Type&		type;
	const float&	aspect;
	bool&			bWireFrame;
	Math::Camera3f&	camera;
	
	float			dt;

	bool mShowRestPose;
	bool mShowCurrentPose = true;
	bool mShowBindPose;
	bool mShowIKPose;

	float mTimeMod = 1.0f;

	void reset() {
		bWireFrame = false;
		mTimeMod = 1.0f;
	}
};

class SampleContext : public NonCopyable {
	using Request	= SampleRequest;
	using Type		= MySampleType;

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
	void test_None_onCreate(Request& req) {}
	void test__END_onCreate(Request& req) {}
	void test_LitTexture_onCreate(Request& req) {
		_debugPoints = new DebugDraw();
		_debugLines  = new DebugDraw();

		_texture		= new Texture("Assets/Textures/uvChecker.png");
		_staticShader	= new Shader(
			"Assets/Shaders/static.vert",
			"Assets/Shaders/lit.frag"
		);
		_vertexPositions	= make_unique< Attribute<vec3f> >();
		_vertexNormals		= make_unique< Attribute<vec3f> >();
		_vertexTexCoords	= make_unique< Attribute<vec2f> >();
		_indexBuffer		= make_unique<IndexBuffer>();

		Vector<vec3f, 4> positions{
			vec3f(-1, -1, 0),
			vec3f(-1,  1, 0),
			vec3f(1,  -1, 0),
			vec3f(1,   1, 0),
		};

		Vector<u32, 6> indices = {
			0,1,2,
			2,1,3
		};

		_debugPoints->appendRange(positions);

		SGE_ASSERT(indices.size() == 6);
		Vector<vec3f> lines;
		lines.resize(indices.size() * 6);
		for (int i = 0, j = 0; i < indices.size(); i += 3) {
			lines[j++] = positions[indices[i]];
			lines[j++] = positions[indices[i + 1]];

			lines[j++] = positions[indices[i + 1]];
			lines[j++] = positions[indices[i + 2]];

			lines[j++] = positions[indices[i + 2]];
			lines[j++] = positions[indices[i]];
		}

		_vertexPositions->uploadToGpu(ByteSpan_make(positions.span()));
		_indexBuffer->uploadToGpu(ByteSpan_make(indices.span()));

		_debugLines->appendRange(lines);
		_debugLines->uploadToGpu();

		_debugPoints->uploadToGpu();

		Vector<vec3f, 4> normals;
		normals.resize(4, vec3f::s_forward());
		_vertexNormals->uploadToGpu(ByteSpan_make(normals.span()));

		Vector<vec2f, 4> uvs = {
			vec2f(0,0),
			vec2f(0,1),
			vec2f(1,0),
			vec2f(1,1),
		};
		_vertexTexCoords->uploadToGpu(ByteSpan_make(uvs.span()));

		// mat4f::s_lookAt(vec3f(0, 0, -5), vec3f::s_zero(), vec3f::s_up());
		req.camera.setPos(0, 0, -5);
		req.camera.setAim(0, 0, 0);

		// mat4f::s_perspective(60.0f, req.aspect, 0.01f, 1000.0f);
		req.camera.setFov(60.f);
		req.camera.setNearClip(0.01f);
		req.camera.setFarClip(1000.0f);
	}
	void test_AnimationScalarTrack_onCreate(Request& req) {
		_referenceLines		= new DebugDraw();
		_scalarTrackLines	= new DebugDraw();
		_handlePoints		= new DebugDraw();
		_handleLines		= new DebugDraw();

		float height		= 1.8f;
		float left			= 1.0f;
		float right			= 12.f;
		float xRange		= right - left;
		float paddingX		= xRange + 1.f;

		{ // xy-coordinate figure
			// Range in both X and Y is 0 to 20, according to mat4f::s_ortho
			// l=b=0, r=t=22f
			for (int i = 0; i < 10; ++i) {
				float originY = (i * 2.f) + (i * 0.2f) + 0.1f;

				for (int j = 0; j < 2; ++j) {
					float originX = left;
					if (j % 2 != 0) {
						originX += paddingX;
					}

					vec3f o(originX, originY, 0);
					_referenceLines->push_back(o);
					_referenceLines->push_back(vec3f(o.x, o.y + height, 0));

					_referenceLines->push_back(o);
					_referenceLines->push_back(vec3f(o.x + xRange, o.y, 0));
				}
			}

			_referenceLines->uploadToGpu();
		}

		{ // linear interpolate
			_scalarTracks.push_back(TrackUtil::createScalarTrack(
				Interpolation::Linear, 2,
				FrameUtil::createFrame(0.0f, 0.0f),
				FrameUtil::createFrame(1.0f, 1.0f))
			);
			_scalarTracksIsLoop.push_back(false);

			_scalarTracks.push_back(TrackUtil::createScalarTrack(
				Interpolation::Linear, 2,
				FrameUtil::createFrame(0.0f, 0.0f),
				FrameUtil::createFrame(0.5f, 1.0f))
			);
			_scalarTracksIsLoop.push_back(false);

			{
				const auto linearTrack = TrackUtil::createScalarTrack(
					Interpolation::Linear, 3,
					FrameUtil::createFrame(0.25f, 0.f),
					FrameUtil::createFrame(0.50f, 1.f),
					FrameUtil::createFrame(0.75f, 0.f)
				);
				_scalarTracks.push_back(linearTrack);
				_scalarTracksIsLoop.push_back(true);

				_scalarTracks.push_back(linearTrack);
				_scalarTracksIsLoop.push_back(false);
			}
		}

		{ // step interpolate
			const auto stepTrack = TrackUtil::createScalarTrack(
				Interpolation::Constant, 5,
				FrameUtil::createFrame(0.20f, 1.f),
				FrameUtil::createFrame(0.25f, 0.f),
				FrameUtil::createFrame(0.30f, 1.f),
				FrameUtil::createFrame(0.35f, 0.f),
				FrameUtil::createFrame(0.40f, 1.f)
			);
			_scalarTracks.push_back(stepTrack);
			_scalarTracksIsLoop.push_back(true);

			_scalarTracks.push_back(stepTrack);
			_scalarTracksIsLoop.push_back(false);
		}

		{ // cubic interpolate
			{
				_scalarTracks.push_back(TrackUtil::createScalarTrack(
					Interpolation::Cubic, 2,
					FrameUtil::createFrame(0.25f, 0),
					FrameUtil::createFrame(0.75f, 1))
				);
				_scalarTracksIsLoop.push_back(false);

				_scalarTracks.push_back(TrackUtil::createScalarTrack(
					Interpolation::Cubic, 2,
					FrameUtil::createFrame(0.25f, 0.676221f, 0.0f, 0.676221f),
					FrameUtil::createFrame(0.75f, 4.043837f, 1.0f, 4.043837f))
				);
				_scalarTracksIsLoop.push_back(false);
			}

			{
				const auto cubicTrack = TrackUtil::createScalarTrack(
					Interpolation::Cubic, 3,
					FrameUtil::createFrame(0.25f, 0, 0, 0),
					FrameUtil::createFrame(0.50f, 0, 1, 0),
					FrameUtil::createFrame(0.75f, 0, 0, 0)
				);
				_scalarTracks.push_back(cubicTrack);
				_scalarTracksIsLoop.push_back(false);
				_scalarTracks.push_back(cubicTrack);
				_scalarTracksIsLoop.push_back(true);
			}

			{
				const auto cubicTrack = TrackUtil::createScalarTrack(
					Interpolation::Cubic, 5,
					FrameUtil::createFrame(0.25f,      0,          0,          0         ),
					FrameUtil::createFrame(0.3833333f, -10.11282f, 0.5499259f, -10.11282f),
					FrameUtil::createFrame(0.5f,       25.82528f,  1,          25.82528f ),
					FrameUtil::createFrame(0.6333333f, 7.925411f,  0.4500741f, 7.925411f ),
					FrameUtil::createFrame(0.75f,      0,          0,          0         )
				);
				_scalarTracks.push_back(cubicTrack);
				_scalarTracksIsLoop.push_back(false);

				_scalarTracks.push_back(cubicTrack);
				_scalarTracksIsLoop.push_back(true);
			}

			{
				const auto cubicTrack = TrackUtil::createScalarTrack(
					Interpolation::Cubic, 5,
				    FrameUtil::createFrame(0.25f,      0,         0,          0         ),
				    FrameUtil::createFrame(0.3833333f, 13.25147f, 0.5499259f, -10.11282f),
				    FrameUtil::createFrame(0.5f,       10.2405f,  1,          -5.545671f),
				    FrameUtil::createFrame(0.6333333f, 7.925411f, 0.4500741f, -11.40672f),
				    FrameUtil::createFrame(0.75f,      0,         0,          0         )
				);
				_scalarTracks.push_back(cubicTrack);
				_scalarTracksIsLoop.push_back(false);

				_scalarTracks.push_back(cubicTrack);
				_scalarTracksIsLoop.push_back(true);
			}
		}

		{
			// Safe to assume that _scalarTracks has a size of 10 (10 xy-coordinate figure)
			SGE_ASSERT(_scalarTracks.size() == _scalarTracksIsLoop.size());

			Track_SampleRequest sr;
			bool isRightSide = false;
			for (int i = 0; i < _scalarTracks.size(); ++i) {

				int tmp = i/2;
				float originY = ((9 - tmp) * 2.f) + ((9 - tmp) * 0.2f) + 0.1f;
				vec3f o(left, originY, 0);
				if (isRightSide) {
					o.x += paddingX;
				}
				isRightSide = !isRightSide;

				const auto& track = _scalarTracks[i];
				sr.isLoop = _scalarTracksIsLoop[i];

				// 0~149 count frame remap to 0~1 time in x-axis
				for (int j = 1; j < 150; ++j) { // j goes from 0 to 149
					float thisJNorm = (j - 1) / 149.0f;
					float nextJNorm = j / 149.0f;

					float thisX = o.x + (thisJNorm * xRange);
					float nextX = o.x + (nextJNorm * xRange);

					sr.time = thisJNorm;
					float thisY = o.y + (track.sample(sr) * height);

					sr.time = nextJNorm;
					float nextY = o.y + (track.sample(sr) * height);

					_scalarTrackLines->push_back(vec3f(thisX, thisY, 0.1f));
					_scalarTrackLines->push_back(vec3f(nextX, nextY, 0.1f));
				}

				{ // key frame display
					size_t frameCount = track.getFrameCount();
					for (int j = 0; j < frameCount; ++j) {
						const auto& keyFrame = track[j];
						const float thisTime = keyFrame.time;
						sr.time = thisTime;

						float thisX = o.x + (thisTime * xRange);
						float thisY = o.y + (track.sample(sr) * height);

						// key frame point
						vec3f thisPoint(thisX, thisY, 0.3f);
						_handlePoints->push_back(thisPoint);

						static const float tangentScale = 0.5f;

						// in-coming tangent
						if (j > 0) {
							const float preTime = thisTime - 0.0005f; // sample the point near the handle point is approximate to tangent!
							float prevX = o.x + (preTime * xRange);

							sr.time = preTime;
							float prevY = o.y + (track.sample(sr) * height);
							vec3f prePoint(prevX, prevY, thisPoint.z);

							_handleLines->push_back(thisPoint);
							_handleLines->push_back(thisPoint + (prePoint-thisPoint).normalize() * tangentScale);
						}

						// out-going tangent
						if (j < frameCount - 1 && track.type() != Interpolation::Constant) {
							const float postTime = thisTime + 0.0005f;
							float postX = o.x + (postTime * xRange);

							sr.time = postTime;
							float postY = o.y + (track.sample(sr) * height);
							vec3f postPoint(postX, postY, thisPoint.z);

							_handleLines->push_back(thisPoint);
							_handleLines->push_back(thisPoint + (postPoint - thisPoint).normalize() * tangentScale);
						}

					}
				}
			}
			_scalarTrackLines->uploadToGpu();
			_handlePoints->uploadToGpu();
			_handleLines->uploadToGpu();
		}
	}
	void test_BezierAndHermiteCurve_onCreate(Request& req) {
		_debugPoints = new DebugDraw();
		_debugLines  = new DebugDraw();

		constexpr const int kSampleCount = 200;
		constexpr const float kFloatOfSampleCount = static_cast<float>(kSampleCount);
		Vector<vec3f> points;
		Vector<vec3f> lines;

		points.reserve(6);
		lines.reserve(kSampleCount * 4 + 6 + 4);

		{ // test bezier curve
			CubicCurveExample::Bezier curve(
				vec3f(-5, 0, 0), // p1
				vec3f(-2, 1, 0), // c1
				vec3f( 2, 1, 0), // c2
				vec3f( 5, 0, 0)  // p2
			);
			points.emplace_back(curve.p1);
			points.emplace_back(curve.c1);
			points.emplace_back(curve.c2);
			points.emplace_back(curve.p2);

			lines.push_back(curve.p1); lines.push_back(curve.c1);
			lines.push_back(curve.c1); lines.push_back(curve.c2);
			lines.push_back(curve.c2); lines.push_back(curve.p2);

			for (int i = 1; i <= kSampleCount; ++i) {
				float cur  = static_cast<float>(i-1) / kFloatOfSampleCount;
				float next = static_cast<float>(i)   / kFloatOfSampleCount;
				lines.emplace_back(curve.lerp(cur));
				lines.emplace_back(curve.lerp(next));
			}
		}

		{ // test hermite spline
			CubicCurveExample::Hermite curve(
				vec3f(-5, 0,0),	// p1
				vec3f( 5, 0,0),	// p2
				vec3f( 2, 2,0),	// tan1
				vec3f(-3,-3,0)  // tan2
			);
			points.emplace_back(curve.p1);
			points.emplace_back(curve.p2);

			lines.emplace_back(curve.p1); lines.emplace_back(curve.p1 + curve.tan1.normalize());
			lines.emplace_back(curve.p2); lines.emplace_back(curve.p2 + curve.tan2.normalize());

			for (int i = 1; i <= kSampleCount; ++i) {
				float cur  = static_cast<float>(i-1) / kFloatOfSampleCount;
				float next = static_cast<float>(i)   / kFloatOfSampleCount;
				lines.emplace_back(curve.lerp(cur));
				lines.emplace_back(curve.lerp(next));
			}
		}

		_debugPoints->appendRange(points);
		_debugLines->appendRange(lines);

		_debugPoints->uploadToGpu();
		_debugLines->uploadToGpu();
	}
	void test_AnimationClip_onCreate(Request& req) {
		_restPoseVisual    = new DebugDraw();
		_bindPoseVisual    = new DebugDraw();
		_currentPoseVisual = new DebugDraw();

		_loadExampleAsset();

		_restPoseVisual->lineFromPose(_skeleton.restPose());
		_restPoseVisual->uploadToGpu();

		_bindPoseVisual->lineFromPose(_skeleton.bindPose());
		_bindPoseVisual->uploadToGpu();

		_currentClip  = 0;
		_playbackTime = 0.f;
		_currentPose  = _skeleton.restPose();
		_currentPoseVisual->lineFromPose(_currentPose);
		_currentPoseVisual->uploadToGpu();

		// play 'Walking' animation clip
		for (int i = 0; i < _clips.size(); ++i) {
			if (_clips[i].name() == "Walking") {
				_currentClip = i;
				break;
			}
		}

		req.mShowRestPose		= true;
		req.mShowCurrentPose	= true;
		req.mShowBindPose		= true;
	}
	void test_MeshSkinning_onCreate(Request& req) {
		_texture = new Texture("Assets/Textures/Woman.png");
		GLTFInfo info;
		GLTFLoader::s_readFile(info, "Assets/Mesh/Woman.gltf");

#if 1 // test optimize clip
		_fastClips.resize(info.animationClips.size());
		for (int i = 0; i < info.animationClips.size(); ++i) {
			_fastClips[i] = std::move(ClipUtil::optimizeClip(info.animationClips[i]));
		}
#endif
		_skeleton.create(info);
		_clips = std::move(info.animationClips);
		RearrangeBones::s_rearrange(_skeleton, info.skinMeshes, _fastClips.span());

		{ // test cpu skinning
			_staticShader = new Shader(
				"Assets/Shaders/static.vert",
				"Assets/Shaders/lit.frag"
			);
			_cpuMeshes.appendRange(info.skinMeshes);
			_cpuAnimInfo.animatedPose = _skeleton.restPose();
		}

		{ // test gpu skinning
#if 1 // test pre-multiplied skin matrix
			_skinnedShader = new Shader(
				"Assets/Shaders/preskinned.vert",
				"Assets/Shaders/lit.frag"
			);
#else
			_skinnedShader = new Shader(
				"Assets/Shaders/skinned.vert",
				"Assets/Shaders/lit.frag"
			);
#endif
			_gpuMeshes.appendRange(info.skinMeshes); // trigger Mesh::operator= and it will uploadToGpu
			_gpuAnimInfo.animatedPose = _skeleton.restPose();
		}

		_cpuAnimInfo.model.position = vec3f(-2, 0, 0);
		_gpuAnimInfo.model.position = vec3f( 2, 0, 0);

		_cpuAttribLoc.setByStaticShader(_staticShader);
		_gpuAttribLoc.setBySkinnedShader(_skinnedShader);

		for (int i = 0; i < _clips.size(); ++i) {
			StrView clipName = _clips[i].name();
			if (clipName == "Walking") {
				_cpuAnimInfo.clip = i;
			}
			if (clipName == "Running") {
				_gpuAnimInfo.clip = i;
			}
		}
	}
	void test_AnimationBlending_onCreate(Request& req) {
		_loadExampleAsset();
		_createExampleShader();
		_defaultSetAnimInfo();

		_blendAnimA.animatedPose = _skeleton.restPose();
		_blendAnimA.playback = 0.f;

		_blendAnimB.animatedPose = _skeleton.restPose();
		_blendAnimB.playback = 0.f;

		_elapsedBlendTime = 0.f;
		_isInvertBlend = false;

		int i = 0;
		for (auto& clip : _clips) {
			StrView clipName = clip.name();
			if (clipName == "Walking") {
				_blendAnimA.clip = i;
				_blendAnimA.playback = clip.getStartTime();
			}
			if (clipName == "Running") {
				_blendAnimB.clip = i;
				_blendAnimB.playback = clip.getStartTime();
			}
			++i;
		}
	}
	void test_Crossfading_onCreate(Request& req) {
		_loadExampleAsset();
		_createExampleShader();
		_defaultSetAnimInfo();

		CrossFadeController* fadeController = new CrossFadeController();
		fadeController->setSkeleton(&_skeleton);
		fadeController->play(&_clips[0]);

		_fadeTimer = 2.0f;
	}
	void test_AdditiveBlending_onCreate(Request& req) {
		_loadExampleAsset();
		_createExampleShader();
		_defaultSetAnimInfo();
		_defaultSelectClip();
		_defaultSetAdditiveBasePose();

		_additiveTime = 0.f;
		_additiveDirection = 1.f;
	}
	void test_CCD_onCreate(Request& req) {
		_ccdSolver = make_unique< IKSolverExample<CCDSolver> >();
		_ccdSolver->create();
	}
	void test_FABRIK_onCreate(Request& req) {
		_fabrikSolver = make_unique< IKSolverExample<FABRIKSolver> >();
		_fabrikSolver->create();
	}
	void test_CCD_BallSocketConstraint_onCreate(Request& req) {
		_ccdBallSocketConstraint = BallSocketConstraintExample<CCDSolver>::instance();
		_ccdBallSocketConstraint->create();
	}
	void test_FABRIK_BallSocketConstraint_onCreate(Request& req) {
		_fabrikBallSocketConstraint = BallSocketConstraintExample<FABRIKSolver>::instance();
		_fabrikBallSocketConstraint->create();
	}
	void test_CCD_HingeSocketConstraint_onCreate(Request& req) {
		_ccdHingeSocketConstraint = HingeSocketConstraintExample<CCDSolver>::instance();
		_ccdHingeSocketConstraint->create();
	}
	void test_FABRIK_HingeSocketConstraint_onCreate(Request& req) {
		_fabrikHingeSocketConstraint = HingeSocketConstraintExample<FABRIKSolver>::instance();
		_fabrikHingeSocketConstraint->create();
	}
	void test_RayCastTriangle_onCreate(Request& req) {
		_loadExampleAsset_AlignFeetOnTheGround();
		_createExampleShader();

		_groundRayDebugDraw = new DebugDrawPL();
		_debugPoints		= new DebugDraw();
	}
	void test_AlignFeetOnTheGround_onCreate(Request& req) {
		_loadExampleAsset();
		_loadExampleAsset_AlignFeetOnTheGround();
		_createExampleShader();
		_defaultSetAnimInfo();
		_defaultSelectClip();

		_motionTrack = TrackUtil::createVectorTrack(Interpolation::Linear, 5,
			FrameUtil::createFrame(0, vec3f(0,  0, 1 )),
			FrameUtil::createFrame(1, vec3f(0,  0, 10)),
			FrameUtil::createFrame(3, vec3f(22, 0, 10)),
			FrameUtil::createFrame(4, vec3f(22, 0, 2 )),
			FrameUtil::createFrame(6, vec3f(0,  0, 1 )) // kWalkingTimeLength = 6.0f
		);

		_leftLeg = new IKLeg();
		_leftLeg->setAnkleToGroundOffset(0.2f);
		_leftLeg->setByJointNames(_skeleton, "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase");

		_rightLeg = new IKLeg();
		_rightLeg->setAnkleToGroundOffset(0.2f);
		_rightLeg->setByJointNames(_skeleton, "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase");

		_leftLeg->setPinTrack(TrackUtil::createScalarTrack(Interpolation::Cubic, 4,
			FrameUtil::createFrame(0,		0),
			FrameUtil::createFrame(0.4f,	1),
			FrameUtil::createFrame(0.6f,	1),
			FrameUtil::createFrame(1,		0)
		));

		_rightLeg->setPinTrack(TrackUtil::createScalarTrack(Interpolation::Cubic, 4,
			FrameUtil::createFrame(0,		1),
			FrameUtil::createFrame(0.3f,	0),
			FrameUtil::createFrame(0.7f,	0),
			FrameUtil::createFrame(1,		1)
		));

		_animatedPoseDebugDraw		= new DebugDrawPL();
		_leftLegDebugDraw			= new DebugDrawPL();
		_rightLegDebugDraw			= new DebugDrawPL();
		_groundRayDebugDraw			= new DebugDrawPL();
		_ankleRayDebugDraw			= new DebugDrawPL();
		_ankleRayCastHitDebugDraw	= new DebugDrawPL();
		_ankle2ToeRayCastDebugDraw	= new DebugDrawPL();
		_toeRayDebugDraw			= new DebugDrawPL();
		_toeRayCastHitDebugDraw		= new DebugDrawPL();
		_ankleToCurrentToeDebugDraw = new DebugDrawPL();
		_ankleToDesiredToeDebugDraw = new DebugDrawPL();

		_animatedPoseDebugDraw->setLineColor(DebugDraw::kBlack);
		_leftLegDebugDraw->setLineColor(DebugDraw::kRed);
		_leftLegDebugDraw->setPointColor(DebugDraw::kRed);
		_rightLegDebugDraw->setLineColor(DebugDraw::kGreen);
		_rightLegDebugDraw->setPointColor(DebugDraw::kGreen);
		_groundRayDebugDraw->setColor(DebugDraw::kWhite);
		_ankleRayDebugDraw->setColor(DebugDraw::kYellow);
		_ankleRayCastHitDebugDraw->setColor(DebugDraw::kBlue);
		_ankle2ToeRayCastDebugDraw->setColor(DebugDraw::kWhite);
		_toeRayDebugDraw->setColor(DebugDraw::kCyan);
		_toeRayCastHitDebugDraw->setColor(DebugDraw::kBlue);
		_ankleToCurrentToeDebugDraw->setColor(DebugDraw::kRed);
		_ankleToDesiredToeDebugDraw->setColor(DebugDraw::kGreen);

		req.bWireFrame			= false;
		_depthTest				= false;

		_showIKPose				= false;
		_showCurrentPose		= false;
		_showEnvironment		= true;
		_showMesh				= true;
		_showGroundRayCast		= false;
		_showAnkleRayCast		= false;
		_showAnkle2ToeRayCast	= false;
		_showToeRayCast			= false;
		_showToeAdjustRayCast	= false;

		_walkingTime			= 0.f;
		_lastModelY				= 0.f;
		_sinkIntoGround			= 0.15f;
		_toeLength				= 0.3f;

		// Start the character clamped to the ground.
		// Move down a little bit so it's not perfectly up
		auto& model = _gpuAnimInfo.model;
		Ray groundRay(	vec3f(model.position.x, 11, model.position.z),
						vec3f::s_down()
		);

		vec3f hitPoint;
		for (auto& triangle : _triangles) {
			if (IntersectionsUtil::raycastTriangle(groundRay, triangle, hitPoint)) {
				model.position = hitPoint;
				break;
			}
		}
		model.position.y -= _sinkIntoGround;
		_lastModelY = model.position.y;

		req.mTimeMod			= 1.0f;
	}

	void test_None_onUpdate(Request& req) {}
	void test__END_onUpdate(Request& req) {}
	void test_LitTexture_onUpdate(Request& req) {
		auto& dt = req.dt;
		_testRotation += dt * 45.0f;
		while (_testRotation > 360.0f) {
			_testRotation -= 360.0f;
		}
	}
	void test_AnimationScalarTrack_onUpdate(Request& req) {}
	void test_BezierAndHermiteCurve_onUpdate(Request& req) {}
	void test_AnimationClip_onUpdate(Request& req) {
		auto& dt = req.dt;
		static float s_clipLoopingTime = 0.f;
		constexpr float kClipMaxLoopTime = 4.f;

		const Clip& clip = _clips[_currentClip];
		_playbackTime = clip.sample(_currentPose, _playbackTime + dt);

		s_clipLoopingTime += dt;
		if (s_clipLoopingTime >= kClipMaxLoopTime) {
			s_clipLoopingTime = 0.f;
			_currentClip = (_currentClip + 1) % _clips.size();
			_playbackTime = 0.f;
			_currentPose = _skeleton.restPose();
		}
		_currentPoseVisual->lineFromPose(_currentPose);
	}
	void test_MeshSkinning_onUpdate(Request& req) {
		auto& dt = req.dt;

		{ // test cpu skinning
#if 1 // test optimize clip
			auto& clip = _fastClips[_cpuAnimInfo.clip];
#else
			auto& clip = _clips[_cpuAnimInfo.clip];
#endif
			_cpuAnimInfo.playback = clip.sample(_cpuAnimInfo.animatedPose, _cpuAnimInfo.playback + dt);
#if 1 // test pre-multiplied skin matrix
			_cpuAnimInfo.animatedPose.getMatrixPalette(_cpuAnimInfo.posePalette);
			const auto& invBindPose = _skeleton.invBindPose();
			for (int i = 0; i < _cpuAnimInfo.posePalette.size(); ++i) {
				_cpuAnimInfo.posePalette[i] = _cpuAnimInfo.posePalette[i] * invBindPose[i];
			}
#endif
			for (int i = 0; i < _cpuMeshes.size(); ++i) {
#if 1 // test pre-multiplied skin matrix
				_cpuMeshes[i].cpuSkin(_cpuAnimInfo.posePalette);
#else
				_cpuMeshes[i].cpuSkin(_skeleton, _cpuAnimInfo.animatedPose);
#endif
			}
		}

		{ // test gpu skinning
#if 1 // test optimize clip
			auto& clip = _fastClips[_gpuAnimInfo.clip];
#else
			auto& clip = _clips[_gpuAnimInfo.clip];
#endif
			/*_gpuAnimInfo.playback = clip.sample(_gpuAnimInfo.animatedPose, _gpuAnimInfo.playback + dt);
			_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);*/
			_gpuAnimInfo.animatedSample(clip, dt);
			_populatePosePalette();

#if 1 // test pre-multiplied skin matrix
			const auto& invBindPose = _skeleton.invBindPose();
			for (int i = 0; i < _gpuAnimInfo.posePalette.size(); ++i) {
				_gpuAnimInfo.posePalette[i] = _gpuAnimInfo.posePalette[i] * invBindPose[i];
			}
#endif
		}
	}
	void test_AnimationBlending_onUpdate(Request& req) {
		auto& dt = req.dt;

		_blendAnimA.animatedSample(_clips, dt);
		_blendAnimB.animatedSample(_clips, dt);

		float bt = Math::clamp01(_elapsedBlendTime);
		if (_isInvertBlend) {
			bt = 1.0f - bt;
		}

		Blending::blend(_gpuAnimInfo.animatedPose, _blendAnimA.animatedPose, _blendAnimB.animatedPose, bt, -1);
		_populatePosePalette();

		_elapsedBlendTime += dt;
		if (_elapsedBlendTime >= 3.0f) { // each 3s switch animation between _blendAnimA and _blendAnimB
			_elapsedBlendTime = 0.f;
			_isInvertBlend = !_isInvertBlend;
			_gpuAnimInfo.animatedPose = _skeleton.restPose();
		}
	}
	void test_Crossfading_onUpdate(Request& req) {
		auto dt = req.dt;

		CrossFadeController* fadeController = CrossFadeController::instance();
		fadeController->update(dt);

		_fadeTimer -= dt;
		if (_fadeTimer < 0.0f) {
			_fadeTimer = 2.0f;

			int clip = _gpuAnimInfo.clip;
			while (clip == _gpuAnimInfo.clip) {
				clip = rand() % _clips.size();
			}
			//			SGE_LOG("{} -> {}", _clips[_gpuAnimInfo.clip].name(), _clips[clip].name());
			_gpuAnimInfo.clip = clip;

			fadeController->fadeTo(&_clips[clip], 0.5f);
		}

		fadeController->curPose().getMatrixPalette(_gpuAnimInfo.posePalette);
	}
	void test_AdditiveBlending_onUpdate(Request& req) {
		auto& dt = req.dt;
#if 0
		_additiveTime += dt * _additiveDirection;
		if (_additiveTime < 0) {
			_additiveTime = 0;
			_additiveDirection *= -1.f;
		}
		if (_additiveTime > 1) {
			_additiveTime = 1;
			_additiveDirection *= -1.f;
		}
#endif
		{ // sample animatedPose
			auto& clip = _clips[_gpuAnimInfo.clip];
			_gpuAnimInfo.playback = clip.sample(_gpuAnimInfo.animatedPose, _gpuAnimInfo.playback + dt);
		}

		{ // sample additivePose
			auto& clip = _clips[_gpuAnimInfo.additiveClip];
			float additivePlaybackTime = clip.getStartTime() + (clip.getDuration() * _additiveTime);
			clip.sample(_gpuAnimInfo.additivePose, additivePlaybackTime);
			Blending::add(_gpuAnimInfo.animatedPose, _gpuAnimInfo.animatedPose, _gpuAnimInfo.additivePose, _gpuAnimInfo.additiveBasePose);
		}

		_populatePosePalette();
	}
	void test_CCD_onUpdate(Request& req) {
		auto& dt = req.dt;
		_ccdSolver->update(dt);
	}
	void test_FABRIK_onUpdate(Request& req) {
		auto& dt = req.dt;
		_fabrikSolver->update(dt);
	}
	void test_CCD_BallSocketConstraint_onUpdate(Request& req) {
		auto& dt = req.dt;
		_ccdBallSocketConstraint->update(dt);
	}
	void test_FABRIK_BallSocketConstraint_onUpdate(Request& req) {
		auto& dt = req.dt;
		_fabrikBallSocketConstraint->update(dt);
	}
	void test_CCD_HingeSocketConstraint_onUpdate(Request& req) {
		auto& dt = req.dt;
		_ccdHingeSocketConstraint->update(dt);
	}
	void test_FABRIK_HingeSocketConstraint_onUpdate(Request& req) {
		auto& dt = req.dt;
		_fabrikHingeSocketConstraint->update(dt);
	}
	void test_RayCastTriangle_onUpdate(Request& req) {
		_groundRayDebugDraw->clear();
		_debugPoints->clear();

		vec3f hitPoint;
		Ray ray(
			vec3f(0, 3, 0),
			vec3f::s_down()
		);

		static constexpr const float rayHeight = 5.f;
		static constexpr const float rayHeightSq = rayHeight * rayHeight;
		_groundRayDebugDraw->add(ray.origin, ray.origin + ray.direction * rayHeight);

#if 1
		for (auto& triangle : _triangles) {
			if (IntersectionsUtil::raycastTriangle(ray, triangle, hitPoint)) {
				if ((hitPoint - ray.origin).lenSq() < rayHeightSq) {
//					SGE_LOG("{}\nv0={}\nv1={}\nv2={}\n", hitPoint, triangle.v0, triangle.v1, triangle.v2);
					_debugPoints->push_back(ray.origin);
					_debugPoints->push_back(hitPoint);
					break;
				}
			}
		}
#else
		/*
				[0, -0.0011286736, 0]
				v0=[-3.5999937, -0.001804769, -1.2090831]
				v1=[4.401746, -0.001804769, 3.2887857]
				v2=[4.401746, 0.0019281805, -1.2090831]
		*/
		vec3f v0(-3.5999937f, -0.001804769f, -1.2090831f);
		vec3f v1(4.401746f, -0.001804769f, 3.2887857f);
		vec3f v2(4.401746f, 0.0019281805f, -1.2090831f);
		auto triangle = Triangle(v0, v1, v2);
		_debugPoints->push_back(v0);
		_debugPoints->push_back(v1);
		_debugPoints->push_back(v2);

		if (IntersectionsUtil::raycastTriangle(ray, triangle, hitPoint)) {
			if ((hitPoint - ray.origin).lenSq() < rayHeightSq) {
				SGE_LOG("{}\nv0={}\nv1={}\nv2={}\n", hitPoint, triangle.v0, triangle.v1, triangle.v2);
				_debugPoints->push_back(ray.origin);
				_debugPoints->push_back(hitPoint);
			}
		}
#endif
	}
	void test_AlignFeetOnTheGround_onUpdate(Request& req) {
		auto dt = req.dt * req.mTimeMod;

		_groundRayDebugDraw->clear();
		_ankleRayDebugDraw->clear();
		_ankleRayCastHitDebugDraw->clear();
		_ankle2ToeRayCastDebugDraw->clear();
		_toeRayDebugDraw->clear();
		_toeRayCastHitDebugDraw->clear();
		_ankleToCurrentToeDebugDraw->clear();
		_ankleToDesiredToeDebugDraw->clear();

		vec3f hitPoint;
		Track_SampleRequest sr;

		auto& animInfo			= _gpuAnimInfo;
		auto& model				= animInfo.model;
		auto& curAnimatedPose	= animInfo.animatedPose;

		// Increment time and sample the animation clip that moves the model on the level rails
		// The Y position is a lie, it's a trackt hat only makes sense from an ortho top view

		constexpr static const float kWalkingSpeed = 0.3f;
		constexpr static const float kWalkingTimeLength = 6.f;

		_walkingTime += dt * kWalkingSpeed;
		while (_walkingTime > kWalkingTimeLength) {
			_walkingTime -= kWalkingTimeLength;
		}

		// Figure out the X and Z position of the model in world spcae
		// Keep the Y position the same as last frame for both to properly orient the model
		float lastYPosition		= model.position.y;

		sr.isLoop				= true;
		sr.time					= _walkingTime;
		vec3f currentPosition	= _motionTrack.sample(sr);

		sr.isLoop				= true;
		sr.time					= _walkingTime + 0.1f;
		vec3f nextPosition		= _motionTrack.sample(sr);

		currentPosition.y		= lastYPosition;
		nextPosition.y			= lastYPosition;

		model.position			= currentPosition;
#if 1
		static constexpr float kRotationSensitive = 10.f;

		// Figure out the forward direction of the model in world spcae
		vec3f forwardDir      = (nextPosition - currentPosition).normalize();
		quat newDirectionQuat = quat::s_lookRotation(forwardDir, vec3f::s_up());
		if (model.rotation.dot(newDirectionQuat) < 0.0f) {
			newDirectionQuat *= -1.0f;
		}
		model.rotation = model.rotation.nlerp(newDirectionQuat, dt * kRotationSensitive);
		vec3f characterForward = model.forward();

		// Figure out the Y position of the model in world spcae
		Ray groundRay(
			vec3f(model.position.x, 11, model.position.z),
			vec3f::s_down()
		);
		for (auto& triangle : _triangles) {
			if (IntersectionsUtil::raycastTriangle(groundRay, triangle, hitPoint)) {
				// Sink the model a little bit into the ground to avoid hyper-extending it's legs
				model.position = hitPoint - vec3f(0, _sinkIntoGround, 0);
				_groundRayDebugDraw->add(groundRay.origin, hitPoint);
				break;
			}
		}
		_groundRayDebugDraw->add(currentPosition, nextPosition); // character forward

		// Sample the current animation, update the pose visual
		// and figure out where the left and right leg are in their up/down animation cycle
		animInfo.animatedSample(_clips, dt);
		_animatedPoseDebugDraw->lineFromPose(curAnimatedPose);

		float normalizedTime = animInfo.getNormalizedTime(_clips);
		normalizedTime = Math::clamp01(normalizedTime);

		Transform lAnkleTran = animInfo.getAnimatedPoseGlobalTransform(_leftLeg->ankle());
		Transform rAnkleTran = animInfo.getAnimatedPoseGlobalTransform(_rightLeg->ankle());

		vec3f  leftAnkleWorldPos = Transform::s_combine(model, lAnkleTran).position;
		vec3f rightAnkleWorldPos = Transform::s_combine(model, rAnkleTran).position;

		// Construct a ray for the left/right ankle,
		// store 
			// the world position
			// and the predictive position of the ankle.
		// This is in case the raycasts below don't hit anything.
		Ray leftAnkleRay(
			leftAnkleWorldPos + vec3f(0, 2, 0),
			vec3f::s_down()
		);
		Ray rightAnkleRay(
			rightAnkleWorldPos + vec3f(0, 2, 0),
			vec3f::s_down()
		);

		// Perform some raycasts for the feet, these are done in world space
		// and will define the IK based target points.
		// For each ankle, we need to know
		// the current position (raycast from knee height to the sole of the foot height)
		// and the predictive position (infinate ray cast).
		// The target point will be between these two goals
			// 1. current position
			// 2. predictive position
		vec3f  predictiveLeftAnkle = leftAnkleWorldPos;
		vec3f predictiveRightAnkle = rightAnkleWorldPos;
		vec3f groundReference = model.position;
		constexpr const float rayHeight = 2.1f;
		constexpr const float rayHeightSq = rayHeight * rayHeight;
		for (auto& triangle : _triangles) {
			if (IntersectionsUtil::raycastTriangle(leftAnkleRay, triangle, hitPoint)) {
				if ((hitPoint - leftAnkleRay.origin).lenSq() < rayHeightSq) {
					leftAnkleWorldPos = hitPoint;
					_ankleRayDebugDraw->add(leftAnkleRay.origin, hitPoint);

					if (hitPoint.y < groundReference.y) {
						groundReference = hitPoint - vec3f(0, _sinkIntoGround, 0);
						SGE_LOG("{}", groundReference);
					}
				}
				predictiveLeftAnkle = hitPoint;
			}
			if (IntersectionsUtil::raycastTriangle(rightAnkleRay, triangle, hitPoint)) {
				if ((hitPoint - rightAnkleRay.origin).lenSq() < rayHeightSq) {
					rightAnkleWorldPos = hitPoint;
					_ankleRayDebugDraw->add(rightAnkleRay.origin, hitPoint);

					if (hitPoint.y < groundReference.y) {
						groundReference = hitPoint - vec3f(0, _sinkIntoGround, 0);
						SGE_LOG("{}", groundReference);
					}
				}
				predictiveRightAnkle = hitPoint;
			}
		}
		_ankleRayCastHitDebugDraw->add( leftAnkleRay.origin,  leftAnkleRay.origin +  leftAnkleRay.direction * rayHeight);
		_ankleRayCastHitDebugDraw->add(rightAnkleRay.origin, rightAnkleRay.origin + rightAnkleRay.direction * rayHeight);
#if 1
		// Lerp the Y position of the mode over a small period of time
		// Just to avoid poping
		model.position.y	= _lastModelY;
		model.position		= model.position.lerp(groundReference, dt * 10.0f);
		_lastModelY			= model.position.y;
#endif

#if 1
		sr.isLoop = true;
		sr.time = normalizedTime;
		float wLeftMotion = _leftLeg->pinTrack().sample(sr);

		sr.isLoop = true;
		sr.time = normalizedTime;
		float wRightMotion = _rightLeg->pinTrack().sample(sr);

		// Lerp between fully clamped to the ground,
		// and somewhat clamped to the ground based on the current phase of the walk cycle
		 leftAnkleWorldPos =  leftAnkleWorldPos.lerp( predictiveLeftAnkle,  wLeftMotion); // 01, interpolate
		rightAnkleWorldPos = rightAnkleWorldPos.lerp(predictiveRightAnkle, wRightMotion);
#endif

		// Now that we know the position of the model, as well as the ankle
		// we can solve the feet.
		 _leftLeg->solveForLeg(model, curAnimatedPose, leftAnkleWorldPos);
		_rightLeg->solveForLeg(model, curAnimatedPose, rightAnkleWorldPos);

		 _leftLegDebugDraw->fromIKSolver( _leftLeg->solver());
		_rightLegDebugDraw->fromIKSolver(_rightLeg->solver());

		// Apply the solved feet
		Blending::blend(curAnimatedPose, curAnimatedPose,  _leftLeg->outPose(), 1.f,  _leftLeg->hip());
		Blending::blend(curAnimatedPose, curAnimatedPose, _rightLeg->outPose(), 1.f, _rightLeg->hip());
#endif

#if 1
		// The toes are still wrong, let's fix those.
		// First, construct some rays for the toes
		Transform  leftAnkleWorld = Transform::s_combine(model, curAnimatedPose.getGlobalTransform( _leftLeg->ankle()));
		Transform rightAnkleWorld = Transform::s_combine(model, curAnimatedPose.getGlobalTransform(_rightLeg->ankle()));

		vec3f  leftToeWorldPos = Transform::s_combine(model, curAnimatedPose.getGlobalTransform( _leftLeg->toe())).position;
		vec3f rightToeWorldPos = Transform::s_combine(model, curAnimatedPose.getGlobalTransform(_rightLeg->toe())).position;

		vec3f origin = leftAnkleWorld.position;
		origin.y	 = leftToeWorldPos.y;
		vec3f offset = characterForward * _toeLength;
		Ray leftToeRay(
			origin + offset + vec3f(0, 1, 0),
			vec3f::s_down()
		);
		_ankle2ToeRayCastDebugDraw->add(origin, origin + offset);

		origin   = rightAnkleWorld.position;
		origin.y = rightToeWorldPos.y;
		Ray rightToeRay = Ray(
			origin + offset + vec3f(0, 1, 0),
			vec3f::s_down()
		);
		_ankle2ToeRayCastDebugDraw->add(origin, origin + offset);

		// Next, see if the toes hit anything
		vec3f  leftToeTarget		= leftToeWorldPos;
		vec3f rightToeTarget		= rightToeWorldPos;
		vec3f  predictiveLeftToe	= leftToeWorldPos;
		vec3f predictiveRightToe	= rightToeWorldPos;
		constexpr const float ankleRayHeight   = 1.1f;
		constexpr const float ankleRayHeightSq = ankleRayHeight * ankleRayHeight;
		for (auto& triangle : _triangles) {
			if (IntersectionsUtil::raycastTriangle(leftToeRay, triangle, hitPoint)) {
				if ((hitPoint - leftToeRay.origin).lenSq() < ankleRayHeightSq) {
					leftToeTarget = hitPoint;
					_toeRayDebugDraw->add(leftToeRay.origin, hitPoint);
				}
				predictiveLeftToe = hitPoint;
			}
			if (IntersectionsUtil::raycastTriangle(rightToeRay, triangle, hitPoint)) {
				if ((hitPoint - rightToeRay.origin).lenSq() < ankleRayHeightSq) {
					rightToeTarget = hitPoint;
					_toeRayDebugDraw->add(rightToeRay.origin, hitPoint);
				}
				predictiveRightToe = hitPoint;
			}
		}
		_toeRayCastHitDebugDraw->add( leftToeRay.origin,  leftToeRay.origin +  leftToeRay.direction * ankleRayHeight);
		_toeRayCastHitDebugDraw->add(rightToeRay.origin, rightToeRay.origin + rightToeRay.direction * ankleRayHeight);

		// Place the toe target at the right location
		leftToeTarget  =  leftToeTarget.lerp( predictiveLeftToe,  wLeftMotion);
		rightToeTarget = rightToeTarget.lerp(predictiveRightToe, wRightMotion);

		constexpr static const float kEpsilon = 0.00001f;

		// If the left or right toe hit, adjust the ankle rotation approrpaiteley
		vec3f leftAnkleToCurrentToe = leftToeWorldPos - leftAnkleWorld.position;
		vec3f leftAnkleToDesiredToe = leftToeTarget   - leftAnkleWorld.position;
		_ankleToCurrentToeDebugDraw->add(leftToeWorldPos, leftAnkleWorld.position);
		_ankleToDesiredToeDebugDraw->add(leftToeTarget,   leftAnkleWorld.position);

		if (leftAnkleToCurrentToe.dot(leftAnkleToDesiredToe) > kEpsilon) { // degrees range (-90, 90), why ???
			Transform ankleLocalTran = curAnimatedPose.getLocalTransform(_leftLeg->ankle());
			const quat4f& leftAnkleWorldRot = leftAnkleWorld.rotation;
		#if 1 // calc in world space
			quat4f currentToDesired	= quat::s_fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
			quat4f worldRotated		= leftAnkleWorldRot * currentToDesired;
			quat4f localRotated		= worldRotated * leftAnkleWorldRot.inverse();
		#else // calc in local space
			quat4f leftAnkleWorldRotInv		= leftAnkleWorldRot.inverse();
			leftAnkleToCurrentToe			= leftAnkleWorldRotInv * leftAnkleToCurrentToe;
			leftAnkleToDesiredToe			= leftAnkleWorldRotInv * leftAnkleToDesiredToe;
			quat4f localRotated				= quat4f::s_fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
		#endif
			ankleLocalTran.rotation = localRotated * ankleLocalTran.rotation;
			curAnimatedPose.setLocalTransform(_leftLeg->ankle(), ankleLocalTran);
		}

		vec3f rightAnkleToCurrentToe = rightToeWorldPos - rightAnkleWorld.position;
		vec3f rightAnkleToDesiredToe = rightToeTarget   - rightAnkleWorld.position;
		_ankleToCurrentToeDebugDraw->add(rightToeWorldPos, rightAnkleWorld.position);
		_ankleToDesiredToeDebugDraw->add(rightToeTarget,   rightAnkleWorld.position);

		if (rightAnkleToCurrentToe.dot(rightAnkleToDesiredToe) > kEpsilon) {
			Transform ankleLocalTran = curAnimatedPose.getLocalTransform(_rightLeg->ankle());
			const quat4f& rightAnkleWorldRot = rightAnkleWorld.rotation;
		#if 1 // calc in world space
			quat4f currentToDesired = quat::s_fromTo(rightAnkleToCurrentToe, rightAnkleToDesiredToe);
			quat4f worldRotated		= rightAnkleWorldRot * currentToDesired;
			quat4f localRotated		= worldRotated * rightAnkleWorldRot.inverse();
		#else
			quat4f rightAnkleWorldRotInv	= rightAnkleWorldRot.inverse();
			rightAnkleToCurrentToe			= rightAnkleWorldRotInv * rightAnkleToCurrentToe;
			rightAnkleToDesiredToe			= rightAnkleWorldRotInv * rightAnkleToDesiredToe;
			quat4f localRotated				= quat4f::s_fromTo(rightAnkleToCurrentToe, rightAnkleToDesiredToe);
		#endif
			ankleLocalTran.rotation = localRotated * ankleLocalTran.rotation;
			curAnimatedPose.setLocalTransform(_rightLeg->ankle(), ankleLocalTran);
		}
#endif

		// Update the matrix palette for skinning
		_populatePosePalette();
	}

	void test_None_onRender(Request& req) {}
	void test__END_onRender(Request& req) {}
	void test_LitTexture_onRender(Request& req) {
		mat4f projection = mat4f::cast(req.camera.projMatrix());
		mat4f view = mat4f::cast(req.camera.viewMatrix());
		mat4f model = mat4f::s_quat(quat4f::s_angleAxis(Math::radians(_testRotation), vec3f::s_forward())); // or mat4f::s_identity();

		mat4f mvp = projection * view * model;

		_staticShader->bind();
		{
			{ // bind uniforms
				Uniform<mat4f>::set(_staticShader->findUniformByName("model"), model);
				Uniform<mat4f>::set(_staticShader->findUniformByName("view"), view);
				Uniform<mat4f>::set(_staticShader->findUniformByName("projection"), projection);
				Uniform<vec3f>::set(_staticShader->findUniformByName("light"), vec3f::s_forward());
				_texture->set(_staticShader->findUniformByName("tex0"), 0);
			}

			{ // bind attributes
				_vertexPositions->bind(_staticShader->findAttributeByName("position"));
				_vertexNormals->bind(_staticShader->findAttributeByName("normal"));
				_vertexTexCoords->bind(_staticShader->findAttributeByName("texCoord"));
			}

			DrawUtil::draw(*_indexBuffer.get());
			_debugLines->draw(DebugDrawMode::Lines, mvp);
			_debugPoints->draw(DebugDrawMode::Points, mvp, DebugDraw::kBlue);

			{ // unbind uniforms
				_texture->unset(0);
				_vertexPositions->unbind(_staticShader->findAttributeByName("position"));
				_vertexNormals->unbind(_staticShader->findAttributeByName("normal"));
				_vertexTexCoords->unbind(_staticShader->findAttributeByName("texCoord"));
			}
		}
		_staticShader->unbind();
	}
	void test_AnimationScalarTrack_onRender(Request& req) {
		float t = 22.f;
		float b = 0;
		float l = 0;
		float r = req.aspect * t;
		float n = 0.01f;
		float f = 5.f;
		mat4f projection = mat4f::s_ortho(l, r, b, t, n, f);
		mat4f view       = mat4f::s_lookAt(vec3f(0, 0, (n + f) / 2), vec3f::s_zero(), vec3f::s_up());
		mat4f mvp        = projection * view * mat4f::s_identity();

		_referenceLines->draw(DebugDrawMode::Lines, mvp, DebugDraw::kYellow);
		_scalarTrackLines->draw(DebugDrawMode::Lines, mvp, DebugDraw::kGreen);
		_handlePoints->draw(DebugDrawMode::Points, mvp, DebugDraw::kBlue);
		_handleLines->draw(DebugDrawMode::Lines, mvp, DebugDraw::kPurple);
	}
	void test_BezierAndHermiteCurve_onRender(Request& req) {
		mat4f projection = mat4f::s_perspective(60.0f, req.aspect, 0.01f, 1000.0f);
		mat4f view       = mat4f::s_lookAt(vec3f(0, 0, -5), vec3f::s_zero(), vec3f::s_up());
		mat4f mvp        = projection * view * mat4f::s_identity();
		_debugLines->draw(DebugDrawMode::Lines, mvp);
		_debugPoints->draw(DebugDrawMode::Points, mvp, DebugDraw::kBlue);
	}
	void test_AnimationClip_onRender(Request& req) {
		mat4f projection = mat4f::s_perspective(60.0f, req.aspect, 0.01f, 10.f);
		mat4f view = mat4f::s_lookAt(vec3f(0, 4, -7), vec3f(0, 4, 0), vec3f::s_up());
		mat4f mvp = projection * view;

		if (req.mShowRestPose) {
			_restPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kRed);
		}

		if (req.mShowCurrentPose) {
			_currentPoseVisual->uploadToGpu();
			_currentPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kBlue);
		}
		if (req.mShowBindPose) {
			_bindPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kGreen);
		}
	}
	void test_MeshSkinning_onRender(Request& req) {
		mat4f projection = mat4f::s_perspective(60.0f, req.aspect, 0.01f, 10.f);
		mat4f view = mat4f::s_lookAt(vec3f(0, 5, 7), vec3f(0, 3, 0), vec3f::s_up());

		{ // test cpu skinning
			mat4f model = mat4f::s_transform(_cpuAnimInfo.model);

			_staticShader->bind();
			{
				{ // bind uniforms
					Uniform<mat4f>::set(_staticShader->findUniformByName("model"), model);
					Uniform<mat4f>::set(_staticShader->findUniformByName("view"), view);
					Uniform<mat4f>::set(_staticShader->findUniformByName("projection"), projection);
					Uniform<vec3f>::set(_staticShader->findUniformByName("light"), vec3f::s_one());
					_texture->set(_staticShader->findUniformByName("tex0"), 0);
				}

				for (int i = 0; i < _cpuMeshes.size(); ++i) {
					_drawMesh(_cpuMeshes[i], _cpuAttribLoc);
				}
				_texture->unset(0);
			}
			_staticShader->unbind();
		}

		{ // test gpu skinning
			mat4f model = mat4f::s_transform(_gpuAnimInfo.model);

			// bind uniform
			_skinnedShader->bind();
			{
				{ // bind uniforms
					Uniform<mat4f>::set(_skinnedShader->findUniformByName("model"), model);
					Uniform<mat4f>::set(_skinnedShader->findUniformByName("view"), view);
					Uniform<mat4f>::set(_skinnedShader->findUniformByName("projection"), projection);
					Uniform<vec3f>::set(_skinnedShader->findUniformByName("light"), vec3f::s_one());
#if 1 // test pre-multiplied skin matrix
					Uniform<mat4f>::set(_skinnedShader->findUniformByName("animated"), _gpuAnimInfo.posePalette);
#else
					Uniform<mat4f>::set(_skinnedShader->findUniformByName("invBindPose"), _skeleton.invBindPose());
					Uniform<mat4f>::set(_skinnedShader->findUniformByName("pose"), _gpuAnimInfo.posePalette);
#endif
					_texture->set(_skinnedShader->findUniformByName("tex0"), 0);
				}

				for (int i = 0; i < _gpuMeshes.size(); ++i) {
					_drawMesh(_gpuMeshes[i], _gpuAttribLoc);
				}
				_texture->unset(0);
			}
			_skinnedShader->unbind();
		}
	}
	void test_AnimationBlending_onRender(Request& req) {
		_onDrawGpuSkinningByDefault(req);
	}
	void test_Crossfading_onRender(Request& req) {
		_onDrawGpuSkinningByDefault(req);
	}
	void test_AdditiveBlending_onRender(Request& req) {
		_onDrawGpuSkinningByDefault(req);
	}
	void test_CCD_onRender(Request& req) {
		_ccdSolver->render(req.aspect);
	}
	void test_FABRIK_onRender(Request& req) {
		_fabrikSolver->render(req.aspect);
	}
	void test_CCD_BallSocketConstraint_onRender(Request& req) {
		_ccdBallSocketConstraint->render(req.aspect);
	}
	void test_FABRIK_BallSocketConstraint_onRender(Request& req) {
		_fabrikBallSocketConstraint->render(req.aspect);
	}
	void test_CCD_HingeSocketConstraint_onRender(Request& req) {
		_ccdHingeSocketConstraint->render(req.aspect);
	}
	void test_FABRIK_HingeSocketConstraint_onRender(Request& req) {
		_fabrikHingeSocketConstraint->render(req.aspect);
	}
	void test_RayCastTriangle_onRender(Request& req) {
		req.bWireFrame = true;

		mat4f projection = mat4f::s_perspective(60.0f, req.aspect, 0.01f, 100.f);
		mat4f view = mat4f::s_lookAt(vec3f(0, 7, 20), vec3f::s_zero(), vec3f::s_up());
		mat4f mvp = projection * view * mat4f::s_identity();
		_onDrawStaticMesh(projection, view, _ikCourse, _ikCourseTexture);

		_debugPoints->uploadToGpu();
		_debugPoints->draw(DebugDrawMode::Points, mvp, DebugDraw::kGreen);
		_groundRayDebugDraw->draw(mvp);
	}
	void test_AlignFeetOnTheGround_onRender(Request& req) {
		auto& modelTran = _gpuAnimInfo.model;
		float modelPosX = modelTran.position.x;
		float modelPosZ = modelTran.position.z;
		mat4 model		= mat4::s_transform(modelTran);

		mat4 projection = mat4::s_perspective(60.0f, req.aspect, 0.01f, 1000.0f);
		mat4 view = mat4::s_lookAt(
			vec3f(modelPosX, 0, modelPosZ) + vec3f(0, 5, 10),
			vec3f(modelPosX, 0, modelPosZ) + vec3f(0, 3, 0),
			vec3f::s_up());
		
		mat4 vp		= projection * view;
		mat4 mvp	= vp * model;

		if (_showMesh) {
			_onDrawGpuSkinning(projection, view);
		}

		if (_showEnvironment) {
			_onDrawStaticMesh(projection, view, _ikCourse, _ikCourseTexture);
		}

		if (!_depthTest) glDisable(GL_DEPTH_TEST);
		{
			if (_showCurrentPose) {
				_animatedPoseDebugDraw->draw(mvp, DebugDrawPL::Mask::Line);
			}
			if (_showIKPose) {
				_leftLegDebugDraw->draw(vp, DebugDrawPL::Mask::Line);
				_rightLegDebugDraw->draw(vp, DebugDrawPL::Mask::Line);
			}
			if (_showGroundRayCast) {
				_groundRayDebugDraw->draw(vp);
			}
			if (_showAnkleRayCast) {
				_ankleRayCastHitDebugDraw->draw(vp);
				_ankleRayDebugDraw->draw(vp);
			}
			if (_showAnkle2ToeRayCast) {
				_ankle2ToeRayCastDebugDraw->draw(vp);
			}
			if (_showToeRayCast) {
				_toeRayCastHitDebugDraw->draw(vp);
				_toeRayDebugDraw->draw(vp);
			}
			if (_showToeAdjustRayCast) {
				_ankleToCurrentToeDebugDraw->draw(vp);
				_ankleToDesiredToeDebugDraw->draw(vp);
			}
		}
		if (!_depthTest) glEnable(GL_DEPTH_TEST);
	}
	
	void test_None_onDrawUI(Request& req) {
		NuklearUI::demo();
	}
	void test__END_onDrawUI(Request& req) {}
	void test_LitTexture_onDrawUI(Request& req) {}
	void test_AnimationScalarTrack_onDrawUI(Request& req) {}
	void test_BezierAndHermiteCurve_onDrawUI(Request& req) {}
	void test_AnimationClip_onDrawUI(Request& req) {
		Rect2f xywh { 5.0f, 5.0f + req.s_HeaderHeight(), 300.0f, 135.0f};
		NuklearUI::Window window(xywh);
		
		static const float layout[] = { 75, 200 };
		NuklearUI::LayoutRow row(NK_STATIC, 25, 2, layout);
		NuklearUI::Label("Animation:");
		NuklearUI::Combo combo(_clipNames, _currentClip, 25, { 200, 200 });
		if (combo.selectedIndex() != _currentClip) {
			_currentPose = _skeleton.restPose();
			_currentClip = combo.selectedIndex();
		}

		NuklearUI::Label("Playback:");
		auto& clip = _clips[_currentClip];
		float startTime = clip.getStartTime();
		float duration	= clip.getDuration();
		float progress = (_playbackTime - startTime) / duration;
		size_t prog = static_cast<size_t>(progress * 200.0f);
		NuklearUI::Progress(prog, 200, NK_FIXED);

		NuklearUI::LayoutRowDynamic(20, 1);

		NuklearUI::CheckboxLabel cb1("Show Rest Pose", req.mShowRestPose);
		if (cb1.isOpen()) {
			req.mShowRestPose = cb1.isActive();
		}
		NuklearUI::CheckboxLabel cb2("Show Current Pose", req.mShowCurrentPose);
		if (cb2.isOpen()) {
			req.mShowCurrentPose = cb2.isActive();
		}
		NuklearUI::CheckboxLabel cb3("Show Bind Pose", req.mShowBindPose);
		if (cb3.isOpen()) {
			req.mShowBindPose = cb3.isActive();
		}
	}
	void test_MeshSkinning_onDrawUI(Request& req) {}
	void test_AnimationBlending_onDrawUI(Request& req) {}
	void test_Crossfading_onDrawUI(Request& req) {}
	void test_AdditiveBlending_onDrawUI(Request& req) {
		auto& currentClip		= _gpuAnimInfo.clip;
		auto& playbackTime		= _gpuAnimInfo.playback;
		auto& mAdditiveIndex	= _gpuAnimInfo.additiveClip;
		auto& mAdditiveBase		= _gpuAnimInfo.additiveBasePose;
		auto& mSkeleton			= _skeleton;

		Rect2f xywh{ 5.0f, 5.0f + req.s_HeaderHeight(), 300.0f, 225.0f };
		NuklearUI::Window window(xywh);
		static const float layout[] = { 75, 200 };
		NuklearUI::LayoutRow(NK_STATIC, 25, 2, layout);

		NuklearUI::Label("Animation:");
		NuklearUI::Combo combo(_clipNames, currentClip, 25, { 200, 200 });
		if (combo.selectedIndex() != currentClip) {
			_gpuAnimInfo.animatedPose = mSkeleton.restPose();
			currentClip = combo.selectedIndex();
		}

		NuklearUI::Label("Playback:");
		auto& clip = _clips[currentClip];
		float startTime = clip.getStartTime();
		float duration = clip.getDuration();
		float progress = (playbackTime - startTime) / duration;
		size_t prog = static_cast<size_t>(progress * 200.0f);
		NuklearUI::Progress(prog, 200, NK_FIXED);

		NuklearUI::LayoutRowDynamic(20, 1);
#if 0
		NuklearUI::CheckboxLabel cb1("Show Rest Pose", req.mShowRestPose);
		if (cb1.isOpen()) {
			req.mShowRestPose = cb1.isActive();
		}
#endif
		NuklearUI::Label("Additive:");
		NuklearUI::Combo combo2(_clipNames, mAdditiveIndex, 25, { 200, 200 });
		if (combo2.selectedIndex() != mAdditiveIndex) {
			_clips[mAdditiveIndex].setIsLoop(true);
			mAdditiveIndex = combo2.selectedIndex();
			mAdditiveBase = Blending::makeAdditiveBasePose(_skeleton, _clips[mAdditiveIndex]);
			_clips[mAdditiveIndex].setIsLoop(true);
		}

		NuklearUI::Label("Strength:");
		NuklearUI::SliderFloat(&_additiveTime, 0.0f, 1.0f);
#if 0
		NuklearUI::LayoutRowDynamic(20, 1);
		NuklearUI::CheckboxLabel cb("Show Current Pose", req.mShowCurrentPose);
		if (cb.isOpen()) {
			req.mShowCurrentPose = cb.isActive();
		}
#endif
	}
	void test_CCD_onDrawUI(Request& req) {}
	void test_FABRIK_onDrawUI(Request& req) {}
	void test_CCD_BallSocketConstraint_onDrawUI(Request& req) {}
	void test_FABRIK_BallSocketConstraint_onDrawUI(Request& req) {}
	void test_CCD_HingeSocketConstraint_onDrawUI(Request& req) {}
	void test_FABRIK_HingeSocketConstraint_onDrawUI(Request& req) {}
	void test_RayCastTriangle_onDrawUI(Request& req) {}
	void test_AlignFeetOnTheGround_onDrawUI(Request& req) {
		Rect2f xywh { 5.0f, 5.0f + req.s_HeaderHeight(), 300.0f, 330.0f };
		NuklearUI::Window window(xywh);
		static const float layout[] = { 75, 200 };
		NuklearUI::LayoutRow row(NK_STATIC, 25, 2, layout);

		NuklearUI::Label("Time speed:");
		NuklearUI::SliderFloat(&req.mTimeMod, 0.0f, 2.0f);
		NuklearUI::LayoutRowDynamic(20, 1);

#define E(MemberName) \
		{ \
			NuklearUI::CheckboxLabel cb(#MemberName, MemberName); \
			if (cb.isOpen()) { \
				MemberName = cb.isActive(); \
			} \
		} \
// ----
		E(_depthTest)
		E(_showMesh)
		E(_showEnvironment)
		E(_showCurrentPose)
		E(_showIKPose)
		E(_showGroundRayCast)
		E(_showAnkleRayCast)
		E(_showAnkle2ToeRayCast)
		E(_showToeRayCast)
		E(_showToeAdjustRayCast)
#undef E
	}

private:

	void _onDrawGpuSkinningByDefault(Request& req) {
		mat4f projection = mat4f::s_perspective(60.0f, req.aspect, 0.01f, 10.f);
		mat4f view       = mat4f::s_lookAt(vec3f(0, 3, 7), vec3f(0, 3, 0), vec3f::s_up());
		_onDrawGpuSkinning(projection, view);

#if 0
		mat4f model = mat4f::s_transform(_gpuAnimInfo.model);
		mat4f mvp = projection * view * model;
		glDisable(GL_DEPTH_TEST);
		if (req.mShowRestPose) {
			_restPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kRed);
		}

		if (req.mShowCurrentPose) {
			_currentPoseVisual->uploadToGpu();
			_currentPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kBlue);
		}
		if (req.mShowBindPose) {
			_bindPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kGreen);
		}
		glEnable(GL_DEPTH_TEST);
#endif
	}

	void _onDrawGpuSkinning(const mat4f& projection, const mat4f& view) {
		mat4f model      = mat4f::s_transform(_gpuAnimInfo.model);

		// bind shader
		_skinnedShader->bind();
		{
			{ // bind uniforms
				Uniform<mat4f>::set(_skinnedShader->findUniformByName("model"), model);
				Uniform<mat4f>::set(_skinnedShader->findUniformByName("view"), view);
				Uniform<mat4f>::set(_skinnedShader->findUniformByName("projection"), projection);
				Uniform<mat4f>::set(_skinnedShader->findUniformByName("invBindPose"), _skeleton.invBindPose());
				Uniform<mat4f>::set(_skinnedShader->findUniformByName("pose"), _gpuAnimInfo.posePalette);
				Uniform<vec3f>::set(_skinnedShader->findUniformByName("light"), vec3f::s_one());
				_texture->set(_skinnedShader->findUniformByName("tex0"), 0);
			}

			for (int i = 0; i < _gpuMeshes.size(); ++i) {
				_drawMesh(_gpuMeshes[i], _gpuAttribLoc);
			}
			_texture->unset(0);
		}
		_skinnedShader->unbind();
	}

	void _onDrawStaticMesh(const mat4f& projection, const mat4f& view, Span<Mesh> meshes, SPtr<Texture> tex) {
		// bind shader
		_staticShader->bind();
		{
			{ // bind uniforms
				Uniform<mat4>::set(_staticShader->findUniformByName("model"), mat4::s_identity());
				Uniform<mat4>::set(_staticShader->findUniformByName("view"), view);
				Uniform<mat4>::set(_staticShader->findUniformByName("projection"), projection);
				Uniform<vec3f>::set(_staticShader->findUniformByName("light"), vec3f::s_one());
				tex->set(_staticShader->findUniformByName("tex0"), 0);
			}
			for (auto& mesh : meshes) {
				_drawMesh(mesh, _cpuAttribLoc);
			}
		}
		tex->unset(0);
		_staticShader->unbind();
	}

	void _populatePosePalette() {
		_cpuAnimInfo.animatedPose.getMatrixPalette(_cpuAnimInfo.posePalette);
		_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);
	}

	void _drawMesh(Mesh& mesh, const AnimationAttribLocation& attribLoc) {
		mesh.bind(attribLoc.pos, attribLoc.normal, attribLoc.uv, attribLoc.weights, attribLoc.joints);
		mesh.draw();
		mesh.unbind(attribLoc.pos, attribLoc.normal, attribLoc.uv, attribLoc.weights, attribLoc.joints);
	}

	void _loadExampleAsset() {
		constexpr const char* kGltfFileName		= "Assets/Mesh/Woman.gltf";
		constexpr const char* kTextureFileName  = "Assets/Textures/Woman.png";

		GLTFInfo info;
		GLTFLoader::s_readFile(info, kGltfFileName);

		_skeleton.create(info);
		_clips.reserve(info.animationClips.size());
		_clips = std::move(info.animationClips);
		_clipNames = std::move(info.animationClipNames);

		_cpuMeshes.reserve(info.skinMeshes.size());
		_cpuMeshes.appendRange(info.skinMeshes);

		_gpuMeshes.reserve(info.skinMeshes.size());
		_gpuMeshes.appendRange(info.skinMeshes);

		_texture = new Texture(kTextureFileName);
	}

	void _loadExampleAsset_AlignFeetOnTheGround() {
		constexpr const char* kGltfFileName = "Assets/Mesh/IKCourse.gltf";
		constexpr const char* kTextureFileName = "Assets/Textures/uvChecker.png";

		GLTFInfo info;
		GLTFLoader::s_readFile(info, kGltfFileName);

		_ikCourse = info.staticMeshes;
		_triangles = IntersectionsUtil::meshesToTriangles(_ikCourse);
		_ikCourseTexture = new Texture(kTextureFileName);
	}

	void _createExampleShader() {
		_staticShader = new Shader (
			"Assets/Shaders/static.vert",
			"Assets/Shaders/lit.frag"
		);
		_skinnedShader = new Shader (
			"Assets/Shaders/skinned.vert",
			"Assets/Shaders/lit.frag"
		);
		_cpuAttribLoc.setByStaticShader(_staticShader);
		_gpuAttribLoc.setBySkinnedShader(_skinnedShader);
	}

	void _defaultSetAnimInfo() {
		_cpuAnimInfo.playback = 0.f;
		_cpuAnimInfo.animatedPose = _skeleton.restPose();

		_gpuAnimInfo.playback = 0.f;
		_gpuAnimInfo.animatedPose = _skeleton.restPose();
		_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);

		_cpuAnimInfo.additivePose = _skeleton.restPose();
		_gpuAnimInfo.additivePose = _skeleton.restPose();
	}

	void _defaultSelectClip() {
		SGE_ASSERT(_clips.size() > 0);
		constexpr const char* kClipName = "Walking";
		for (int i = 0; i < _clips.size(); ++i) {
			if (_clips[i].name() == kClipName) {
				_currentClip = i;
				_cpuAnimInfo.clip = i;
				_gpuAnimInfo.clip = i;
				break;
			}
		}
	}

	void _defaultSetAdditiveBasePose() {
		constexpr const char* kClipName = "Lean_Left";
		for (int i = 0; i < _clips.size(); ++i) {
			if (_clips[i].name() == kClipName) {
				_cpuAnimInfo.additiveClip = i;
				_gpuAnimInfo.additiveClip = i;
				break;
			}
		}

		{
			auto& clip = _clips[_cpuAnimInfo.additiveClip];
			_cpuAnimInfo.additiveBasePose = Blending::makeAdditiveBasePose(_skeleton, clip);
			clip.setIsLoop(false);
		}
		{
			auto& clip = _clips[_gpuAnimInfo.additiveClip];
			_gpuAnimInfo.additiveBasePose = Blending::makeAdditiveBasePose(_skeleton, clip);
			clip.setIsLoop(false);
		}
	}

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

	Skeleton									_skeleton;
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

	bool										_showIKPose;
	bool										_showCurrentPose;
	bool										_depthTest;
	bool										_showMesh;
	bool										_showEnvironment;
	bool										_showGroundRayCast;
	bool										_showAnkleRayCast;
	bool										_showAnkle2ToeRayCast;
	bool										_showToeRayCast;
	bool										_showToeAdjustRayCast;

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
};



class GameAnimeProgMainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	using Type = MySampleType;

	static constexpr const int kSampleCount = enumInt(Type::_END);

	void update(float dt) {
		_sampleRequest.dt = dt;

		if (_vertexArrayObject == 0)
			return;

		if (_sampleContext)
			_sampleContext->update(_sampleRequest);
	}

	void render() {
		if (_vertexArrayObject == 0)
			return;

		_beginRender();

		if (_sampleContext) {
			_sampleContext->render(_sampleRequest);

			if (_bShowUI) {
				_sampleContext->drawUI(_sampleRequest);
			}
		}

		if (_bShowUI) {
			_drawUI();
		}
		
		_endRender();
	}

protected:
	virtual void onCreate(CreateDesc& desc) override  {
		Base::onCreate(desc);

		{ // create opengl render context
			const HDC dc = hdc();
			PIXELFORMATDESCRIPTOR pfd;
			pfd 				= {};
			pfd.nSize 			= sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion 		= 1;
			pfd.dwFlags 		= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.iPixelType 		= PFD_TYPE_RGBA;
			pfd.cColorBits 		= 24;
			pfd.cDepthBits 		= 32;
			pfd.cStencilBits 	= 8;
			pfd.iLayerType 		= PFD_MAIN_PLANE;
			int pixelFormat 	= ChoosePixelFormat(dc, &pfd);
			SetPixelFormat(dc, pixelFormat, &pfd);

			// legacy render context
			HGLRC tempRC = wglCreateContext(dc);
			wglMakeCurrent(dc, tempRC);

			// legacy render context just for get function pointer of 'wglCreateContextAttribsARB'
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
			wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			const int attribList[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_FLAGS_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB,
				WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};

			// modern render context
			HGLRC hglrc = wglCreateContextAttribsARB(dc, 0, attribList);

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tempRC);
			wglMakeCurrent(dc, hglrc);

			// use 'glad' to load all opengl core function
			if (!gladLoadGL()) {
				throw SGE_ERROR("Could not initialize GLAD\n");
			}

			SGE_LOG("OpenGL Version: {}.{} loaded", GLVersion.major, GLVersion.minor);
		}

		{ // vsynch: https://www.khronos.org/opengl/wiki/Swap_Interval
			PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
			bool isSwapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

			_vsynch = 0;
			if (isSwapControlSupported) {
				PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
				PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

				if (wglSwapIntervalEXT(1)) {
					SGE_LOG("Enabled vsynch\n");
					_vsynch = wglGetSwapIntervalEXT();
				}
				else {
					SGE_LOG("Could not enable vsynch\n");
				}
			}
			else { // !swapControlSupported
				SGE_LOG("WGL_EXT_swap_control not supported\n");
			}
		}

		// Setup some OpenGL required state
		glGenVertexArrays(1, &_vertexArrayObject);
		glBindVertexArray(_vertexArrayObject);

		// set-up camera
		_camera.setPos(0, 10, 10);
		_camera.setAim(0, 0, 0);

		// create Nuklear
		NuklearUI::createContext();

		// create sample
		_createSample();
	}

	virtual void onCloseButton() override {
		if (_vertexArrayObject != 0) {
			HDC dc		= hdc();
			HGLRC hglrc = wglGetCurrentContext();

			// delete VAO
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &_vertexArrayObject);
			_vertexArrayObject = 0;

			// deleate Nuklear
			NuklearUI::destroyContext();

			// delete render context
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(_hwnd, dc);
		}

		NativeUIApp::current()->quit(0);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		using Button = UIMouseEventButton;

		if (NuklearUI::onUIMouseEvent(ev)) {
			return;
		}

		if (ev.isDragging()) {
			switch (ev.pressedButtons) {
			case Button::Middle: {
				auto d = ev.deltaPos * 0.05f;
				_camera.move(d.x, d.y, 0);
			}break;
			case Button::Right: {
				auto d = ev.deltaPos * 0.01f;
				_camera.orbit(d.x, d.y);
			}break;
			}
		}
		if (ev.isScroll()) {
			auto d = ev.scroll * -0.005f;
			_camera.dolly(d.y);
		}
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		using KeyCode = UIKeyboardEventKeyCode;

		NuklearUI::onUIKeyboardEvent(ev);

		if (ev.isDown(KeyCode::Escape)) {
			_exitSample();
		}
		if (ev.isDown(KeyCode::Tab)) {
			if (ev.IsModifierKeyDown(KeyCode::Shift)) {
				_switchToLastSample(); 
			} else {
				_switchToNextSample();
			}
		}

		if (ev.isDown(KeyCode::Space)) {
			_bShowSettingWindow = !_bShowSettingWindow;
		}

		if (ev.IsModifierKeyDown(KeyCode::Ctrl)) {
			static float const kValueStep = 0.1f;
			auto& scaleFactor = NuklearUI::scaleFactor;
			if (ev.isDown(KeyCode::Equals)) {
				scaleFactor += kValueStep;
			}
			else if (ev.isDown(KeyCode::Hyphen)) {
				scaleFactor = Math::max(1.0f, scaleFactor - kValueStep);
			}
			else if (ev.isDown(KeyCode::R)) {
				scaleFactor = 1.0f;
			}
			else if (ev.isDown(KeyCode::H)) {
				_bShowUI = !_bShowUI;
			}
		}
	}

private:
	void _beginRender() {

		_camera.setViewport(clientRect());

		const Vec2f& clientSize	= clientRect().size;
		_aspect = clientSize.x / clientSize.y;

		glViewport(0, 0, static_cast<GLsizei>(clientSize.x), static_cast<GLsizei>(clientSize.y));
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPointSize(5.0f * NuklearUI::scaleFactor);
		glLineWidth(1.5f * NuklearUI::scaleFactor);

		glBindVertexArray(_vertexArrayObject);
		glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_uploadWireFrameMode();
	}

	void _endRender() {
		SwapBuffers(hdc());
		if (_vsynch != 0) {
			glFinish();
		}
	}

	void _drawSampleSelectorWindow() {
		const Vec2f& clientSize = clientRect().size;

		TempString title;
		if (_type != Type::None) {
			FmtTo(title, "{}. {}", enumInt(_type), _type);
			Rect2f xywh { 0, 0, NuklearUI::Util::toNKSize(clientSize.x), 0 };
			NuklearUI::Window window(xywh, title);
			if (!window.isOpen()) {
				_exitSample();
			}
			return;
		}

		static Vec2f pos		{ 50.f,  20.f };
		static Vec2f sItemSize	{ 250.f, 25.f };
		static const int sItemCount = kSampleCount - 1;
		Rect2f xywh = {
			pos.x,
			pos.y,
			sItemSize.x + NuklearUI::LayoutRowStatic::kItemWMargin,
			sItemCount * (sItemSize.y + NuklearUI::LayoutRowStatic::kItemHMargin)
		};
		NuklearUI::Window window(xywh, "My Sample Selector", NuklearUI::Window::kMovableStyle);
		if (!window.isOpen()) {
			return;
		}
		NuklearUI::LayoutRowStatic layout(sItemSize.y, sItemSize.x);
		for (auto flag = Type::None + 1; flag != Type::_END; flag += 1) {
			title.clear();
			FmtTo(title, "{}. {}", enumInt(flag), flag);
			NuklearUI::ButtonLabel btn(title);
			if (btn.isOpen()) {
				_switchSample(flag);
			}
		}
	}
	
	void _drawSettingWindow() {
		if (!_bShowSettingWindow) return;
		auto& scaleFactor = NuklearUI::scaleFactor;
		// Rect2f xywh = { 0, clientSize.y - h * scaleFactor, clientSize.x / scaleFactor, 60.f }; // why ???
		static constexpr Rect2f xywh { 0, 0, 220.f, 60.f };
		NuklearUI::Window window(xywh, "ScaleFactor:");
		if (!window.isOpen()) {
			_bShowSettingWindow = false;
			return;
		}
		auto windowSize = NuklearUI::windowGetSize();
		NuklearUI::LayoutRowStatic layout(0, windowSize.x - 25.f);
		NuklearUI::SliderFloat(&scaleFactor, 1.0f, 2.0f);
	}

	void _drawUI() {
		{
			auto noWireFrame = makeScopedValue(&_bWireFrame, false);
			_uploadWireFrameMode();

			_drawSampleSelectorWindow();
			_drawSettingWindow();

			NuklearUI::render(clientRect().size);
		}
		_uploadWireFrameMode();
	}

	void _uploadWireFrameMode() {
		glPolygonMode(GL_FRONT_AND_BACK, _bWireFrame ? GL_LINE : GL_FILL);
	}

	void _switchSample(Type newType) {
		if (_type == newType)
			return;

		_destroySample();

		_sampleRequest.reset();
		_type = newType;

		_createSample();
	}

	void _exitSample() {
		_switchSample(Type::None);
	}

	void _switchToLastSample() {
		int sampleIndex = (enumInt(_type) - 1 + kSampleCount) % kSampleCount;
		_switchSample(static_cast<Type>(sampleIndex));
	}

	void _switchToNextSample() {
		int sampleIndex = (enumInt(_type) + 1) % kSampleCount;
		_switchSample(static_cast<Type>(sampleIndex));
	}

	void _destroySample() {
		if (_sampleContext) {
			_sampleContext.release();
			_sampleContext = nullptr;
		}
	}

	void _createSample() {
		_destroySample();
		_sampleContext = make_unique<SampleContext>();
		_sampleContext->create(_sampleRequest);
	}

	GLuint	_vertexArrayObject	= 0;
	int		_vsynch				= 0;

	Type	_type				= SampleContext::kStartUpDefaultType;
	float	_aspect				= 0;
	bool	_bWireFrame			= false;

	bool	_bShowUI			= true;
	bool	_bShowSettingWindow	= false;

	Math::Camera3f	_camera;

	SampleRequest		_sampleRequest { _type, _aspect, _bWireFrame, _camera };
	UPtr<SampleContext>	_sampleContext;
};

class GameAnimeProgApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../examples/Test102");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1500, 1080 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Game Anime Prog Window");
		}
	}

	virtual void onUpdate(float dt) override {
		_mainWin.update(dt);
		_mainWin.render();
		NuklearUI::UIInput input;
	}

private:
	GameAnimeProgMainWin _mainWin;
};

} // namespace


int main(int argc, const char** argv) {
	sge::GameAnimeProgApp app;
	sge::GameAnimeProgApp::CreateDesc desc;
	app.run(desc);

	return 0;
}
