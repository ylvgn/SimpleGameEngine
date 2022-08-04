#include <sge_editor.h>

#include <sge_render.h>
#include <sge_render/mesh/RenderMesh.h>
#include <sge_render/mesh/WavefrontObjLoader.h>
#include <sge_render/mesh/RenderTerrain.h>

#include <sge_core/file/FilePath.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_render/vertex/VertexLayoutManager.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) {
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

		// test
		_camera.setPos(58.932793f, 38.021767f, 3.6692433f);
		_camera.setAim(0.79875153f, 0.8193707f, 1.8785787f);

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

#if 1
		auto shader = renderer->createShader("Assets/Shaders/test.shader");
#else
		auto shader = renderer->createShader("Assets/Shaders/test.hlsl");
#endif

		_material = renderer->createMaterial();
		_material->setShader(shader);
		_material->setParam("mainTex", _testTexture);

		EditMesh editMesh;
#if 1
		WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
		// the current shader need color
		for (size_t i = editMesh.color.size(); i < editMesh.pos.size(); i++) {
			editMesh.color.emplace_back(255, 255, 255, 255);
		}
#else
		EditMesh editMesh;
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
		_testTerrain.create();
	}

	virtual void onCloseButton() {
		NativeUIApp::current()->quit(0);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		if (ev.isDragging()) {
			using Button = UIMouseEventButton;
			switch (ev.pressedButtons) {
				case Button::Left: {
					auto d = ev.deltaPos * 0.01f;
					_camera.orbit(d.x, d.y);
				}break;

				case Button::Middle: {
					auto d = ev.deltaPos * 0.005f;
					_camera.move(d.x, d.y, 0);
				}break;

				case Button::Right: {
					auto d = ev.deltaPos * -0.005f;
					_camera.dolly(d.x + d.y);
				}break;
			}
			//SGE_LOG("{}\t{}", _camera.pos(), _camera.aim());
		}
	}

	virtual void onDraw() {
		Base::onDraw();
		if (!_renderContext) return;

		_camera.setViewport(clientRect());

		{
			auto model = Mat4f::s_identity();
			auto view = _camera.viewMatrix();
			auto proj = _camera.projMatrix();
			auto mvp = proj * view;

			_material->setParam("sge_matrix_model", model);
			_material->setParam("sge_matrix_view", view);
			_material->setParam("sge_matrix_proj", proj);
			_material->setParam("sge_matrix_mvp", mvp);

			_material->setParam("sge_camera_pos", _camera.pos());

			_material->setParam("sge_light_pos", Vec3f(10, 10, 0));
			_material->setParam("sge_light_dir", Vec3f(-5, -10, -2));
			_material->setParam("sge_light_power", 4.0f);
			_material->setParam("sge_light_color", Vec3f(1, 1, 1));
#if 1
			_testTerrain.sge_matrix_view = view;
			_testTerrain.sge_matrix_proj = proj;
			_testTerrain.sge_camera_pos = _camera.pos();
#endif
		}
//-----
//		auto time = GetTickCount() * 0.001f;
//		auto s = abs(sin(time * 2));
		auto s = 1.0f;
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));
//-----

		_renderContext->setFrameBufferSize(clientRect().size);

		_renderContext->beginRender();

		_cmdBuf.reset();
		_cmdBuf.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });
#if 0
		_cmdBuf.drawMesh(SGE_LOC, _renderMesh, _material);
#else
		_testTerrain.render(_cmdBuf); // test
#endif

		_cmdBuf.swapBuffers();
		_renderContext->commit(_cmdBuf);
		_renderContext->endRender();

		drawNeeded();
	}

	RenderTerrain _testTerrain;

	SPtr<Material> _material;
	SPtr<Texture2D> _testTexture;

	SPtr<RenderContext>	_renderContext;
	RenderCommandBuffer _cmdBuf;
	RenderMesh _renderMesh;

	Math::Camera3f	_camera;
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

	#if 1 // for quick testing
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

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Editor");
		}
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