#include <sge_editor.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();
		auto* editor = EditorContext::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

#if 1
		_camera.setPos(0, 10, 30);
		//_camera.setPos(0, 1200, 10);  // debug
		_camera.setAim(0, 0, 0);
#else
		// just for test 5x5 terrain 
		_camera.setPos(58.932793f, 38.021767f, 3.6692433f);
		_camera.setAim(0.79875153f, 0.8193707f, 1.8785787f);
#endif

		Texture2D_CreateDesc texDesc;
		auto& image = texDesc.imageToUpload;
#if 1
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

		auto shader = renderer->createShader("Assets/Shaders/test.shader");

		_material = renderer->createMaterial();
		_material->setShader(shader);
		_material->setParam("mainTex", _testTexture);

#if 0
		EditMesh editMesh;
#if 1
		WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
		// the current shader need color
		for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
			editMesh.color.emplace_back(255, 255, 255, 255);
		}
#else
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
#endif

		{
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
			EditMesh cubeEditMesh;

			// pos color normal
			for (int a = 0; a < 2; a++)
				for (int b = 0; b < 2; b++)
					for (int c = 0; c < 2; c++) {
						cubeEditMesh.pos.emplace_back(a? a : -1, b? b : -1, c? c : -1);
						cubeEditMesh.color.emplace_back(255, 0, 0, 255);
						cubeEditMesh.normal.emplace_back(0, 0, 0);
					}

			// indexes
			Vector<u32> cubeIndexes = {
				2,0,1, 2,1,3,
				0,6,4, 0,2,6,
				3,7,2, 7,6,2,
				1,7,3, 1,5,7,
				4,6,7, 4,7,5,
				4,1,0, 4,5,1,
			};
			cubeEditMesh.indices = std::move(cubeIndexes);

			// uv
			cubeEditMesh.uv[0].emplace_back(0, 0);
			cubeEditMesh.uv[0].emplace_back(0, 1);
			cubeEditMesh.uv[0].emplace_back(1, 0);
			cubeEditMesh.uv[0].emplace_back(1, 1);
			cubeEditMesh.uv[0].emplace_back(1, 1);
			cubeEditMesh.uv[0].emplace_back(1, 0);
			cubeEditMesh.uv[0].emplace_back(0, 1);
			cubeEditMesh.uv[0].emplace_back(0, 0);

			auto test_shader = Renderer::instance()->createShader("Assets/Shaders/test.shader");

			for (int i = 0; i < 10; i++) {
				TempString s;
				FmtTo(s, "Object {}", i);
				auto* e = _scene.addEntity(s);

				{ // CTransform
					auto* t = e->addComponent<CTransform>();
					t->setPosition(static_cast<float>(i * 5), -2, 0);
				}

				{ // CMeshRenderer
					auto* meshRender = e->addComponent<CMeshRenderer>();
					auto mat = Renderer::instance()->createMaterial();
					mat->setShader(test_shader);
					meshRender->setMesh(cubeEditMesh);
					meshRender->setMaterial(mat);
				}
			}

			editor->entitySelection.add(EntityId(1));
			editor->entitySelection.add(EntityId(3));
		}
	}

	virtual void onCloseButton() {
		NativeUIApp::current()->quit(0);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		_renderContext->onUIMouseEvent(ev);
		if (_renderContext->wantCaptureMouse()) return;  //tmp

		if (ev.isDragging()) {
			using Button = UIMouseEventButton;
			switch (ev.pressedButtons) {
				case Button::Left: {
					auto d = ev.deltaPos * 0.01f;
					_camera.orbit(d.x, d.y);
				}break;

				case Button::Middle: {
					auto d = ev.deltaPos * 0.05f;
					_camera.move(d.x, d.y, 0);
				}break;

				case Button::Right: {
					auto d = ev.deltaPos * -0.05f;
					_camera.dolly(d.x + d.y);
				}break;
			}
			//SGE_LOG("{}\t{}", _camera.pos(), _camera.aim());
		}
	}

	virtual void onUIMouseCursor(UIMouseEvent& ev) override {
		_renderContext->onUIMouseCursor(ev);
	}

	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) override {
		//_renderContext.onUIKeyboardEvent(ev); todo
	}

	virtual void onDraw() {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();

		_renderRequest.reset(_renderContext);
		_renderRequest.matrix_model = Mat4f::s_identity();
		_renderRequest.matrix_view = _camera.viewMatrix();
		_renderRequest.matrix_proj = _camera.projMatrix();
		_renderRequest.matrix_vp = _camera.projMatrix() * _camera.viewMatrix();
		_renderRequest.camera_pos = _camera.pos();

		_renderRequest.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

//-----
//		auto time = GetTickCount() * 0.001f;
//		auto s = abs(sin(time * 2));
		auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));
//-----

		//_renderRequest.drawMesh(SGE_LOC, _renderMesh, _material);
		//_terrain.render(_renderRequest);

		//ImGui::ShowDemoWindow(nullptr);

		RendererSystem::instance()->render(_renderRequest);

		_hierarchyWindow.draw(_scene, _renderRequest);
		_inspectorWindow.draw(_scene, _renderRequest);

		_renderContext->drawUI(_renderRequest);
		_renderRequest.swapBuffers();
		_renderContext->commit(_renderRequest.commandBuffer);

		_renderContext->endRender();
		drawNeeded();
	}

	RenderTerrain _terrain;

	SPtr<Material> _material;
	SPtr<Texture2D> _testTexture;

	SPtr<RenderContext>	_renderContext;
	RenderMesh _renderMesh;

	Math::Camera3f	_camera;

	RenderRequest	_renderRequest;

	Scene			_scene;

	EditorHierarchyWindow _hierarchyWindow;
	EditorInspectorWindow _inspectorWindow;
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
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

	#if 0 // for quick testing (but not work for RenderDoc !!)
		{ // compile shader
			SHELLEXECUTEINFO ShExecInfo = {};
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = L"open";
			ShExecInfo.lpFile = L"compile_shaders.bat";
			ShExecInfo.lpParameters = L"";
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_SHOW;
			ShExecInfo.hInstApp = NULL;
			ShellExecuteEx(&ShExecInfo);
			WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
			CloseHandle(ShExecInfo.hProcess);
		}
	#endif
		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			//renderDesc.apiType = OpenGL;
			Renderer::create(renderDesc);
		}

		//TypeManager::instance()->registerType<CTransform>();
		//TypeManager::instance()->registerType<CMeshRenderer>();
		EditorContext::createContext();
		RendererSystem::createSytem();

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1040, 880 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Editor");
		}
	}

	virtual void onQuit() {
		RendererSystem::destroySystem();
		EditorContext::destroyContext();
		Base::onQuit();
	}

private:
	MainWin		_mainWin;
};

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;
	app.run(desc);

	return 0;
}