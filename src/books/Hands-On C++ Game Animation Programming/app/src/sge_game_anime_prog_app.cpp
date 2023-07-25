#include <sge_game_anime_prog.h>
#include <sge_game_anime_prog/animation/CubicCurveExample.h>

namespace sge {

// https://registry.khronos.org/OpenGL/api/GL/wglext.h
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

struct AnimationAttribLocation {

	int pos;
	int normal;
	int uv;
	int weights;
	int joints;

	void setBySkinnedShader(const Shader* shader) {
		pos		= shader->findAttributeByName("position");
		normal	= shader->findAttributeByName("normal");
		uv		= shader->findAttributeByName("texCoord");
		weights = shader->findAttributeByName("weights");
		joints	= shader->findAttributeByName("joints");
	}

	void setByStaticShader(const Shader* shader) {
		pos		= shader->findAttributeByName("position");
		normal	= shader->findAttributeByName("normal");
		uv		= shader->findAttributeByName("texCoord");
		weights = -1;
		joints  = -1;
	}
};

struct AnimationInstance {
	Transform model; // used to model matrix

	// used to clip sampling
	Pose  animatedPose;
	int   clip = 0;
	float playback = 0.f;

	// gpu skinning
	Vector<mat4> posePalette;

	// blending
	Pose additivePose;
	int  additiveClip = 0;
	Pose additiveBasePose;

// helper functions ------------------------
	void animatedSample(const Span<const Clip> clips, float dt) {
		if (clips.size() <= clip) return;
		playback = clips[clip].sample(animatedPose, playback + dt);
	}

	template<class T>
	void animatedSample(const ClipT<T>& clip, float dt) {
		playback = clip.sample(animatedPose, playback + dt);
	}
};

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	void update(float dt) { onUpdate(dt); };

	void render() {
		beginRender();
		onRender();
		endRender();
	}

	Color4f kRed	{1,0,0,1};
	Color4f kGreen	{0,1,0,1};
	Color4f kBlue	{0,0,1,1};
	Color4f kYellow	{1,1,0,1};
	Color4f kPurple	{1,0,1,1};

protected:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);

		{ // create opengl render context
			const HDC dc = hdc();
			PIXELFORMATDESCRIPTOR pfd;
			pfd					= {};
			pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion		= 1;
			pfd.dwFlags			= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.iPixelType		= PFD_TYPE_RGBA;
			pfd.cColorBits		= 24;
			pfd.cDepthBits		= 32;
			pfd.cStencilBits	= 8;
			pfd.iLayerType		= PFD_MAIN_PLANE;
			int pixelFormat		= ChoosePixelFormat(dc, &pfd);
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
				} else {
					SGE_LOG("Could not enable vsynch\n");
				}
			}
			else { // !swapControlSupported
				SGE_LOG("WGL_EXT_swap_control not supported\n");
			}

			glGenVertexArrays(1, &_vertexArrayObject);
			glBindVertexArray(_vertexArrayObject);
		}
	}
	virtual void onUpdate(float dt) {}
	virtual void onRender() {}

	virtual void onCloseButton() override {
		if (_vertexArrayObject != 0) {
			HDC dc = hdc();
			HGLRC hglrc = wglGetCurrentContext();

			// delete VAO
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &_vertexArrayObject);
			_vertexArrayObject = 0;

			// delete render context
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(_hwnd, dc);
		}

		NativeUIApp::current()->quit(0);
	}

	virtual void beginRender() {
		float clientWidth = _clientRect.size.x;
		float clientHeight = _clientRect.size.y;
		glViewport(0, 0, static_cast<GLsizei>(clientWidth), static_cast<GLsizei>(clientHeight));

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPointSize(5.0f);

		glBindVertexArray(_vertexArrayObject);
		glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		_aspect = _clientRect.size.x / _clientRect.size.y;
	}

	virtual void endRender() {
		SwapBuffers(hdc());
		if (_vsynch != 0) {
			glFinish();
		}
	}

protected:

	int		_vsynch = 0;
	GLuint	_vertexArrayObject = 0;
	float	_aspect = 0;
};

class MyExampleMainWin : public MainWin {
	using Base = MainWin;
	using This = MyExampleMainWin;

#define RUN_CASE__ITEM(E, SGE_FN, ...) \
	case MyCaseType::E: { \
		test_##E##_##SGE_FN(__VA_ARGS__); \
	} break; \
// ----------
#define RUN_CASE__onCreate(E, ...) RUN_CASE__ITEM(E, onCreate)
#define RUN_CASE__onUpdate(E, ...) RUN_CASE__ITEM(E, onUpdate, dt)
#define RUN_CASE__onRender(E, ...) RUN_CASE__ITEM(E, onRender)

#define RUN_CASE(SGE_FN, ...) \
	switch (__caseType) { \
		MyCaseType##_ENUM_LIST(RUN_CASE__##SGE_FN) \
	} \
// ----------

#define MyCaseType_ENUM_LIST(E) \
	E(LitTexture,) \
	E(AnimationScalarTrack,) \
	E(AnimationClip,) \
	E(MeshSkinning,) \
	E(AnimationBlending,) \
	E(Crossfading,) \
	E(AdditiveBlending,) \
	E(CCD,) \
	E(FABRIK,) \
// ----------
	SGE_ENUM_DECLARE(MyCaseType, u8)
	MyCaseType __caseType = MyCaseType::FABRIK;

	virtual void onCreate(CreateDesc& desc) override { Base::onCreate(desc); RUN_CASE(onCreate) }
	virtual void onUpdate(float dt)			override { RUN_CASE(onUpdate) }
	virtual void onRender()					override { RUN_CASE(onRender) }
#undef RUN_CASE

private:
	void test_LitTexture_onCreate() {
		_debugPoints = eastl::make_unique<DebugDraw>();
		_debugLines  = eastl::make_unique<DebugDraw>();

		_texture = new Texture("Assets/Textures/uvChecker.png");
		_staticShader = new Shader(
			"Assets/Shaders/static.vert",
			"Assets/Shaders/lit.frag"
		);
		_vertexPositions	= eastl::make_unique< Attribute<vec3> >();
		_vertexNormals		= eastl::make_unique< Attribute<vec3> >();
		_vertexTexCoords	= eastl::make_unique< Attribute<vec2> >();
		_indexBuffer		= eastl::make_unique<IndexBuffer>();

		Vector<vec3> positions{
			vec3(-1, -1, 0),
			vec3(-1,  1, 0),
			vec3( 1, -1, 0),
			vec3( 1,  1, 0),
		};
		Vector<u32> indices = {
			0,1,2,
			2,1,3
		};

		_debugPoints->push_back(positions);

		SGE_ASSERT(indices.size() == 6);
		for (int i = 0; i < indices.size(); i += 3) {
			_debugLines->push_back(positions[indices[i]]);
			_debugLines->push_back(positions[indices[i+1]]);

			_debugLines->push_back(positions[indices[i+1]]);
			_debugLines->push_back(positions[indices[i+2]]);

			_debugLines->push_back(positions[indices[i+2]]);
			_debugLines->push_back(positions[indices[i]]);
		}

		_vertexPositions->uploadToGpu(positions);
		_indexBuffer->uploadToGpu(indices);

		_debugPoints->uploadToGpu();
		_debugLines->uploadToGpu();

		Vector<vec3> normals;
		normals.resize(4, vec3::s_forward());
		_vertexNormals->uploadToGpu(normals);

		Vector<vec2> uvs = {
			vec2(0,0),
			vec2(0,1),
			vec2(1,0),
			vec2(1,1),
		};
		_vertexTexCoords->uploadToGpu(uvs);
	}
	void test_AnimationScalarTrack_onCreate() {
		_referenceLines 	= eastl::make_unique<DebugDraw>();
		_scalarTrackLines 	= eastl::make_unique<DebugDraw>();
		_handlePoints 		= eastl::make_unique<DebugDraw>();
		_handleLines 		= eastl::make_unique<DebugDraw>();

		float height = 1.8f;
		float left   = 1.0f;
		float right  = 12.f;
		float xRange = right - left;
		float paddingX = xRange + 1.f;

		{ // xy-coordinate figure
			// Range in both X and Y is 0 to 20, according to mat4::s_ortho
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
					_referenceLines->push_back(vec3(o.x, o.y + height, 0));

					_referenceLines->push_back(o);
					_referenceLines->push_back(vec3(o.x + xRange, o.y, 0));
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
					FrameUtil::createFrame(0.5f,  1.f),
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

					_scalarTrackLines->push_back(vec3(thisX, thisY, 0.1f));
					_scalarTrackLines->push_back(vec3(nextX, nextY, 0.1f));
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
						vec3 thisPoint(thisX, thisY, 0.3f);
						_handlePoints->push_back(thisPoint);

						static const float tangentScale = 0.5f;

						// in-coming tangent
						if (j > 0) {
							const float preTime = thisTime - 0.0005f; // sample the point near the handle point is approximate to tangent!
							float prevX = o.x + (preTime * xRange);

							sr.time = preTime;
							float prevY = o.y + (track.sample(sr) * height);
							vec3 prePoint(prevX, prevY, thisPoint.z);

							_handleLines->push_back(thisPoint);
							_handleLines->push_back(thisPoint + (prePoint-thisPoint).normalize() * tangentScale);
						}

						// out-going tangent
						if (j < frameCount - 1 && track.type() != Interpolation::Constant) {
							const float postTime = thisTime + 0.0005f;
							float postX = o.x + (postTime * xRange);

							sr.time = postTime;
							float postY = o.y + (track.sample(sr) * height);
							vec3 postPoint(postX, postY, thisPoint.z);

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
	void test_BezierAndHermiteCurve_onCreate() {
		_debugPoints = eastl::make_unique<DebugDraw>();
		_debugLines  = eastl::make_unique<DebugDraw>();

		{ // test bezier curve
			CubicCurveExample::Bezier curve(
				vec3(-5, 0, 0), // p1
				vec3(-2, 1, 0), // c1
				vec3( 2, 1, 0), // c2
				vec3( 5, 0, 0)  // p2
			);

			_debugPoints->push_back(curve.p1());
			_debugPoints->push_back(curve.c1());
			_debugPoints->push_back(curve.c2());
			_debugPoints->push_back(curve.p2());

			for (int i = 0; i < 199; i++) {
				float t1 = static_cast<float>(i) / 199.f;
				float t2 = static_cast<float>(i + 1) / 199.f;

				_debugLines->push_back(curve.lerp(t1));
				_debugLines->push_back(curve.lerp(t2));

				curve.factor(t1, _debugPoints);
			}

			curve.factor(1, _debugPoints);
		}

		{ // test hermite spline
			CubicCurveExample::Hermite curve(
				vec3(-5,0,0), // p1
				vec3( 5,0,0), // p1
				0.2f,         // tan1
				-3.f          // tan2
			);

			_debugPoints->push_back(curve.p1());
			_debugPoints->push_back(curve.p2());

			for (int i = 0; i < 199; i++) {
				float t1 = static_cast<float>(i) / 199.f;
				float t2 = static_cast<float>(i + 1) / 199.f;

				_debugLines->push_back(curve.lerp(t1));
				_debugLines->push_back(curve.lerp(t2));

				curve.factor(t1, _debugPoints);
			}

			curve.factor(1, _debugPoints);
		}

		_debugPoints->uploadToGpu();
		_debugLines->uploadToGpu();
	}
	void test_AnimationClip_onCreate() {
		_restPoseVisual    = eastl::make_unique<DebugDraw>();
		_bindPoseVisual    = eastl::make_unique<DebugDraw>();
		_currentPoseVisual = eastl::make_unique<DebugDraw>();

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
	}
	void test_MeshSkinning_onCreate() {
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
		RearrangeBones::s_rearrange(_skeleton, info.meshes, _fastClips.span());

		{ // test cpu skinning
			_staticShader = new Shader(
				"Assets/Shaders/static.vert",
				"Assets/Shaders/lit.frag"
			);
			_cpuMeshes.appendRange(info.meshes);
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
			_gpuMeshes.appendRange(info.meshes); // trigger Mesh::operator= and will uploadToGpu
			_gpuAnimInfo.animatedPose = _skeleton.restPose();
		}

		_cpuAnimInfo.model.position = vec3(-2, 0, 0);
		_gpuAnimInfo.model.position = vec3( 2, 0, 0);

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
	void test_AnimationBlending_onCreate() {
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
	void test_Crossfading_onCreate() {
		_loadExampleAsset();
		_createExampleShader();
		_defaultSetAnimInfo();

		CrossFadeController* fadeController = new CrossFadeController();
		fadeController->setSkeleton(&_skeleton);
		fadeController->play(&_clips[0]);

		_fadeTimer = 2.0f;
	}
	void test_AdditiveBlending_onCreate() {
		_loadExampleAsset();
		_createExampleShader();
		_defaultSetAnimInfo();
		_defaultSelectClip();
		_defaultSetAdditiveBasePose();

		_additiveTime = 0.f;
		_additiveDirection = 1.f;
	}
	void test_CCD_onCreate() {
		_ccdSolver = eastl::make_unique< IKSolverExample<CCDSolver> >();
		_ccdSolver->onCreate();
	}
	void test_FABRIK_onCreate() {
		_fabrikSolver = eastl::make_unique< IKSolverExample<FABRIKSolver> >();
		_fabrikSolver->onCreate();
	}

	void test_LitTexture_onUpdate(float dt) {
		_testRotation += dt * 45.0f;
		while (_testRotation > 360.0f) {
			_testRotation -= 360.0f;
		}
	}
	void test_AnimationScalarTrack_onUpdate(float dt)  {}
	void test_BezierAndHermiteCurve_onUpdate(float dt) {}
	void test_AnimationClip_onUpdate(float dt) {
		static float s_clipLoopingTime = 0.f;
		static const float k_clipMaxLoopTime = 4.f;

		const Clip& clip = _clips[_currentClip];
		_playbackTime = clip.sample(_currentPose, _playbackTime + dt);

		s_clipLoopingTime += dt;
		if (s_clipLoopingTime >= k_clipMaxLoopTime) {
			s_clipLoopingTime = 0.f;
			_currentClip = (_currentClip + 1) % _clips.size();
			_playbackTime = 0.f;
			_currentPose = _skeleton.restPose();
		}
		_currentPoseVisual->lineFromPose(_currentPose);
	}
	void test_MeshSkinning_onUpdate(float dt) {
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
	void test_AnimationBlending_onUpdate(float dt) {
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
	void test_Crossfading_onUpdate(float dt) {
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
	void test_AdditiveBlending_onUpdate(float dt) {
		_additiveTime += dt * _additiveDirection;
		if (_additiveTime < 0) {
			_additiveTime = 0;
			_additiveDirection *= -1.f;
		}
		if (_additiveTime > 1) {
			_additiveTime = 1;
			_additiveDirection *= -1.f;
		}

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
	void test_CCD_onUpdate(float dt) {
		_ccdSolver->onUpdate(dt);
	}
	void test_FABRIK_onUpdate(float dt) {
		_fabrikSolver->onUpdate(dt);
	}

	void test_LitTexture_onRender() {
		mat4 projection = mat4::s_perspective(60.0f, _aspect, 0.01f, 1000.0f);
		mat4 view = mat4::s_lookAt(vec3(0, 0, -5), vec3::s_zero(), vec3::s_up());
		mat4 model = mat4::s_quat(quat::s_angleAxis(Math::radians(_testRotation), vec3::s_forward())); // or mat4::s_identity();
		mat4 mvp = projection * view * model;

		_staticShader->bind();
		{
			{ // bind uniforms
				Uniform<mat4>::set(_staticShader->findUniformByName("model"), model);
				Uniform<mat4>::set(_staticShader->findUniformByName("view"), view);
				Uniform<mat4>::set(_staticShader->findUniformByName("projection"), projection);
				Uniform<vec3>::set(_staticShader->findUniformByName("light"), vec3::s_forward());
				_texture->set(_staticShader->findUniformByName("tex0"), 0);
			}

			{ // bind attributes
				_vertexPositions->bind(_staticShader->findAttributeByName("position"));
				_vertexNormals->bind(_staticShader->findAttributeByName("normal"));
				_vertexTexCoords->bind(_staticShader->findAttributeByName("texCoord"));
			}

			DrawUtil::draw(*_indexBuffer.get());
			_debugLines->draw(DebugDrawMode::Lines, mvp);
			_debugPoints->draw(DebugDrawMode::Points, mvp, kBlue);

			{ // unbind uniforms
				_texture->unset(0);
				_vertexPositions->unbind(_staticShader->findAttributeByName("position"));
				_vertexNormals->unbind(_staticShader->findAttributeByName("normal"));
				_vertexTexCoords->unbind(_staticShader->findAttributeByName("texCoord"));
			}
		}
		_staticShader->unbind();
	}
	void test_AnimationScalarTrack_onRender() {
		float t = 22.f;
		float b = 0;
		float l = 0;
		float r = _aspect * t;
		float n = 0.01f;
		float f = 5.f;
		mat4 projection = mat4::s_ortho(l, r, b, t, n, f);
		mat4 view       = mat4::s_lookAt(vec3(0, 0, (n + f) / 2), vec3::s_zero(), vec3::s_up());
		mat4 mvp        = projection * view * mat4::s_identity();

		_referenceLines->draw(DebugDrawMode::Lines, mvp, kYellow);
		_scalarTrackLines->draw(DebugDrawMode::Lines, mvp, kGreen);
		_handlePoints->draw(DebugDrawMode::Points, mvp, kBlue);
		_handleLines->draw(DebugDrawMode::Lines, mvp, kPurple);
	}
	void test_BezierAndHermiteCurve_onRender() {
		mat4 projection = mat4::s_perspective(60.0f, _aspect, 0.01f, 1000.0f);
		mat4 view       = mat4::s_lookAt(vec3(0, 0, -5), vec3::s_zero(), vec3::s_up());
		mat4 mvp        = projection * view * mat4::s_identity();
		_debugLines->draw(DebugDrawMode::Lines, mvp);
		_debugPoints->draw(DebugDrawMode::Points, mvp, kBlue);
	}
	void test_AnimationClip_onRender() {
		mat4 projection = mat4::s_perspective(60.0f, _aspect, 0.01f, 10.f);
		mat4 view = mat4::s_lookAt(vec3(0, 4, -7), vec3(0, 4, 0), vec3::s_up());
		mat4 mvp = projection * view;

		_restPoseVisual->draw(DebugDrawMode::Lines, mvp, kRed);
		_bindPoseVisual->draw(DebugDrawMode::Lines, mvp, kGreen);
		_currentPoseVisual->uploadToGpu();
		_currentPoseVisual->draw(DebugDrawMode::Lines, mvp, kBlue);
	}
	void test_MeshSkinning_onRender() {
		mat4 projection = mat4::s_perspective(60.0f, _aspect, 0.01f, 10.f);
		mat4 view = mat4::s_lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3::s_up());

		{ // test cpu skinning
			mat4 model = mat4::s_transform(_cpuAnimInfo.model);

			_staticShader->bind();
			{
				{ // bind uniforms
					Uniform<mat4>::set(_staticShader->findUniformByName("model"), model);
					Uniform<mat4>::set(_staticShader->findUniformByName("view"), view);
					Uniform<mat4>::set(_staticShader->findUniformByName("projection"), projection);
					Uniform<vec3>::set(_staticShader->findUniformByName("light"), vec3::s_one());
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
			mat4 model = mat4::s_transform(_gpuAnimInfo.model);

			// bind uniform
			_skinnedShader->bind();
			{
				{ // bind uniforms
					Uniform<mat4>::set(_skinnedShader->findUniformByName("model"), model);
					Uniform<mat4>::set(_skinnedShader->findUniformByName("view"), view);
					Uniform<mat4>::set(_skinnedShader->findUniformByName("projection"), projection);
					Uniform<vec3>::set(_skinnedShader->findUniformByName("light"), vec3::s_one());
#if 1 // test pre-multiplied skin matrix
					Uniform<mat4>::set(_skinnedShader->findUniformByName("animated"), _gpuAnimInfo.posePalette);
#else
					Uniform<mat4>::set(_skinnedShader->findUniformByName("invBindPose"), _skeleton.invBindPose());
					Uniform<mat4>::set(_skinnedShader->findUniformByName("pose"), _gpuAnimInfo.posePalette);
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
	void test_AnimationBlending_onRender() {
		_onDrawGpuSkinning();
	}
	void test_Crossfading_onRender() {
		_onDrawGpuSkinning();
	}
	void test_AdditiveBlending_onRender() {
		_onDrawGpuSkinning();
	}
	void test_CCD_onRender() {
		_ccdSolver->onRender(_aspect);
	}

	void test_FABRIK_onRender() {
		_fabrikSolver->onRender(_aspect);
	}

private:

	template<class IKSolver>
	class IKSolverExample : public NonCopyable {
		
		const Color4f kRed		{1,0,0,1};
		const Color4f kGreen	{0,1,0,1};
		const Color4f kBlue		{0,0,1,1};
		const Color4f kYellow	{1,1,0,1};
		const Color4f kPurple	{1,0,1,1};

		const float kCamPitch	= 45.0f;
		const float kCamYaw		= 60.0f;
		const float kCamDist	= 7.0f;

		const float kGizmoSize	= 0.25f;
	public:
		IKSolverExample() {
			_sr.time = 0.0f;
			_sr.isLoop = true;

			{ // create ikChains
				_solver.resize(6);
				_solver[0] = Transform(vec3::s_zero(), quat::s_angleAxis(Math::radians(90.f), vec3::s_right()), vec3::s_one());
				_solver[1] = Transform(vec3(0, 0, 1.0f), quat::s_identity(), vec3::s_one());
				_solver[2] = Transform(vec3(0, 0, 1.5f), quat::s_identity(), vec3::s_one());
				_solver[3] = Transform(vec3(0, 0, 0.5f), quat::s_angleAxis(Math::radians(90.0f), vec3::s_up()), vec3::s_one());
				_solver[4] = Transform(vec3(0, 0, 0.5f), quat::s_identity(), vec3::s_one());
				_solver[5] = Transform(vec3(0, 0, 0.5f), quat::s_identity(), vec3::s_one());
			}

			{ // add one clip, and make it loop
				vec3f startPos = vec3(1,-2,0);
				_target.position = startPos;
				const int kFrameCount = 14;
				float factor = 0.5f;
				VectorTrack& posTrack = constCast(_targetPath.position());
				posTrack.resize(kFrameCount);
				posTrack[0 ] = FrameUtil::createFrame(0.f,   startPos     * factor);
				posTrack[1 ] = FrameUtil::createFrame(1.0f,  vec3(1, 2,0) * factor);
				posTrack[2 ] = FrameUtil::createFrame(2.0f,  vec3(1, 4,0) * factor);
				posTrack[3 ] = FrameUtil::createFrame(3.0f,  vec3(3, 4,0) * factor);
				posTrack[4 ] = FrameUtil::createFrame(4.0f,  vec3(5, 4,0) * factor);
				posTrack[5 ] = FrameUtil::createFrame(5.0f,  vec3(5, 4,2) * factor);
				posTrack[6 ] = FrameUtil::createFrame(6.0f,  vec3(5, 4,4) * factor);
				posTrack[7 ] = FrameUtil::createFrame(7.0f,  vec3(3, 4,4) * factor);
				posTrack[8 ] = FrameUtil::createFrame(8.0f,  vec3(3, 2,4) * factor);
				posTrack[9 ] = FrameUtil::createFrame(9.0f,  vec3(3, 2,2) * factor);
				posTrack[10] = FrameUtil::createFrame(10.0f, vec3(1, 2,2) * factor);
				posTrack[11] = FrameUtil::createFrame(11.0f, vec3(1, 0,2) * factor);
				posTrack[12] = FrameUtil::createFrame(12.0f, vec3(1,-2,2) * factor);
				posTrack[13] = FrameUtil::createFrame(13.0f, startPos     * factor);
			}
		}

		void onCreate() {
			// debugDraw depends on opengl, create it after "gladLoadGL"
			_debugLines = eastl::make_unique<DebugDraw>();
			_debugPoints = eastl::make_unique<DebugDraw>();

			_targetVisual.resize(3);
			for (int i = 0; i < 3; i++) {
				_targetVisual[i] = eastl::make_unique<DebugDraw>(2);
			}
		}

		void onUpdate(float dt) {
			_sr.time += dt;
			if (_sr.time > _targetPath.getEndTime()) {
				_sr.time -= _targetPath.getEndTime();
			}

			_target = _targetPath.sample(_target, _sr);
//			SGE_LOG("{}s :\t{}", _sr.time, _target.position);
			_solver.solve(_target);
		}

		void onRender(float aspect) {
			vec3 eyesPos(kCamDist * cosf(Math::radians(kCamYaw)) * sinf(Math::radians(kCamPitch)),
				kCamDist * cosf(Math::radians(kCamPitch)),
				kCamDist * sinf(Math::radians(kCamYaw)) * sinf(Math::radians(kCamPitch))
			);

			mat4 projection = mat4::s_perspective(60.0f, aspect, 0.01f, 100.0f);
			mat4 view = mat4::s_lookAt(eyesPos /*vec3(0, 0, 10)*/, vec3::s_zero(), vec3::s_up());
			mat4 mvp = projection * view * mat4::s_identity();

			{ // draw ikChains
				_debugLines->linesFromIKSolver(_solver);
				_debugPoints->pointsFromIKSolver(_solver);
				_debugLines->uploadToGpu();
				_debugPoints->uploadToGpu();
				_debugLines->draw(DebugDrawMode::Lines, mvp, kPurple);
				_debugPoints->draw(DebugDrawMode::Points, mvp, kPurple);
			}

			{ // 6 points -> 3 lines (xyz-axis) of target
				(*_targetVisual[0])[0] = _target.position + (vec3::s_right() * kGizmoSize);		// x - red
				(*_targetVisual[0])[1] = _target.position + (vec3::s_right() * -kGizmoSize);		//-x - red

				(*_targetVisual[1])[0] = _target.position + (vec3::s_up() * kGizmoSize);			// y - green
				(*_targetVisual[1])[1] = _target.position + (vec3::s_up() * -kGizmoSize);			//-y - green

				(*_targetVisual[2])[0] = _target.position + (vec3::s_forward() * kGizmoSize);		// z - blue
				(*_targetVisual[2])[1] = _target.position + (vec3::s_forward() * -kGizmoSize);	//-z - blue

				_targetVisual[0]->uploadToGpu();
				_targetVisual[1]->uploadToGpu();
				_targetVisual[2]->uploadToGpu();

				_targetVisual[0]->draw(DebugDrawMode::Lines, mvp, kRed);
				_targetVisual[1]->draw(DebugDrawMode::Lines, mvp, kGreen);
				_targetVisual[2]->draw(DebugDrawMode::Lines, mvp, kBlue);
			}
		}

	private:

		UPtr<DebugDraw>				_debugPoints;
		UPtr<DebugDraw>				_debugLines;
		Vector<UPtr<DebugDraw>, 3>	_targetVisual;

		Track_SampleRequest			_sr;

		Transform					_target;
		TransformTrack				_targetPath;

		IKSolver					_solver;
	};

	void _onDrawGpuSkinning() {
		mat4 projection = mat4::s_perspective(60.0f, _aspect, 0.01f, 10.f);
		mat4 view       = mat4::s_lookAt(vec3(0, 3, 7), vec3(0, 3, 0), vec3::s_up());
		mat4 model      = mat4::s_transform(_gpuAnimInfo.model);

		// bind uniform
		_skinnedShader->bind();
		{
			{ // bind uniforms
				Uniform<mat4>::set(_skinnedShader->findUniformByName("model"), model);
				Uniform<mat4>::set(_skinnedShader->findUniformByName("view"), view);
				Uniform<mat4>::set(_skinnedShader->findUniformByName("projection"), projection);
				Uniform<mat4>::set(_skinnedShader->findUniformByName("invBindPose"), _skeleton.invBindPose());
				Uniform<mat4>::set(_skinnedShader->findUniformByName("pose"), _gpuAnimInfo.posePalette);
				Uniform<vec3>::set(_skinnedShader->findUniformByName("light"), vec3::s_one());
				_texture->set(_skinnedShader->findUniformByName("tex0"), 0);
			}

			for (int i = 0; i < _gpuMeshes.size(); ++i) {
				_drawMesh(_gpuMeshes[i], _gpuAttribLoc);
			}
			_texture->unset(0);
		}
		_skinnedShader->unbind();
	}

	void _populatePosePalette() {
		_cpuAnimInfo.animatedPose.getMatrixPalette(_cpuAnimInfo.posePalette);
		_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);
	}

	void _drawMesh(Mesh& mesh, const AnimationAttribLocation& aloc) {
		mesh.bind(aloc.pos, aloc.normal, aloc.uv, aloc.weights, aloc.joints);
		mesh.draw();
		mesh.unbind(aloc.pos, aloc.normal, aloc.uv, aloc.weights, aloc.joints);
	}

	void _loadExampleAsset() {
		GLTFInfo info;
		GLTFLoader::s_readFile(info, "Assets/Mesh/Woman.gltf");

		_skeleton.create(info);
		_clips.reserve(info.animationClips.size());
		_clips = std::move(info.animationClips);

		_cpuMeshes.reserve(info.meshes.size());
		_cpuMeshes.appendRange(info.meshes);

		_gpuMeshes.reserve(info.meshes.size());
		_gpuMeshes.appendRange(info.meshes);

		_texture = new Texture("Assets/Textures/Woman.png");
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
		constexpr const char* k_ClipName = "Walking";
		for (int i = 0; i < _clips.size(); ++i) {
			if (_clips[i].name() == k_ClipName) {
				_currentClip = i;
				_cpuAnimInfo.clip = i;
				_gpuAnimInfo.clip = i;
				break;
			}
		}
	}

	void _defaultSetAdditiveBasePose() {
		constexpr const char* k_ClipName = "Lean_Left";
		for (int i = 0; i < _clips.size(); ++i) {
			if (_clips[i].name() == k_ClipName) {
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
	float									_testRotation = 0.0f;

	SPtr<Shader>							_staticShader;
	SPtr<Texture>							_texture;

	UPtr< Attribute<vec3> >					_vertexPositions;
	UPtr< Attribute<vec3> >					_vertexNormals;
	UPtr< Attribute<vec2> >					_vertexTexCoords;
	UPtr<IndexBuffer>						_indexBuffer;

	UPtr<DebugDraw>							_debugPoints;
	UPtr<DebugDraw>							_debugLines;

	Vector<ScalarTrack>						_scalarTracks;
	Vector<bool>							_scalarTracksIsLoop;

	UPtr<DebugDraw>							_scalarTrackLines;
	UPtr<DebugDraw>							_handlePoints;
	UPtr<DebugDraw>							_handleLines;
	UPtr<DebugDraw>							_referenceLines;

	Skeleton								_skeleton;
	Pose									_currentPose;
	Vector<Clip>							_clips;
	int										_currentClip;
	float									_playbackTime;

	UPtr<DebugDraw>							_restPoseVisual;
	UPtr<DebugDraw>							_bindPoseVisual;
	UPtr<DebugDraw>							_currentPoseVisual;

	Vector<Mesh>							_cpuMeshes;
	AnimationInstance						_cpuAnimInfo;
	AnimationAttribLocation					_cpuAttribLoc;

	SPtr<Shader>							_skinnedShader;
	Vector<Mesh>							_gpuMeshes;
	AnimationInstance						_gpuAnimInfo;
	AnimationAttribLocation					_gpuAttribLoc;

	Vector<FastClip>						_fastClips;

	AnimationInstance						_blendAnimA;
	AnimationInstance						_blendAnimB;
	float									_elapsedBlendTime;
	bool									_isInvertBlend;

	float									_fadeTimer;

	float									_additiveTime;      // 0~1
	float									_additiveDirection; // -1,1

	UPtr< IKSolverExample<CCDSolver> >		_ccdSolver;
	UPtr< IKSolverExample<FABRIKSolver> >	_fabrikSolver;
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
			winDesc.rect = { 10, 10, 800, 600 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Game Anime Prog Window");
		}
	}

	virtual void onUpdate(float dt) override {
		_mainWin.update(dt);
		_mainWin.render();
	}

private:
	MyExampleMainWin _mainWin;
};

} // namespace


int main(int argc, const char** argv) {
	sge::GameAnimeProgApp app;
	sge::GameAnimeProgApp::CreateDesc desc;
	app.run(desc);

	return 0;
}
