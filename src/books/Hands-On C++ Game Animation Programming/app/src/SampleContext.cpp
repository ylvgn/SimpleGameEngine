#include "SampleContext.h"

namespace sge {

static float SampleContext_sUIHeaderHeight() {
	return (NuklearUI::Window::kTitleHeight + 5) * NuklearUI::g_scaleFactor;
}

void SampleContext::test_None_onCreate(Request& req) {
	// just for test
	_staticShader = new Shader(
		"../Test101/LocalTemp/Imported/Assets/Shaders/test.shader/gl3/pass0/vs_330.spv.glsl",
		"../Test101/LocalTemp/Imported/Assets/Shaders/test.shader/gl3/pass0/ps_330.spv.glsl"
	);
	_staticShader->dumpUniformBlocks();
	_staticShader->dumpActiveAttrib();
}
void SampleContext::test__END_onCreate(Request& req) {}
void SampleContext::test_LitTexture_onCreate(Request& req) {
	_debugPoints	= new DebugDraw();
	_debugLines		= new DebugDraw();

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
		vec3f( 1, -1, 0),
		vec3f( 1,  1, 0),
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
		lines[j++] = positions[indices[i+1]];

		lines[j++] = positions[indices[i+1]];
		lines[j++] = positions[indices[i+2]];

		lines[j++] = positions[indices[i+2]];
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
}
void SampleContext::test_AnimationScalarTrack_onCreate(Request& req) {
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
				FrameUtil::createFrame(0.25f,       0,          0,           0         ),
				FrameUtil::createFrame(0.3833333f, -10.11282f,  0.5499259f, -10.11282f ),
				FrameUtil::createFrame(0.5f,        25.82528f,  1,           25.82528f ),
				FrameUtil::createFrame(0.6333333f,  7.925411f,  0.4500741f,  7.925411f ),
				FrameUtil::createFrame(0.75f,       0,          0,           0         )
			);
			_scalarTracks.push_back(cubicTrack);
			_scalarTracksIsLoop.push_back(false);

			_scalarTracks.push_back(cubicTrack);
			_scalarTracksIsLoop.push_back(true);
		}

		{
			const auto cubicTrack = TrackUtil::createScalarTrack(
				Interpolation::Cubic, 5,
				FrameUtil::createFrame(0.25f,      0,         0,           0        ),
				FrameUtil::createFrame(0.3833333f, 13.25147f, 0.5499259f, -10.11282f),
				FrameUtil::createFrame(0.5f,       10.2405f,  1,          -5.545671f),
				FrameUtil::createFrame(0.6333333f, 7.925411f, 0.4500741f, -11.40672f),
				FrameUtil::createFrame(0.75f,      0,         0,           0        )
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
				float thisJNorm = (j-1) / 149.0f;
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
void SampleContext::test_BezierAndHermiteCurve_onCreate(Request& req) {
	_debugPoints = new DebugDraw();
	_debugLines  = new DebugDraw();

	constexpr const int   kSampleCount = 200;
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
void SampleContext::test_AnimationClip_onCreate(Request& req) {
	_loadExampleAsset();
	_defaultSetPoseDebugDraw();
	_defaultSetCamera(req, { 0,4,7 }, { 0,4,0 });

	_currentClip  = 0;
	_playbackTime = 0.f;
	_currentPose  = _skeleton->restPose();

	// play 'Walking' animation clip
	for (int i = 0; i < _clips.size(); ++i) {
		if (_clips[i].name() == "Walking") {
			_currentClip = i;
			break;
		}
	}

	req.bShowRestPose		= true;
	req.bShowCurrentPose	= true;
	req.bShowBindPose		= true;
}
void SampleContext::test_MeshSkinning_onCreate(Request& req) {
	_texture = new Texture("Assets/Textures/Woman.png");
	GLTFInfo info;
	GLTFLoader::s_readFile(info, "Assets/Mesh/Woman.gltf");

#if 1 // test optimize clip
	_fastClips.resize(info.animationClips.size());
	for (int i = 0; i < info.animationClips.size(); ++i) {
		_fastClips[i] = ClipUtil::optimizeClip(info.animationClips[i]);
	}
#endif
	_skeleton = new Skeleton();
	_skeleton->create(info);
	_clips = std::move(info.animationClips);
	RearrangeBones::s_rearrange(_skeleton, info.skinMeshes, _fastClips);

	const auto& restPose = _skeleton->restPose();

	{ // test cpu skinning
		_staticShader = new Shader(
			"Assets/Shaders/static.vert",
			"Assets/Shaders/lit.frag"
		);
		_cpuMeshes.appendRange(info.skinMeshes);
		_cpuAnimInfo.animatedPose = restPose;
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
		_gpuAnimInfo.animatedPose = restPose;
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

	_defaultSetCamera(req, { 0,5,7 }, { 0,3,0 });
}
void SampleContext::test_AnimationBlending_onCreate(Request& req) {
	_loadExampleAsset();
	_loadExampleShader();
	_defaultSetAnimInfo();
	_defaultSetCamera(req, { 0,4,7 }, { 0,3,0 });
	_defaultSetPoseDebugDraw();
	_blendAnimA.animatedPose = _skeleton->restPose();
	_blendAnimA.playback = 0.f;

	_blendAnimB.animatedPose = _skeleton->restPose();
	_blendAnimB.playback = 0.f;

	_elapsedBlendTime	= 0.f;
	_isInvertBlend		= false;

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
void SampleContext::test_Crossfading_onCreate(Request& req) {
	_loadExampleAsset();
	_loadExampleShader();
	_defaultSetAnimInfo();
	_defaultSetCamera(req, { 0,4,6 }, { 0,3,0 });
	_defaultSetPoseDebugDraw();

	CrossFadeController* fadeController = CrossFadeController::instance();
	if (fadeController == nullptr) {
		fadeController = new CrossFadeController();
	}
	fadeController->setSkeleton(_skeleton);
	fadeController->play(&_clips[0]);

	_fadeTimer = 2.0f;
}
void SampleContext::test_AdditiveBlending_onCreate(Request& req) {
	_loadExampleAsset();
	_loadExampleShader();
	_defaultSetAnimInfo();
	_defaultSelectClip();
	_defaultSetAdditiveBasePose();
	_defaultSetCamera(req, { 0,5,6 }, { 0,3,0 });
	_defaultSetPoseDebugDraw();

	_additiveTime		= 0.f;
	_additiveDirection	= 1.f;
}
void SampleContext::test_CCD_onCreate(Request& req) {
	_ccdSolver = make_unique< IKSolverExample<CCDSolver> >();
	_ccdSolver->create(req);
}
void SampleContext::test_FABRIK_onCreate(Request& req) {
	_fabrikSolver = make_unique< IKSolverExample<FABRIKSolver> >();
	_fabrikSolver->create(req);
}
void SampleContext::test_CCD_BallSocketConstraint_onCreate(Request& req) {
	_ccdBallSocketConstraint = BallSocketConstraintExample<CCDSolver>::instance();
	_ccdBallSocketConstraint->create(req);
}
void SampleContext::test_FABRIK_BallSocketConstraint_onCreate(Request& req) {
	_fabrikBallSocketConstraint = BallSocketConstraintExample<FABRIKSolver>::instance();
	_fabrikBallSocketConstraint->create(req);
}
void SampleContext::test_CCD_HingeSocketConstraint_onCreate(Request& req) {
	_ccdHingeSocketConstraint = HingeSocketConstraintExample<CCDSolver>::instance();
	_ccdHingeSocketConstraint->create(req);
}
void SampleContext::test_FABRIK_HingeSocketConstraint_onCreate(Request& req) {
	_fabrikHingeSocketConstraint = HingeSocketConstraintExample<FABRIKSolver>::instance();
	_fabrikHingeSocketConstraint->create(req);
}
void SampleContext::test_RayCastTriangle_onCreate(Request& req) {
	_loadExampleAsset_AlignFeetOnTheGround();
	_loadExampleShader();
	_defaultSetCamera(req, { 0,7,20 }, vec3f::s_zero());

	_groundRayDebugDraw = new DebugDrawPL();
	_debugPoints		= new DebugDraw();
}
void SampleContext::test_AlignFeetOnTheGround_onCreate(Request& req) {
	_loadExampleAsset();
	_loadExampleAsset_AlignFeetOnTheGround();
	_loadExampleShader();
	_defaultSetAnimInfo();
	_defaultSelectClip();
	_defaultSetCamera(req);
	_defaultSetPoseDebugDraw();

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

	_depthTest					= false;

	req.bWireFrame				= false;
	req.bShowIKPose				= false;
	req.bShowCurrentPose		= false;
	req.bShowEnvironment		= true;
	req.bShowModelMesh			= true;
	req.bShowGroundRayCast		= false;
	req.bShowAnkleRayCast		= false;
	req.bShowAnkle2ToeRayCast	= false;
	req.bShowToeRayCast			= false;
	req.bShowToeAdjustRayCast	= false;
	req.bIsCameraFollow			= true;

	_walkingTime				= 0.f;
	_lastModelY					= 0.f;
	_sinkIntoGround				= 0.15f;
	_toeLength					= 0.3f;

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
}
void SampleContext::test_DualQuaterionMeshSkinning_onCreate(Request& req) {
	_loadExampleAsset_DualQuaterionMeshSkinning();
	_loadExampleShader();
	_defaultSetAnimInfo();
	_defaultSelectClip();
	_defaultSetCamera(req, { 0,0,15 }, { 0,0,0 });
	//_gpuAnimInfo.model.position.set(0,0,0);
}
void SampleContext::test_InstancedCrowds_onCreate(Request& req) {
	_loadExampleShader();
	_loadExampleAsset();
	_defaultSetAnimInfo();
	_defaultSetCamera(req, { 0,15,40 }, { 0,3,0 });
	_loadExampleAsset_InstancedCrowds();
}

void SampleContext::test_None_onUpdate(Request& req) {}
void SampleContext::test__END_onUpdate(Request& req) {}
void SampleContext::test_LitTexture_onUpdate(Request& req) {
	auto& dt = req.dt;
	_testRotation += dt * 45.0f;
	while (_testRotation > 360.0f) {
		_testRotation -= 360.0f;
	}
}
void SampleContext::test_AnimationScalarTrack_onUpdate(Request& req) {}
void SampleContext::test_BezierAndHermiteCurve_onUpdate(Request& req) {}
void SampleContext::test_AnimationClip_onUpdate(Request& req) {
	constexpr float kClipMaxLoopTime = 4.f;
	static float s_clipLoopingTime = 0.f;
	auto& dt = req.dt;

	const Clip& clip = _clips[_currentClip];
	_playbackTime	 = clip.sample(_currentPose, _playbackTime + dt);

	s_clipLoopingTime += dt;
	if (s_clipLoopingTime >= kClipMaxLoopTime) {
		s_clipLoopingTime = 0.f;
		_currentClip  = (_currentClip + 1) % _clips.size();
		_playbackTime = 0.f;
		_currentPose  = _skeleton->restPose();
	}
	_currentPoseVisual->lineFromPose(_currentPose);
}
void SampleContext::test_MeshSkinning_onUpdate(Request& req) {
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
		const auto& invBindPose = _skeleton->invBindPose();
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
		const auto& invBindPose = _skeleton->invBindPose();
		for (int i = 0; i < _gpuAnimInfo.posePalette.size(); ++i) {
			_gpuAnimInfo.posePalette[i] = _gpuAnimInfo.posePalette[i] * invBindPose[i];
		}
#endif
	}
}
void SampleContext::test_AnimationBlending_onUpdate(Request& req) {
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
		_gpuAnimInfo.animatedPose = _skeleton->restPose();
	}
}
void SampleContext::test_Crossfading_onUpdate(Request& req) {
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
void SampleContext::test_AdditiveBlending_onUpdate(Request& req) {
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

	_currentPoseVisual->lineFromPose(_gpuAnimInfo.animatedPose);
}
void SampleContext::test_CCD_onUpdate(Request& req) {
	_ccdSolver->update(req);
}
void SampleContext::test_FABRIK_onUpdate(Request& req) {
	_fabrikSolver->update(req);
}
void SampleContext::test_CCD_BallSocketConstraint_onUpdate(Request& req) {
	_ccdBallSocketConstraint->update(req);
}
void SampleContext::test_FABRIK_BallSocketConstraint_onUpdate(Request& req) {
	_fabrikBallSocketConstraint->update(req);
}
void SampleContext::test_CCD_HingeSocketConstraint_onUpdate(Request& req) {
	_ccdHingeSocketConstraint->update(req);
}
void SampleContext::test_FABRIK_HingeSocketConstraint_onUpdate(Request& req) {
	_fabrikHingeSocketConstraint->update(req);
}
void SampleContext::test_RayCastTriangle_onUpdate(Request& req) {
	_groundRayDebugDraw->clear();
	_debugPoints->clear();

	vec3f hitPoint;
	Ray ray(vec3f(0, 3, 0),
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
void SampleContext::test_AlignFeetOnTheGround_onUpdate(Request& req) {
	auto& dt = req.dt;

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

	static constexpr const float kWalkingSpeed		= 0.3f;
	static constexpr const float kWalkingTimeLength = 6.f;

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

	if (leftAnkleToCurrentToe.dot(leftAnkleToDesiredToe) > kEpsilon) { // degrees range (-90, 90)
		Transform ankleLocalTran = curAnimatedPose.getLocalTransform(_leftLeg->ankle());
		const quat4f& leftAnkleWorldRot = leftAnkleWorld.rotation;
	#if 1 // calc in world space
		quat4f currentToDesired	= quat::s_fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
		quat4f worldRotated		= leftAnkleWorldRot * currentToDesired;
		quat4f localRotated		= worldRotated * leftAnkleWorldRot.inverse();
	#else // calc in local space
		quat4f leftAnkleWorldRotInv	= leftAnkleWorldRot.inverse();
		leftAnkleToCurrentToe		= leftAnkleWorldRotInv * leftAnkleToCurrentToe;
		leftAnkleToDesiredToe		= leftAnkleWorldRotInv * leftAnkleToDesiredToe;
		quat4f localRotated			= quat4f::s_fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
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
		quat4f rightAnkleWorldRotInv = rightAnkleWorldRot.inverse();
		rightAnkleToCurrentToe		 = rightAnkleWorldRotInv * rightAnkleToCurrentToe;
		rightAnkleToDesiredToe		 = rightAnkleWorldRotInv * rightAnkleToDesiredToe;
		quat4f localRotated			 = quat4f::s_fromTo(rightAnkleToCurrentToe, rightAnkleToDesiredToe);
	#endif
		ankleLocalTran.rotation = localRotated * ankleLocalTran.rotation;
		curAnimatedPose.setLocalTransform(_rightLeg->ankle(), ankleLocalTran);
	}
#endif

	// Update the matrix palette for skinning
	_populatePosePalette();
}
void SampleContext::test_DualQuaterionMeshSkinning_onUpdate(Request& req) {
	_gpuAnimInfo.animatedSample(_clips, req.dt);
	_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);
	_gpuAnimInfo.animatedPose.getDualQuaternionPalette(_gpuAnimInfo.dqPosePalette);
}
void SampleContext::test_InstancedCrowds_onUpdate(Request& req) {
	int i = 0;
	size_t clipCount = _clips.size();
	for (auto& c : _crowds) {
		int clipIndex = i % clipCount;
		c->update(req.dt, _clips[clipIndex], _crowdAnimTextures[clipIndex]->texSize());
		++i;
	}
}

void SampleContext::test_None_onRender(Request& req) {}
void SampleContext::test__END_onRender(Request& req) {}
void SampleContext::test_LitTexture_onRender(Request& req) {
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());
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
void SampleContext::test_AnimationScalarTrack_onRender(Request& req) {
	auto clientSize = req.camera.viewport().size;
	float aspect = clientSize.y != 0 ? clientSize.x / clientSize.y : 0;
#if 1 // ortho camera
	req.camera.setProjectionType(Math::CameraProjectionType::Orthogrphic);
	req.camera.setViewport(Rect2f{ 0, 0, aspect * 22.f, 22.f });
	req.camera.setNearClip(0.01f);
	req.camera.setFarClip(5.0f);
	req.camera.setPos(0, 0, 2.5f);
	req.camera.setAim(0, 0, 0);
	req.camera.setUp (0, 1, 0);

	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());
#else
	float l = 0;
	float t = 22.f;
	float b = 0;
	float r = aspect * t;
	float n = 0.01f;
	float f = 5.f;
	mat4f projection = mat4f::s_ortho(l, r, b, t, n, f);
	mat4f view = mat4f::s_lookAt(vec3f(0, 0, (n + f) / 2), vec3f::s_zero(), vec3f::s_up());
#endif

	mat4f mvp = projection * view /** mat4f::s_identity()*/;

	_referenceLines->draw(DebugDrawMode::Lines, mvp, DebugDraw::kYellow);
	_scalarTrackLines->draw(DebugDrawMode::Lines, mvp, DebugDraw::kGreen);
	_handlePoints->draw(DebugDrawMode::Points, mvp, DebugDraw::kBlue);
	_handleLines->draw(DebugDrawMode::Lines, mvp, DebugDraw::kPurple);
}
void SampleContext::test_BezierAndHermiteCurve_onRender(Request& req) {
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());
	mat4f mvp = projection * view * mat4f::s_identity();
	_debugLines->draw(DebugDrawMode::Lines, mvp);
	_debugPoints->draw(DebugDrawMode::Points, mvp, DebugDraw::kBlue);
}
void SampleContext::test_AnimationClip_onRender(Request& req) {
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());
	mat4f vp = projection * view;

	if (req.bShowRestPose) {
		mat4f mvp = vp * mat4f::s_translate(vec3f(-3, 0, 0));
		_restPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kRed);
	}
	if (req.bShowCurrentPose) {
		mat4f mvp = vp * mat4f::s_identity();
		_currentPoseVisual->uploadToGpu();
		_currentPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kBlue);
	}
	if (req.bShowBindPose) {
		mat4f mvp = vp * mat4f::s_translate(vec3f(3, 0, 0));
		_bindPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kGreen);
	}
}

void SampleContext::test_MeshSkinning_onRender(Request& req) {
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());

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

			for(auto& mesh : _cpuMeshes) {
				_cpuAttribLoc.bindAttribAndDraw(mesh);
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
				Uniform<mat4f>::set(_skinnedShader->findUniformByName("invBindPose"), _skeleton->invBindPose());
				Uniform<mat4f>::set(_skinnedShader->findUniformByName("pose"), _gpuAnimInfo.posePalette);
#endif
				_texture->set(_skinnedShader->findUniformByName("tex0"), 0);
			}
			for (auto& mesh : _gpuMeshes) {
				_gpuAttribLoc.bindAttribAndDraw(mesh);
			}
			_texture->unset(0);
		}
		_skinnedShader->unbind();
	}
}
void SampleContext::test_AnimationBlending_onRender(Request& req) {
	_onDrawGpuSkinningByDefault(req);
}
void SampleContext::test_Crossfading_onRender(Request& req) {
	_onDrawGpuSkinningByDefault(req);
}
void SampleContext::test_AdditiveBlending_onRender(Request& req) {
	_onDrawGpuSkinningByDefault(req);
}
void SampleContext::test_CCD_onRender(Request& req) {
	_ccdSolver->render(req);
}
void SampleContext::test_FABRIK_onRender(Request& req) {
	_fabrikSolver->render(req);
}
void SampleContext::test_CCD_BallSocketConstraint_onRender(Request& req) {
	_ccdBallSocketConstraint->render(req);
}
void SampleContext::test_FABRIK_BallSocketConstraint_onRender(Request& req) {
	_fabrikBallSocketConstraint->render(req);
}
void SampleContext::test_CCD_HingeSocketConstraint_onRender(Request& req) {
	_ccdHingeSocketConstraint->render(req);
}
void SampleContext::test_FABRIK_HingeSocketConstraint_onRender(Request& req) {
	_fabrikHingeSocketConstraint->render(req);
}
void SampleContext::test_RayCastTriangle_onRender(Request& req) {
	req.bWireFrame = true;
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());
	mat4f mvp = projection * view * mat4f::s_identity();
	_onDrawStaticMesh(req, _ikCourse, _ikCourseTexture);

	_debugPoints->uploadToGpu();
	_debugPoints->draw(DebugDrawMode::Points, mvp, DebugDraw::kGreen);
	_groundRayDebugDraw->draw(mvp);
}
void SampleContext::test_AlignFeetOnTheGround_onRender(Request& req) {
	auto& modelTran = _gpuAnimInfo.model;
	float modelPosX = modelTran.position.x;
	float modelPosZ = modelTran.position.z;
	mat4f model		= mat4f::s_transform(modelTran);

	if (req.bIsCameraFollow) {
		vec3f eye    = vec3f(modelPosX, 0, modelPosZ) + vec3f(0, 5, 10);
		vec3f target = vec3f(modelPosX, 0, modelPosZ) + vec3f(0, 3, 0);
		req.camera.setPos(eye.x, eye.y, eye.z);
		req.camera.setAim(target.x, target.y, target.z);
		req.camera.setUp(0,1,0);
	}

	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());

	mat4f vp	= projection * view;
	mat4f mvp	= vp * model;

	if (req.bShowModelMesh) {
		_onDrawGpuSkinning(req);
	}

	if (req.bShowEnvironment) {
		_onDrawStaticMesh(req, _ikCourse, _ikCourseTexture);
	}

	if (!_depthTest) glDisable(GL_DEPTH_TEST);
	{
		if (req.bShowCurrentPose) {
			_animatedPoseDebugDraw->draw(mvp, DebugDrawPL::Mask::Line);
		}
		if (req.bShowIKPose) {
			_leftLegDebugDraw->draw(vp, DebugDrawPL::Mask::Line);
			_rightLegDebugDraw->draw(vp, DebugDrawPL::Mask::Line);
		}
		if (req.bShowGroundRayCast) {
			_groundRayDebugDraw->draw(vp);
		}
		if (req.bShowAnkleRayCast) {
			_ankleRayCastHitDebugDraw->draw(vp);
			_ankleRayDebugDraw->draw(vp);
		}
		if (req.bShowAnkle2ToeRayCast) {
			_ankle2ToeRayCastDebugDraw->draw(vp);
		}
		if (req.bShowToeRayCast) {
			_toeRayCastHitDebugDraw->draw(vp);
			_toeRayDebugDraw->draw(vp);
		}
		if (req.bShowToeAdjustRayCast) {
			_ankleToCurrentToeDebugDraw->draw(vp);
			_ankleToDesiredToeDebugDraw->draw(vp);
		}
	}
	if (!_depthTest) glEnable(GL_DEPTH_TEST);
}
void SampleContext::test_DualQuaterionMeshSkinning_onRender(Request& req) {
	bool isDualSkinning = req.curSkinningMethod == 0;

	Shader* shader = isDualSkinning ? _dqSkinnedShader : _skinnedShader;

	mat4f model(mat4f::s_transform(_gpuAnimInfo.model));

	test_DualQuaterionMeshSkinning_onRender_Inner(req, shader, _gpuAttribLoc, model, isDualSkinning);
}
void SampleContext::test_InstancedCrowds_onRender(Request& req) {
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());

	_crowdSkinnedShader->bind();
	{
		{ // bind uniforms
			Uniform<mat4f>::set(_crowdSkinnedShader->findUniformByName("view"), view);
			Uniform<mat4f>::set(_crowdSkinnedShader->findUniformByName("projection"), projection);
			Uniform<mat4f>::set(_crowdSkinnedShader->findUniformByName("invBindPose"), _gpuAnimInfo.invBindPosePalette);
			Uniform<vec3f>::set(_crowdSkinnedShader->findUniformByName("light"), vec3f::s_one());
			_texture->set(_crowdSkinnedShader->findUniformByName("tex0"), 0);	
		}

		{
			size_t clipCount = _clips.size();
			int i = 0;
			for (auto& c : _crowds) {
				int clipIndex = i % clipCount;
				{ // bind instance-specific uniforms
					c->bindUniforms();
					_crowdAnimTextures[clipIndex]->set(_crowdSkinnedShader->findUniformByName("animTex"), 1);
				}

				// draw instanced
				for (int j = 0; j < _gpuMeshes.size(); ++j) {
					_gpuAttribLoc.bindAttribAndDrawInstanced(_gpuMeshes[j], c->getCrowdCount());
				}
				_crowdAnimTextures[clipIndex]->unset(1);
				++i;
			}
		}
		_texture->unset(0);
	}
	_crowdSkinnedShader->unbind();
}

void SampleContext::test_None_onDrawUI(Request& req) {
//	NuklearUI::demo();
}
void SampleContext::test__END_onDrawUI(Request& req) {}
void SampleContext::test_LitTexture_onDrawUI(Request& req) {}
void SampleContext::test_AnimationScalarTrack_onDrawUI(Request& req) {}
void SampleContext::test_BezierAndHermiteCurve_onDrawUI(Request& req) {}
void SampleContext::test_AnimationClip_onDrawUI(Request& req) {
	Rect2f xywh{ 5.0f, 5.0f + SampleContext_sUIHeaderHeight(), 300.0f, 135.0f };
	NuklearUI::Window window(xywh);

	static const float layout[] = { 75, 200 };
	NuklearUI::LayoutRow row(NK_STATIC, 25, 2, layout);
	NuklearUI::Label("Animation:");
	NuklearUI::Combo combo(_clipNames, _currentClip, 25, { 200, 200 });
	if (combo.selectedIndex() != _currentClip) {
		_currentPose = _skeleton->restPose();
		_currentClip = combo.selectedIndex();
	}

	NuklearUI::Label("Playback:");
	auto& clip      = _clips[_currentClip];
	float startTime = clip.getStartTime();
	float duration  = clip.getDuration();
	float progress  = (_playbackTime - startTime) / duration;
	size_t prog     = static_cast<size_t>(progress * 200.0f);
	NuklearUI::Progress(&prog, 200, NK_FIXED);

	NuklearUI::LayoutRowDynamic(20, 1);
#define E(NAME, VALUE) \
		{ \
			NuklearUI::CheckboxLabel cb(NAME, VALUE); \
			if (cb.isOpen()) { \
				VALUE = cb.isActive(); \
			} \
		} \
// ----
	E("Show Rest Pose", req.bShowRestPose)
	E("Show Current Pose", req.bShowCurrentPose)
	E("Show Bind Pose", req.bShowBindPose)
#undef E
}
void SampleContext::test_MeshSkinning_onDrawUI(Request& req) {}
void SampleContext::test_AnimationBlending_onDrawUI(Request& req) {}
void SampleContext::test_Crossfading_onDrawUI(Request& req) {}
void SampleContext::test_AdditiveBlending_onDrawUI(Request& req) {
	auto& currentClip		= _gpuAnimInfo.clip;
	auto& playbackTime		= _gpuAnimInfo.playback;
	auto& mAdditiveIndex	= _gpuAnimInfo.additiveClip;
	auto& mAdditiveBase		= _gpuAnimInfo.additiveBasePose;

	Rect2f xywh{ 5.0f, 5.0f + SampleContext_sUIHeaderHeight(), 300.0f, 225.0f };
	NuklearUI::Window window(xywh);
	static const float layout[] = { 75, 200 };
	NuklearUI::LayoutRow(NK_STATIC, 25, 2, layout);

	NuklearUI::Label("Animation:");
	NuklearUI::Combo combo(_clipNames, currentClip, 25, { 200, 200 });
	if (combo.selectedIndex() != currentClip) {
		_gpuAnimInfo.animatedPose = _skeleton->restPose();
		currentClip = combo.selectedIndex();
	}

	NuklearUI::Label("Playback:");
	auto& clip		= _clips[currentClip];
	float startTime = clip.getStartTime();
	float duration  = clip.getDuration();
	float progress  = (playbackTime - startTime) / duration;
	size_t prog     = static_cast<size_t>(progress * 200.0f);
	NuklearUI::Progress(&prog, 200, NK_FIXED);

	NuklearUI::Label("Additive:");
	NuklearUI::Combo combo2(_clipNames, mAdditiveIndex, 25, { 200, 200 });
	if (combo2.selectedIndex() != mAdditiveIndex) {
		_clips[mAdditiveIndex].setIsLoop(true);
		mAdditiveIndex = combo2.selectedIndex();
		mAdditiveBase  = Blending::makeAdditiveBasePose(*_skeleton.ptr(), _clips[mAdditiveIndex]);
		_clips[mAdditiveIndex].setIsLoop(true);
	}

	NuklearUI::Label("Strength:");
	NuklearUI::SliderFloat(&_additiveTime, 0.0f, 1.0f);

	NuklearUI::LayoutRowDynamic(20, 1);
#define E(NAME, VALUE) \
	{ \
		NuklearUI::CheckboxLabel cb(NAME, VALUE); \
		if (cb.isOpen()) { \
			VALUE = cb.isActive(); \
		} \
	} \
// ----
	E("Show Rest Pose", req.bShowRestPose)
	E("Show Bind Pose", req.bShowBindPose)
	E("Show Current Pose", req.bShowCurrentPose)
#undef E
}
void SampleContext::test_CCD_onDrawUI(Request& req) {}
void SampleContext::test_FABRIK_onDrawUI(Request& req) {}
void SampleContext::test_CCD_BallSocketConstraint_onDrawUI(Request& req) {}
void SampleContext::test_FABRIK_BallSocketConstraint_onDrawUI(Request& req) {}
void SampleContext::test_CCD_HingeSocketConstraint_onDrawUI(Request& req) {}
void SampleContext::test_FABRIK_HingeSocketConstraint_onDrawUI(Request& req) {}
void SampleContext::test_RayCastTriangle_onDrawUI(Request& req) {}
void SampleContext::test_AlignFeetOnTheGround_onDrawUI(Request& req) {
	Rect2f xywh { 5.0f, 5.0f + SampleContext_sUIHeaderHeight(), 300.0f, 330.0f };
	NuklearUI::Window window(xywh);
	static const float layout[] = { 75, 200 };
	NuklearUI::LayoutRow row(NK_STATIC, 25, 2, layout);

	NuklearUI::LayoutRowDynamic(20, 1);
	{
		NuklearUI::CheckboxLabel cb("Is Depth Test", _depthTest);
		if (cb.isOpen()) {
			_depthTest = cb.isActive();
		}
	}

#define E(NAME, VALUE) \
	{ \
		NuklearUI::CheckboxLabel cb(NAME, VALUE); \
		if (cb.isOpen()) { \
			VALUE = cb.isActive(); \
		} \
	} \
// ----
	E("Show Bind Pose", req.bShowBindPose)
	E("Show IK Pose", req.bShowIKPose)
	E("Show Current Pose", req.bShowCurrentPose)
	E("Show Model Mesh", req.bShowModelMesh)
	E("Show Environment", req.bShowEnvironment)
	E("Show Ground RayCast", req.bShowGroundRayCast)
	E("Show Ankle RayCast", req.bShowAnkleRayCast)
	E("Show Ankle To Toe RayCast", req.bShowAnkle2ToeRayCast)
	E("Show Toe RayCast", req.bShowToeRayCast)
	E("Show Toe Adjust RayCast", req.bShowToeAdjustRayCast)
	E("Is Camera Follow", req.bIsCameraFollow)
#undef E
}
void SampleContext::test_DualQuaterionMeshSkinning_onDrawUI(Request& req) {
	Rect2f xywh{ 5.0f, 5.0f + SampleContext_sUIHeaderHeight(), 300.0f, 60.0f };
	NuklearUI::Window window(xywh);

	static const float layout[] = { 75, 200 };
	NuklearUI::LayoutRow row(NK_STATIC, 25, 2, layout);
	NuklearUI::Label("Blend Mode:");
		
	static String comboNames[] {
		"Dual Quaternion",
		"Linear Blend",
	};
	NuklearUI::Combo combo(comboNames, req.curSkinningMethod, 25, { 200, 200 });
	if (combo.selectedIndex() != req.curSkinningMethod) {
		req.curSkinningMethod = combo.selectedIndex();
	}
}
void SampleContext::test_InstancedCrowds_onDrawUI(Request& req) {}

#if 0
#pragma mark ================= helper function ====================
#endif
void SampleContext::_onDrawGpuSkinningByDefault(Request& req) {
	_onDrawGpuSkinning(req);
}

void SampleContext::_onDrawGpuSkinning(Request& req) {
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());
	mat4f model = mat4f::s_transform(_gpuAnimInfo.model);
	mat4f mvp = projection * view * model;

	// bind shader
	_skinnedShader->bind();
	{
		{ // bind uniforms
			Uniform<mat4f>::set(_skinnedShader->findUniformByName("model"), model);
			Uniform<mat4f>::set(_skinnedShader->findUniformByName("view"), view);
			Uniform<mat4f>::set(_skinnedShader->findUniformByName("projection"), projection);
			Uniform<mat4f>::set(_skinnedShader->findUniformByName("invBindPose"), _gpuAnimInfo.invBindPosePalette);
			Uniform<mat4f>::set(_skinnedShader->findUniformByName("pose"), _gpuAnimInfo.posePalette);
			Uniform<vec3f>::set(_skinnedShader->findUniformByName("light"), vec3f::s_one());
			_texture->set(_skinnedShader->findUniformByName("tex0"), 0);
		}

		for (int i = 0; i < _gpuMeshes.size(); ++i) {
			_gpuAttribLoc.bindAttribAndDraw(_gpuMeshes[i]);
		}
		_texture->unset(0);
	}
	_skinnedShader->unbind();

	if (!_depthTest) glDisable(GL_DEPTH_TEST);
	if (req.bShowRestPose) {
		_restPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kRed);
	}
	if (req.bShowCurrentPose) {
		_currentPoseVisual->lineFromPose(_gpuAnimInfo.animatedPose);
		_currentPoseVisual->uploadToGpu();
		_currentPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kBlue);
	}
	if (req.bShowBindPose) {
		_bindPoseVisual->draw(DebugDrawMode::Lines, mvp, DebugDraw::kGreen);
	}
	if (!_depthTest) glEnable(GL_DEPTH_TEST);
}

void SampleContext::_onDrawStaticMesh(Request& req, Span<Mesh> meshes, Texture* tex) {
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());

	// bind shader
	_staticShader->bind();
	{
		{ // bind uniforms
			Uniform<mat4f>::set(_staticShader->findUniformByName("model"), mat4f::s_identity());
			Uniform<mat4f>::set(_staticShader->findUniformByName("view"), view);
			Uniform<mat4f>::set(_staticShader->findUniformByName("projection"), projection);
			Uniform<vec3f>::set(_staticShader->findUniformByName("light"), vec3f::s_one());
			tex->set(_staticShader->findUniformByName("tex0"), 0);
		}
		for (auto& mesh : meshes) {
			_cpuAttribLoc.bindAttribAndDraw(mesh);
		}
	}
	tex->unset(0);
	_staticShader->unbind();
}

void SampleContext::_populatePosePalette() {
	_cpuAnimInfo.animatedPose.getMatrixPalette(_cpuAnimInfo.posePalette);
	_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);
}

void SampleContext::test_DualQuaterionMeshSkinning_onRender_Inner(	Request& req,
																	Shader* shader,
																	const AnimationAttribLocation& attrLoc,
																	const mat4f& model,
																	bool isDualSkinning)
{
	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());

	// bind shader
	shader->bind();
	{
		{ // bind uniforms
			Uniform<mat4f>::set(shader->findUniformByName("model"), model);
			Uniform<mat4f>::set(shader->findUniformByName("view"), view);
			Uniform<mat4f>::set(shader->findUniformByName("projection"), projection);
			if (isDualSkinning) {
				Uniform<dual_quat>::set(shader->findUniformByName("invBindPose"), _gpuAnimInfo.dqInvBindPosePalette);
				Uniform<dual_quat>::set(shader->findUniformByName("pose"), _gpuAnimInfo.dqPosePalette);
			}
			else {
				Uniform<mat4f>::set(shader->findUniformByName("invBindPose"), _gpuAnimInfo.invBindPosePalette);
				Uniform<mat4f>::set(shader->findUniformByName("pose"), _gpuAnimInfo.posePalette);
			}
			Uniform<vec3f>::set(shader->findUniformByName("light"), vec3f::s_one());
			_texture->set(shader->findUniformByName("tex0"), 0);
		}

		for (int i = 0; i < _gpuMeshes.size(); ++i) {
			attrLoc.bindAttribAndDraw(_gpuMeshes[i]);
		}
		_texture->unset(0);
	}
	_skinnedShader->unbind();
}

void SampleContext::_loadExampleAsset() {
	constexpr const char* kGltfFileName    = "Assets/Mesh/Woman.gltf";
	constexpr const char* kTextureFileName = "Assets/Textures/Woman.png";

	GLTFInfo info;
	GLTFLoader::s_readFile(info, kGltfFileName);

	_skeleton = new Skeleton();
	_skeleton->create(info);
	_clips.reserve(info.animationClips.size());
	_clips = std::move(info.animationClips);
	_clipNames = std::move(info.animationClipNames);

	_cpuMeshes.reserve(info.skinMeshes.size());
	_cpuMeshes.appendRange(info.skinMeshes);

	_gpuMeshes.reserve(info.skinMeshes.size());
	_gpuMeshes.appendRange(info.skinMeshes);

	_texture = new Texture(kTextureFileName);
}

void SampleContext::_loadExampleAsset_AlignFeetOnTheGround() {
	constexpr const char* kGltfFileName    = "Assets/Mesh/IKCourse.gltf";
	constexpr const char* kTextureFileName = "Assets/Textures/uvChecker.png";

	GLTFInfo info;
	GLTFLoader::s_readFile(info, kGltfFileName);

	_ikCourse  = info.staticMeshes;
	_triangles = IntersectionsUtil::meshesToTriangles(_ikCourse);
	_ikCourseTexture = new Texture(kTextureFileName);
}

void SampleContext::_loadExampleAsset_DualQuaterionMeshSkinning() {
	constexpr const char* kGltfFileName    = "Assets/Mesh/dq.gltf";
	constexpr const char* kTextureFileName = "Assets/Textures/dq.png";

	GLTFInfo info;
	GLTFLoader::s_readFile(info, kGltfFileName);

	_skeleton = new Skeleton();
	_skeleton->create(info);
	_clips.reserve(info.animationClips.size());
	_clips = std::move(info.animationClips);
	_clipNames = std::move(info.animationClipNames);

	_cpuMeshes.reserve(info.skinMeshes.size());
	_cpuMeshes.appendRange(info.skinMeshes);

	_gpuMeshes.reserve(info.skinMeshes.size());
	_gpuMeshes.appendRange(info.skinMeshes);

	_texture = new Texture(kTextureFileName);
}
   
void SampleContext::_loadExampleAsset_InstancedCrowds() {
	size_t clipCount = _clips.size();
	_crowds.resize(clipCount);

	_loadOrBakeAnimTex();
	_randomSetCrowdSize(20); // actorCount: 20 x clipCount: 20 crowds
}

void SampleContext::_loadExampleShader() {
	_staticShader = new Shader(
		"Assets/Shaders/static.vert",
		"Assets/Shaders/lit.frag"
	);
	_skinnedShader = new Shader(
		"Assets/Shaders/skinned.vert",
		"Assets/Shaders/lit.frag"
	);
	_dqSkinnedShader = new Shader(
		"Assets/Shaders/dualquaternion.vert",
		"Assets/Shaders/lit.frag"
	);
	_crowdSkinnedShader = new Shader(
		"Assets/Shaders/crowd.vert",
		"Assets/Shaders/lit.frag"
	);
	_cpuAttribLoc.setByStaticShader (_staticShader);
	_gpuAttribLoc.setBySkinnedShader(_skinnedShader);
}

void SampleContext::_defaultSetAnimInfo() {
	auto& restPose = _skeleton->restPose();

	_cpuAnimInfo.playback     = 0.f;
	_cpuAnimInfo.animatedPose = restPose;
	_cpuAnimInfo.additivePose = restPose;
	_skeleton->getInvBindPose(_cpuAnimInfo.invBindPosePalette);

	_gpuAnimInfo.playback     = 0.f;
	_gpuAnimInfo.animatedPose = restPose;
	_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);
	_gpuAnimInfo.animatedPose.getDualQuaternionPalette(_gpuAnimInfo.dqPosePalette);
	_skeleton->getInvBindPose(_gpuAnimInfo.invBindPosePalette);
	_skeleton->getInvBindPose(_gpuAnimInfo.dqInvBindPosePalette);
	_gpuAnimInfo.additivePose = restPose;
}

void SampleContext::_defaultSelectClip() {
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

void SampleContext::_defaultSetAdditiveBasePose() {
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
		_cpuAnimInfo.additiveBasePose = Blending::makeAdditiveBasePose(*_skeleton.ptr(), clip);
		clip.setIsLoop(false);
	}

	{
		auto& clip = _clips[_gpuAnimInfo.additiveClip];
		_gpuAnimInfo.additiveBasePose = Blending::makeAdditiveBasePose(*_skeleton.ptr(), clip);
		clip.setIsLoop(false);
	}
}

void SampleContext::_defaultSetPoseDebugDraw() {
	_restPoseVisual		= new DebugDraw();
	_bindPoseVisual		= new DebugDraw();
	_currentPoseVisual	= new DebugDraw();

	_restPoseVisual->lineFromPose(_skeleton->restPose());
	_restPoseVisual->uploadToGpu();

	_bindPoseVisual->lineFromPose(_skeleton->bindPose());
	_bindPoseVisual->uploadToGpu();

	_currentPoseVisual->lineFromPose(_skeleton->bindPose());
	_currentPoseVisual->uploadToGpu();
}

void SampleContext::_defaultSetCamera(Request& req, const vec3f& pos /*= vec3f(0,0,0)*/, const vec3f& aim /*= vec3f(0,3,7)*/) {
	req.camera.setProjectionType(Math::CameraProjectionType::Perspective);
	req.camera.setPos(pos.x, pos.y, pos.z);
	req.camera.setAim(aim.x, aim.y, aim.z);
	req.camera.setFov(60.f);
	req.camera.setNearClip(0.01f);
	req.camera.setFarClip(1000.0f);
}

void SampleContext::_loadOrBakeAnimTex() {
	size_t clipCount = _clips.size();
	_crowdAnimTextures.resize(clipCount);

	static constexpr const int kTexSize = 512;

	for (int i = 0; i < clipCount; ++i) {
		TempString filename;
		const auto& clip = _clips[i];
		FmtTo(filename, "Assets/Textures/{}.animTex", clip.name());

		SPtr<AnimTexture> tex = new AnimTexture();
		if (File::exists(filename)) {
			tex->load(filename);
		} else {
			tex->resize(kTexSize);
			AnimBaker::bakeAnimationClipToTex(tex, _skeleton, clip);
			tex->save(filename, true, false);
		}
		_crowdAnimTextures[i].reset(tex);
	}
}

void SampleContext::_randomSetCrowdSize(size_t newCrowdCount) {
	Vector<vec3f> occupiedPoints;
	for (int i = 0; i < _crowds.size(); ++i) {
		auto c = make_unique<Crowd>();
		c->setShader(_crowdSkinnedShader);
		c->resize(newCrowdCount);
		c->randomizeTimes(_clips[i]);
		c->randomizePositions(occupiedPoints, vec3f(-40, 0, -80.0f), vec3f(40, 0, 30.0f), 8.f);
		if (_crowds[i]) {
			_crowds[i].release();
			_crowds[i] = nullptr;
		}
		_crowds[i] = std::move(c);
	}
}

} // namespace