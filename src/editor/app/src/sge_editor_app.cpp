#include <sge_editor.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();
		auto* editor = EditorContext::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

		{ // setup camera
		#if 1
			_camera.setPos(0, 10, 10);
//			_camera.setPos(0, 1200, 10); // debug terrain
			_camera.setAim(0, 0, 0);
		#else
			// just for test 5x5 terrain 
			_camera.setPos(58.932793f, 38.021767f, 3.6692433f);
			_camera.setAim(0.79875153f, 0.8193707f, 1.8785787f);
		#endif
		}

		{ // create texture
		#if 1
//			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker.bmp");
//			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker.png");
//			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker_BC1.dds");
//			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker_BC2.dds");
//			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker_BC3.dds");
			_testTexture = renderer->createTexture2DFromFile("Assets/Textures/uvChecker_BC7.dds");
		#else
			Texture2D_CreateDesc texDesc;
			Texture2D::UploadRequest texUploadRequest;
			texDesc.uploadRequest = &texUploadRequest;

			int w = 256;
			int h = 256;
			texDesc.size.set(w, h);
			texDesc.colorType = ColorType::RGBAb;

			Image image;
			image.create(Color4b::kColorType, w, h);
			for (int y = 0; y < w; y++) {
				auto span = image.row<Color4b>(y);
				for (int x = 0; x < h; x++) {
					span[x] = Color4b(static_cast<u8>(x),	// r, span[x] means row[x]
									  static_cast<u8>(y),	// g
									  0,					// b
									  255);					// a
				}
			}
			texUploadRequest.assign(image);
			_testTexture = renderer->createTexture2D(texDesc);
		#endif
		}

		{ // line material
			auto lineShader = renderer->createShader("Assets/Shaders/line.shader");
			_lineMaterial = renderer->createMaterial();
			_lineMaterial->setShader(lineShader);
		}

		{ // test material
			_shader = renderer->createShader("Assets/Shaders/test.shader");

			_material = renderer->createMaterial();
			_material->setShader(_shader);
			_material->setParam("mainTex", _testTexture);
		}

		{ // create mesh
			EditMesh editMesh;

			WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
			EditMeshUtil::addColors(editMesh, Color4b(255, 255, 255, 255));

			_renderMesh.create(editMesh);
		}

		{ // create terrain
			float size		= 2048;
			float pos		= size / -2.f;
			float y			= -100;
			float height	= 200;
			int maxLod		= 6;
			_terrain.createFromHeightMapFile(Vec3f(pos, y, pos),
											 Vec2f(size, size),
											 height,
											 maxLod,
											 "Assets/Terrain/TerrainTest/TerrainHeight_Small.png");
		}

		{ // create ECS
			EditMesh editMesh;
			WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/box.obj");
			EditMeshUtil::addColors(editMesh, Color4b(255, 255, 255, 255));
			
			_meshAsset = new MeshAsset();
			_meshAsset->mesh.create(editMesh);

			Vector<Entity*> entities;

			for (int i = 0; i < 25; i++) {
				auto* e = _scene.addEntity("Entity");
				auto* t = e->transform();

				entities.emplace_back(e);

				{ // CMeshRenderer
					auto* mr = e->addComponent<CMeshRenderer>();
					mr->mesh = _meshAsset;
					auto mtl = renderer->createMaterial();
					mtl->setShader(_shader);
					mtl->setParam("test_color", Color4f(1, 1, 1, 1));
					mtl->setParam("mainTex", _testTexture);
					mr->material = mtl;
				}

				{ // CTransform
					const int col = 5;
					int x = i % col;
					int z = i / col;

					if (x == 0) {
						t->setLocalPos(0, 4, static_cast<float>(z));
					} else {
						auto* parent = entities[z * col]->transform();
						parent->addChild(t);
						t->setLocalPos(static_cast<float>(x), 0, 0);
					}
				}
			}

//			editor->entitySelection.add(EntityId(2));
			editor->entitySelection.add(EntityId(3));
		}
	}

	virtual void onCloseButton() override {
		NativeUIApp::current()->quit(0);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		if (_renderContext->onUIMouseEvent(ev))
			return;

		if (ev.isDragging()) {
			using Button = UIMouseEventButton;
			switch (ev.pressedButtons) {
				case Button::Left: {
					auto d = ev.deltaPos * 0.005f;
					_camera.pan(-d.x, d.y);
				}break;
				case Button::Middle: {
					auto d = ev.deltaPos * 0.05f;
					_camera.move(d.x, d.y, 0);
				}break;
				case Button::Right: {
					auto d = ev.deltaPos * 0.005f;
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
		_renderContext->onUIKeyboardEvent(ev);
	}

	virtual void onDraw() override {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();
		_renderRequest.reset(_renderContext, _camera);
//		_renderRequest.setViewport(clientRect()); no use now

		{ // set camera culling frustum
			auto fov = _camera.fov();
			_camera.setFov(fov / 2); // just for debug camera culling
			_renderRequest.cameraFrustum = _camera.frustum();
			_camera.setFov(fov);
		}

		_renderRequest.debug.drawBoundingBox = true;
		_renderRequest.lineMaterial = _lineMaterial;
//		_renderRequest.matrix_model = Mat4f::s_identity();

		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

//-----
//		auto time = ::GetTickCount() * 0.001f;
//		auto s = Math::abs(Math::sin(time * 2));
		auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));
//-----

		_renderRequest.drawFrustum(_renderRequest.cameraFrustum, Color4b(100, 255, 100, 255));
		_renderRequest.drawMesh(SGE_LOC, _renderMesh, _material);
//		_terrain.render(_renderRequest);

		CRendererSystem::instance()->render(_renderRequest);

//----- ui stuff
		ImGui::ShowDemoWindow(nullptr);
		_hierarchyWindow.draw(_scene, _renderRequest);
		_inspectorWindow.draw(_scene, _renderRequest);
		_statisticsWindow.draw(_scene, _renderRequest);
		_renderContext->drawUI(_renderRequest);

		_renderRequest.swapBuffers();
		_renderRequest.commit();
		_renderContext->endRender();
		drawNeeded();
	}

	SPtr<Shader>				_shader;

	SPtr<Material>				_lineMaterial;

	SPtr<Material>				_material;
	SPtr<Texture2D>				_testTexture;

	SPtr<RenderContext>			_renderContext;
	RenderMesh					_renderMesh;

	SPtr<MeshAsset>				_meshAsset;

	RenderTerrain				_terrain;

	Math::Camera3f				_camera;
	Scene						_scene;

	RenderRequest				_renderRequest;

	EditorHierarchyWindow		_hierarchyWindow;
	EditorInspectorWindow		_inspectorWindow;
	EditorStatisticsWindow		_statisticsWindow;
};

class EditorApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test101");

		{ // just for quick debug
		#if 0
			CommandLine::runShell("Assets/Shaders/sge_gnu_make.bat");
		#else
			CommandLine::runShell("Assets/Shaders/sge_ninja.bat");
		#endif
		}

		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			renderDesc.apiType = Renderer::ApiType::OpenGL;
//			renderDesc.apiType = Renderer::ApiType::DX11;
			Renderer::create(renderDesc);
		}

		EditorContext::create();
		EngineContext::create();

		{ // create window
			TempString title;
			FmtTo(title, "SGE Editor - {}, VSync: {}", Renderer::instance()->apiType(), Renderer::instance()->vsync());

			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1040, 880 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle(title);
		}
	}

	virtual void onQuit() {
		EngineContext::destroy();
		EditorContext::destroy();
		Base::onQuit();
	}

private:
	MainWin	_mainWin;
};

} // namespace sge

SGE_MAIN(sge::EditorApp)