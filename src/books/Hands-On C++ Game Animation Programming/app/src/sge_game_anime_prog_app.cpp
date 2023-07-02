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

struct AnimationInstance {
	Pose animatedPose;
	Transform model;

	// cpu skinning
	int clip = 0;
	float playback = 0.f;

	// gpu skinning
	Vector<mat4> posePalette;
};

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
	const Color4f k_red		{1,0,0,1};
	const Color4f k_green	{0,1,0,1};
	const Color4f k_blue	{0,0,1,1};
	const Color4f k_yellow	{1,1,0,1};
	const Color4f k_purple	{1,0,1,1};
public:
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

#if 0 // test lit texture
		_debugPoints		= eastl::make_unique<DebugDraw>();
		_debugLines			= eastl::make_unique<DebugDraw>();

		_testTexture		= eastl::make_unique<Texture>("Assets/Textures/uvChecker.png");
		_testShader			= eastl::make_unique<Shader>("Assets/Shaders/static.vert", "Assets/Shaders/lit.frag");
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
#endif

#if 0 // test bezier curve or hermite spline

		_debugPoints = eastl::make_unique<DebugDraw>();
		_debugLines  = eastl::make_unique<DebugDraw>();
		{ // test bezier curve
			CubicCurveExample::Bezier curve(
				vec3(-5, 0, 0), // p1
				vec3(-2, 1, 0), // c1
				vec3(2, 1, 0),  // c2
				vec3(5, 0, 0)   // p2
			);

			_debugPoints->push_back(curve.p1());
			_debugPoints->push_back(curve.c1());
			_debugPoints->push_back(curve.c2());
			_debugPoints->push_back(curve.p2());

			for (int i = 0; i < 199; i++) {
				float t1 = static_cast<float>(i) / 199.f;
				float t2 = static_cast<float>(i+1) / 199.f;

				_debugLines->push_back(curve.lerp(t1));
				_debugLines->push_back(curve.lerp(t2));

				curve.factor(t1, _debugPoints);
			}

			curve.factor(1, _debugPoints);
		}

		{ // test hermite spline
			CubicCurveExample::Hermite curve(
				vec3(-5, 0, 0), // p1
				vec3(5, 0, 0),  // p1
				0.2f,  // tan1
				-3.f   // tan2
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
#endif

#if 0 // test track
		_referenceLines		= eastl::make_unique<DebugDraw>();
		_scalarTrackLines	= eastl::make_unique<DebugDraw>();
		_handlePoints		= eastl::make_unique<DebugDraw>();
		_handleLines		= eastl::make_unique<DebugDraw>();

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
					size_t numFrames = track.size();
					for (int j = 0; j < numFrames; ++j) {
						const auto& keyFrame = track[j];
						const float thisTime = keyFrame.time;
						sr.time = thisTime;

						float thisX = o.x + (thisTime * xRange);
						float thisY = o.y + (track.sample(sr) * height);

						// key frame point
						vec3 thisPoint(thisX, thisY, 0.3f);
						_handlePoints->push_back(thisPoint);

						static const float tangentScale = 0.5f;

						// incoming tangent
						if (j > 0) {
							const float preTime = thisTime - 0.0005f; // sample the point near the handle point is approximate to tangent!
							float prevX = o.x + (preTime * xRange);

							sr.time = preTime;
							float prevY = o.y + (track.sample(sr) * height);
							vec3 prePoint(prevX, prevY, thisPoint.z);

							_handleLines->push_back(thisPoint);
							_handleLines->push_back(thisPoint + (prePoint-thisPoint).normalize() * tangentScale);
						}

						// outgoing tangent
						if (j < numFrames - 1 && track.type() != Interpolation::Constant) {
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
#endif

#if 0 // test animation clip
		_restPoseVisual		= eastl::make_unique<DebugDraw>();
		_bindPoseVisual		= eastl::make_unique<DebugDraw>();
		_currentPoseVisual	= eastl::make_unique<DebugDraw>();

		GLTFInfo info;
		GLTFLoader::s_readFile(info, "Assets/Mesh/Woman.gltf");
		_skeleton.create(info);
		_clips = std::move(info.animationClips);

		_restPoseVisual->fromPose(info.restPose());
		_restPoseVisual->uploadToGpu();

		_bindPoseVisual->fromPose(info.bindPose());
		_bindPoseVisual->uploadToGpu();

		_currentClip = 0;
		_playbackTime = 0.f;
		_currentPose = info.restPose();
		_currentPoseVisual->fromPose(_currentPose);
		_currentPoseVisual->uploadToGpu();

		// play 'Walking' animation clip
		for (int i = 0; i < _clips.size(); ++i) {
			if (_clips[i].name() == "Walking") {
				_currentClip = i;
				break;
			}
		}
#endif

#if 1 // test skinning
		_diffuseTexture	= eastl::make_unique<Texture>("Assets/Textures/Woman.png");

		GLTFInfo info;
		GLTFLoader::s_readFile(info, "Assets/Mesh/Woman.gltf");

#if 1 // test optimize clip
		_fastClips.resize(info.animationClips.size());
		for (int i = 0; i < info.animationClips.size(); ++i) {
			_fastClips[i] = ClipUtil::optimizeClip(info.animationClips[i]);
		}
#endif

		_skeleton.create(info);
		_clips = std::move(info.animationClips);
		RearrangeBones::s_rearrange(_skeleton, info.meshes, _fastClips.span());

		{ // test cpu skinning
			_staticShader = eastl::make_unique<Shader>(
				"Assets/Shaders/static.vert",
				"Assets/Shaders/lit.frag"
			);
			_cpuMeshes.appendRange(info.meshes);
			_cpuAnimInfo.animatedPose = _skeleton.restPose();
		}

		{ // test gpu skinning
#if 1 // test pre-multiplied skin matrix
			_skinnedShader = eastl::make_unique<Shader>(
				"Assets/Shaders/preskinned.vert",
				"Assets/Shaders/lit.frag"
			);
#else
			_skinnedShader = eastl::make_unique<Shader>(
				"Assets/Shaders/skinned.vert",
				"Assets/Shaders/lit.frag"
			);
#endif
			_gpuMeshes.appendRange(info.meshes); // trigger Mesh::operator= and will uploadToGpu
			_gpuAnimInfo.animatedPose = _skeleton.restPose();
			_gpuAnimInfo.posePalette.resize(_gpuAnimInfo.animatedPose.size());
			_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);
		}

		_cpuAnimInfo.model.position = vec3(-2, 0, 0);
		_gpuAnimInfo.model.position = vec3( 2, 0, 0);

		for (int i = 0; i < _clips.size(); ++i) {
			StrView clipName = _clips[i].name();
			if (clipName == "Walking") {
				_cpuAnimInfo.clip = i;
			}
			if (clipName == "Running") {
				_gpuAnimInfo.clip = i;
			}
		}
#endif
	}

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

	void update(float dt) {
#if 0 // test lit texture
		_testRotation += dt * 45.0f;
		while (_testRotation > 360.0f) {
			_testRotation -= 360.0f;
		}
#endif

#if 0 // test animation clip
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
		_currentPoseVisual->fromPose(_currentPose);
#endif

#if 1 // test skinning
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
			_gpuAnimInfo.playback = clip.sample(_gpuAnimInfo.animatedPose, _gpuAnimInfo.playback + dt);
			_gpuAnimInfo.animatedPose.getMatrixPalette(_gpuAnimInfo.posePalette);
		#if 1 // test pre-multiplied skin matrix
			const auto& invBindPose = _skeleton.invBindPose();
			for (int i = 0; i < _gpuAnimInfo.posePalette.size(); ++i) {
				_gpuAnimInfo.posePalette[i] = _gpuAnimInfo.posePalette[i] * invBindPose[i];
			}
		#endif
		}
#endif
	}

	void render() {
		HDC dc = hdc();

		// set viewport
		float clientWidth  = _clientRect.size.x;
		float clientHeight = _clientRect.size.y;
		glViewport(0, 0, static_cast<GLsizei>(clientWidth), static_cast<GLsizei>(clientHeight));

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPointSize(5.0f);

		glBindVertexArray(_vertexArrayObject);
		glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float aspect = clientWidth / clientHeight;
		(void)aspect;

#if 0 // test lit texture
		mat4 projection = mat4::s_perspective(60.0f, aspect, 0.01f, 1000.0f);
		mat4 view = mat4::s_lookAt(vec3(0, 0, -5), vec3::s_zero(), vec3::s_up());
		mat4 model = mat4::s_quat(quat::s_angleAxis(Math::radians(_testRotation), vec3::s_forward())); // or mat4::s_identity();
		mat4 mvp = projection * view * model;

		_testShader->bind();
		{

			{ // bind uniforms
				Uniform<mat4>::set(_testShader->findUniformByName("model"), model);
				Uniform<mat4>::set(_testShader->findUniformByName("view"), view);
				Uniform<mat4>::set(_testShader->findUniformByName("projection"), projection);
				Uniform<vec3>::set(_testShader->findUniformByName("light"), vec3::s_forward());
				_testTexture->set(_testShader->findUniformByName("tex0"), 0);
			}

			{ // bind attributes
				_vertexPositions->bind(_testShader->findAttributeByName("position"));
				_vertexNormals->bind(_testShader->findAttributeByName("normal"));
				_vertexTexCoords->bind(_testShader->findAttributeByName("texCoord"));
			}

			DrawUtil::draw(*_indexBuffer.get());
			_debugLines->draw(DebugDrawMode::Lines, mvp);
			_debugPoints->draw(DebugDrawMode::Points, mvp, k_blue);

			{ // unbind uniforms
				_testTexture->unset(0);
				_vertexPositions->unbind(_testShader->findAttributeByName("position"));
				_vertexNormals->unbind(_testShader->findAttributeByName("normal"));
				_vertexTexCoords->unbind(_testShader->findAttributeByName("texCoord"));
			}
		}
		_testShader->unbind();
#endif

#if 0 // test bezier curve or hermite spline
		mat4 projection = mat4::s_perspective(60.0f, aspect, 0.01f, 1000.0f);
		mat4 view = mat4::s_lookAt(vec3(0, 0, -5), vec3::s_zero(), vec3::s_up());
		mat4 mvp = projection * view * mat4::s_identity();
		_debugLines->draw(DebugDrawMode::Lines, mvp);
		_debugPoints->draw(DebugDrawMode::Points, mvp, k_blue);
#endif

#if 0 // test track
		float l = 0;
		float b = 0;
		float t = 22.f;
		float r = aspect * t;
		float n = 0.01f;
		float f = 5.f;
		mat4 view = mat4::s_lookAt(vec3(0, 0, (n + f) / 2), vec3::s_zero(), vec3::s_up());

		//mat4 projection = mat4::s_ortho(0, aspect * 22.0f, 0, 22, 0.01f, 5);
		mat4 projection = mat4::s_ortho(l, r, b, t, n, f);

		mat4 mvp = projection * view * mat4::s_identity();
		_referenceLines->draw(DebugDrawMode::Lines, mvp, k_yellow);
		_scalarTrackLines->draw(DebugDrawMode::Lines, mvp, k_green);
		_handlePoints->draw(DebugDrawMode::Points, mvp, k_blue);
		_handleLines->draw(DebugDrawMode::Lines, mvp, k_purple);
#endif

#if 0 // test animation clip
		mat4 projection = mat4::s_perspective(60.0f, aspect, 0.01f, 10.f);
		mat4 view = mat4::s_lookAt(vec3(0, 4, -7), vec3(0, 4, 0), vec3::s_up());
		mat4 mvp = projection * view;

		_restPoseVisual->draw(DebugDrawMode::Lines, mvp, k_red);
		_bindPoseVisual->draw(DebugDrawMode::Lines, mvp, k_green);
		_currentPoseVisual->uploadToGpu();
		_currentPoseVisual->draw(DebugDrawMode::Lines, mvp, k_blue);
#endif

#if 1 // test skinning
		mat4 projection = mat4::s_perspective(60.0f, aspect, 0.01f, 10.f);
		mat4 view = mat4::s_lookAt(vec3(0,5,7), vec3(0,3,0), vec3::s_up());

		{ // test cpu skinning
			mat4 model = mat4::s_transform(_cpuAnimInfo.model);

			_staticShader->bind();
			{
				{ // bind uniforms
					Uniform<mat4>::set(_staticShader->findUniformByName("model"), model);
					Uniform<mat4>::set(_staticShader->findUniformByName("view"), view);
					Uniform<mat4>::set(_staticShader->findUniformByName("projection"), projection);
					Uniform<vec3>::set(_staticShader->findUniformByName("light"), vec3::s_one());
					_diffuseTexture->set(_staticShader->findUniformByName("tex0"), 0);
				}

				u32 pos		= _staticShader->findAttributeByName("position");
				u32 normal	= _staticShader->findAttributeByName("normal");
				u32 uv		= _staticShader->findAttributeByName("texCoord");
				for (int i = 0; i < _cpuMeshes.size(); ++i) {
					auto& mesh = _cpuMeshes[i];
					mesh.bind(pos, normal, uv);
					mesh.draw();
					mesh.unbind(pos, normal, uv);
				}
				_diffuseTexture->unset(0);
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
					_diffuseTexture->set(_skinnedShader->findUniformByName("tex0"), 0);
				}

				u32 pos		= _skinnedShader->findAttributeByName("position");
				u32 normal	= _skinnedShader->findAttributeByName("normal");
				u32 uv		= _skinnedShader->findAttributeByName("texCoord");
				u32 weights = _skinnedShader->findAttributeByName("weights");
				u32 joints	= _skinnedShader->findAttributeByName("joints");
				for (int i = 0; i < _gpuMeshes.size(); ++i) {
					auto& mesh = _gpuMeshes[i];
					mesh.bind(pos, normal, uv, weights, joints);
					mesh.draw();
					mesh.unbind(pos, normal, uv, weights, joints);
				}
				_diffuseTexture->unset(0);
			}
			_skinnedShader->unbind();
		}
#endif
		SwapBuffers(dc);
		if (_vsynch != 0) {
			glFinish();
		}
	}

private:
	int _vsynch = 0;

	GLuint _vertexArrayObject = 0;

	float _testRotation = 0.0f;

	UPtr<Shader>  _testShader;
	UPtr<Texture> _testTexture;

	UPtr< Attribute<vec3> >	_vertexPositions;
	UPtr< Attribute<vec3> >	_vertexNormals;
	UPtr< Attribute<vec2> >	_vertexTexCoords;
	UPtr<IndexBuffer>		_indexBuffer;

	UPtr<DebugDraw>			_debugPoints;
	UPtr<DebugDraw>			_debugLines;

	Vector<ScalarTrack>		_scalarTracks;
	Vector<bool>			_scalarTracksIsLoop;

	UPtr<DebugDraw>			_scalarTrackLines;
	UPtr<DebugDraw>			_handlePoints;
	UPtr<DebugDraw>			_handleLines;
	UPtr<DebugDraw>			_referenceLines;

	Skeleton				_skeleton;
	Pose					_currentPose;
	Vector<Clip>			_clips;
	int						_currentClip;
	float					_playbackTime;

	UPtr<DebugDraw>			_restPoseVisual;
	UPtr<DebugDraw>			_bindPoseVisual;
	UPtr<DebugDraw>			_currentPoseVisual;

	UPtr<Texture>			_diffuseTexture;

	UPtr<Shader>			_staticShader;
	Vector<Mesh>			_cpuMeshes;
	AnimationInstance		_cpuAnimInfo;

	UPtr<Shader>			_skinnedShader;
	Vector<Mesh>			_gpuMeshes;
	AnimationInstance		_gpuAnimInfo;

	Vector<FastClip>		_fastClips;
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
		Base::onUpdate(dt);
		_deltaTime += dt;
		if (_deltaTime < kFPS) {
			return;
		}
		_mainWin.update(Math::min(kFPS, _deltaTime));
		_mainWin.render();
		_deltaTime -= kFPS;
	}

private:
	MainWin _mainWin;
	float _deltaTime = 0.f;
	const float kFPS = 1 / 60.0f;
};

} // namespace

int main(int argc, const char** argv) {
	sge::GameAnimeProgApp app;
	sge::GameAnimeProgApp::CreateDesc desc;
	app.run(desc);

	return 0;
}
