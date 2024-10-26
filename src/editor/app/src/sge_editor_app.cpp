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

#if 1
		_camera.setPos(0, 10, 10);
		//_camera.setPos(0, 1200, 10);	// debug terrain
		_camera.setAim(0, 0, 0);
#else
		// just for test 5x5 terrain 
		_camera.setPos(58.932793f, 38.021767f, 3.6692433f);
		_camera.setAim(0.79875153f, 0.8193707f, 1.8785787f);
#endif

		{ // texture
			Texture2D_CreateDesc texDesc;
			Texture2D::UploadRequest texUploadRequest;
			auto& image = texUploadRequest.imageToUpload;
			texDesc.uploadRequest = &texUploadRequest;
	#if 1
			//image.loadFile("Assets/Textures/uvChecker.bmp", ColorType::RGBAb);
			//image.loadFile("Assets/Textures/uvChecker.png");
			//image.loadFile("Assets/Textures/uvChecker_BC1.dds");
			//image.loadFile("Assets/Textures/uvChecker_BC2.dds");
			//image.loadFile("Assets/Textures/uvChecker_BC3.dds");
			image.loadFile("Assets/Textures/uvChecker_BC7.dds");

			texDesc.size = image.size();
			texDesc.colorType = image.colorType();
	#else
			int w = 256;
			int h = 256;

			texDesc.size.set(w, h);
			texDesc.colorType = ColorType::RGBAb;

			image.create(Color4b::kColorType, w, h);

			for (int y = 0; y < w; y++) {
				auto span = image.row<Color4b>(y);
				for (int x = 0; x < h; x++) {
					span[x] = Color4b(static_cast<u8>(x),	// r, span[x] means row[x]
						static_cast<u8>(y),					// g
						0,									// b
						255);								// a
				}
			}
	#endif
			_testTexture = renderer->createTexture2D(texDesc);
		}

		{ // line material
			auto lineShader = renderer->createShader("Assets/Shaders/line.shader");
			_lineMaterial = renderer->createMaterial();
			_lineMaterial->setShader(lineShader);
		}

		{ // material
			_shader = renderer->createShader("Assets/Shaders/test.shader");

			_material = renderer->createMaterial();
			_material->setShader(_shader);
			_material->setParam("mainTex", _testTexture);

			EditMesh editMesh;
#if 1
			WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
			// the current shader need color
			for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
				editMesh.color.emplace_back(255, 255, 255, 255);
			}
#else
			// triangle mesh
			editMesh.pos.emplace_back(0.0f, 0.5f, 0.0f);
			editMesh.pos.emplace_back(0.5f, -0.5f, 0.0f);
			editMesh.pos.emplace_back(-0.5f, -0.5f, 0.0f);

			editMesh.color.emplace_back(255, 0, 0, 255);
			editMesh.color.emplace_back(0, 255, 0, 255);
			editMesh.color.emplace_back(0, 0, 255, 255);

			editMesh.uv[0].emplace_back(0, 1);
			editMesh.uv[0].emplace_back(1, 1);
			editMesh.uv[0].emplace_back(1, 0);

			editMesh.normal.emplace_back(0, 0, 0);
			editMesh.normal.emplace_back(0, 0, 0);
			editMesh.normal.emplace_back(0, 0, 0);

			// because CullMode = D3D11_CULL_BACK; 2->1->0
			editMesh.indices.emplace_back(2);
			editMesh.indices.emplace_back(1);
			editMesh.indices.emplace_back(0);
#endif
			_renderMesh.create(editMesh);
		}

		{ // terrain
			float size = 2048;
			float pos = size / -2;
			float y = -100;
			float height = 200;
			int maxLod = 6;
			_terrain.createFromHeightMapFile(
				Vec3f(pos, y, pos),
				Vec2f(size, size),
				height,
				maxLod,
				"Assets/Terrain/TerrainTest/TerrainHeight_Small.png");
		}

		{ // ECS
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
		_renderContext->onUIKeyboardEvent(ev);
	}

	virtual void onDraw() override {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();

		_renderRequest.reset(_renderContext, _camera);

		{// debug culling
			auto fov = _camera.fov();
			_camera.setFov(fov / 2);
			_renderRequest.cameraFrustum = _camera.frustum();
			_camera.setFov(fov);
		}

		_renderRequest.debug.drawBoundingBox = true;

		_renderRequest.lineMaterial = _lineMaterial;
//		_renderRequest.matrix_model = Mat4f::s_identity();

		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

//-----
//		auto time = GetTickCount() * 0.001f;
//		auto s = abs(sin(time * 2));
		auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));
//-----

		_renderRequest.drawFrustum(_renderRequest.cameraFrustum, Color4b(100, 255, 100, 255));

		_renderRequest.drawMesh(SGE_LOC, _renderMesh, _material);
		//_terrain.render(_renderRequest);

		ImGui::ShowDemoWindow(nullptr);

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

	SPtr<Shader>		_shader;

	SPtr<Material>		_lineMaterial;

	SPtr<Material>		_material;
	SPtr<Texture2D>		_testTexture;

	SPtr<RenderContext>	_renderContext;
	RenderMesh			_renderMesh;

	SPtr<MeshAsset>		_meshAsset;

	RenderTerrain		_terrain;

	Math::Camera3f		_camera;
	Scene				_scene;

	RenderRequest		_renderRequest;

	EditorHierarchyWindow		_hierarchyWindow;
	EditorInspectorWindow		_inspectorWindow;
	EditorStatisticsWindow		_statisticsWindow;
};

class EditorApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../examples/Test101");

			Directory::setCurrent(workingDir);
			SGE_DUMP_VAR(Directory::current());
		}
#if SGE_OS_WINDOWS
	#if 1 // for quick testing (but not work for RenderDoc !!)
		CommandLine::runShell("compile_shaders.bat");
	#endif
#endif
		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			//renderDesc.apiType = OpenGL;
			Renderer::create(renderDesc);
		}

		EditorContext::create();
		EngineContext::create();

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1040, 880 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Editor");
		}
	}

	virtual void onQuit() {
		EngineContext::destroy();
		EditorContext::destroy();
		Base::onQuit();
	}

private:
	MainWin		_mainWin;
};

} // namespace sge

SGE_MAIN(sge::EditorApp)