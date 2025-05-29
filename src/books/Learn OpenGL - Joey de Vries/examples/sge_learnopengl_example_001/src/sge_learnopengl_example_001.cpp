#include <sge_learnopengl.h>

namespace sge {

// FYI: https://learnopengl.com/Advanced-OpenGL/Cubemaps
class sge_learnopengl_example_001 : public JoeyOGL_NativeUIWindow {
	using Base = JoeyOGL_NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();
		auto* editor = EditorContext::instance();

		{ // create shader
			_shader = renderer->createShader("Assets/Shaders/test.shader");
			_skyboxShader = renderer->createShader("Assets/Shaders/skybox.shader");
		}

		{ // create texture
			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker_BC7.dds");
		}

		{ // create texture cube(skybox)
			SamplerState s;
			s.filter = SamplerState::Filter::Linear;
			s.wrapU = SamplerState::Wrap::Clamp;
			s.wrapV = SamplerState::Wrap::Clamp;
			s.wrapW = SamplerState::Wrap::Clamp;

			Vector<StrView, TextureCube_UploadRequest::kFaceMaxCount> filenames;
			filenames.resizeToLocalBufSize();
			filenames[enumInt(TextureCubeFaceOrder::Right)]  = "Assets/Textures/Skybox/right.png";
			filenames[enumInt(TextureCubeFaceOrder::Left)]	 = "Assets/Textures/Skybox/left.png";
			filenames[enumInt(TextureCubeFaceOrder::Top)]	 = "Assets/Textures/Skybox/top.png";
			filenames[enumInt(TextureCubeFaceOrder::Bottom)] = "Assets/Textures/Skybox/bottom.png";
			filenames[enumInt(TextureCubeFaceOrder::Front)]  = "Assets/Textures/Skybox/front.png";
			filenames[enumInt(TextureCubeFaceOrder::Back)]   = "Assets/Textures/Skybox/back.png";
			_testTextureCube = renderer->createTextureCubeFromFiles(filenames, s);
		}

		{ // create 1 entity
			{ // create mesh
				EditMesh editMesh;

				WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
				EditMeshUtil::addColors(editMesh, Color4b(255, 255, 255, 255));

				_meshAsset = new MeshAsset();
				_meshAsset->mesh.create(editMesh);
			}

			auto* e = _scene.addEntity("Entity");
			auto* t = e->transform();
			t->setLocalPos(0, 0, 0);
			{ // add CMeshRenderer
				auto* mr = e->addComponent<CMeshRenderer>();
				mr->mesh = _meshAsset;
				auto mtl = renderer->createMaterial();
				mtl->setShader(_shader);
				mtl->setParam("test_color", Color4f(1, 1, 1, 1));
				mtl->setParam("test_float", 1.f);
				mtl->setParam("mainTex", _testTexture);
				mr->material = mtl;
			}
		}

		{ // create 2 entity
			{ // create mesh
				EditMesh editMesh;

				WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/box.obj");
				EditMeshUtil::addColors(editMesh, Color4b(255, 255, 255, 255));

				_boxMesh = new MeshAsset();
				_boxMesh->mesh.create(editMesh);
			}

			auto* e = _scene.addEntity("skybox");
			auto* t = e->transform();
			t->setLocalPos(0, 0, 0);
			t->setLocalScale({ 60.f,35.f,35.f });
			{ // add CMeshRenderer
				auto* mr = e->addComponent<CMeshRenderer>();
				mr->mesh = _boxMesh;
				auto mtl = renderer->createMaterial();
				mtl->setShader(_skyboxShader);
				mtl->setParam("skyboxMap", _testTextureCube);
				mr->material = mtl;
			}
		}

		editor->entitySelection.add(EntityId(1));
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		using KeyCode = UIKeyboardEventKeyCode;
		Base::onUIKeyboardEvent(ev);
	}

	virtual void onDraw() override {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();
		_renderRequest.reset(_renderContext, _camera);

		_renderRequest.cameraFrustum = _camera.frustum();
		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

		CRendererSystem::instance()->render(_renderRequest);

		_hierarchyWindow.draw(_scene, _renderRequest);
		_inspectorWindow.draw(_scene, _renderRequest);
		_statisticsWindow.draw(_scene, _renderRequest);

		_renderContext->drawUI(_renderRequest);
		_renderRequest.swapBuffers();
		_renderRequest.commit();

		_renderContext->endRender();
		drawNeeded();
	}

	SPtr<Texture2D>		_testTexture;
	SPtr<MeshAsset>		_meshAsset;
	SPtr<Shader>		_shader;

	SPtr<Shader>		_skyboxShader;
	SPtr<MeshAsset>		_boxMesh;
	SPtr<TextureCube>	_testTextureCube;
};

} // namespace sge

SGE_MAIN(sge::JoeyOGL_NativeUIApp<sge::sge_learnopengl_example_001>)