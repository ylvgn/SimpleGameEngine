#include <sge_editor.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		desc.ownDC = true;
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

		{ // texture
			int w = 256;
			int h = 256;
			Texture2D_CreateDesc texDesc;
			Texture2D::UploadRequest texUploadRequest;
			texDesc.uploadRequest = &texUploadRequest;
			auto& image = texUploadRequest.imageToUpload;

			texDesc.size.set(w, h);
			texDesc.colorType = ColorType::RGBAb;
			image.create(Color4b::kColorType, w, h);

			for (int y = 0; y < w; y++) {
				auto span = image.row<Color4b>(y);
				for (int x = 0; x < h; x++) {
					span[x] = Color4b(	static_cast<u8>(x),	// r, span[x] means row[x]
										static_cast<u8>(y),	// g
										255,				// b
										255);				// a
				}
			}
			_testTexture = renderer->createTexture2D(texDesc);
		}

		auto shader = renderer->createShader("Assets/Shaders/test_texture.shader");
		_material = renderer->createMaterial();
		_material->setShader(shader);
		_material->setParam("mainTex", _testTexture);

		EditMesh editMesh;
#if 1
		WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
		EditMeshUtil::addColors(editMesh, Color4b(255, 255, 255, 255));
#else
		float d = 0.5f;
		editMesh.pos.emplace_back( 0, d, 0);
		editMesh.pos.emplace_back( d,-d, 0);
		editMesh.pos.emplace_back(-d,-d, 0);

		editMesh.color.emplace_back(255, 0, 0, 255);
		editMesh.color.emplace_back(0, 255, 0, 255);
		editMesh.color.emplace_back(0, 0, 255, 255);
#endif
		_renderMesh.create(editMesh);
	}

	virtual void onCloseButton() override {
		NativeUIApp::current()->quit(0);
	}

#if 1
	virtual void onDraw() override {
		Base::onDraw();
		static float dt = 1 / 60.f;
		onUpdate(dt);
		drawNeeded();
	}
#endif

	void onUpdate(float dt) {
		if (!_renderContext) return;

		auto time = ::GetTickCount() * 0.001f;
		auto s = Math::abs(Math::sin(time * 2));
		_material->setParam("test_float", s * 0.5f);
		_material->setParam("test_color", Color4f(s, s, s, 1));

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();

		_cmdBuf.reset(_renderContext);
		_cmdBuf.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

		for (auto& sm : _renderMesh.subMeshes()) {
			auto passes = _material->passes();
			for (size_t i = 0; i < passes.size(); ++i) {
				auto* cmd = _cmdBuf.addDrawCall();
				#if _DEBUG
					cmd->debugLoc = SGE_LOC;
				#endif

				cmd->material		   = _material;
				cmd->materialPassIndex = i;

				cmd->primitive		= RenderPrimitiveType::Triangles;
				cmd->vertexLayout	= _renderMesh.vertexLayout();
				cmd->vertexBuffer	= sm.vertexBuffer();
				cmd->vertexCount	= sm.vertexCount();
				cmd->indexCount		= sm.indexCount();
				cmd->indexBuffer	= sm.indexBuffer();
				cmd->indexType		= sm.indexType();
			}
		}

		ImGui::Render(); // TODO _renderContext->drawUI(_renderRequest);

		_cmdBuf.swapBuffers();
		_renderContext->commit(_cmdBuf);

		_renderContext->endRender();
	}

	SPtr<Material>		_material;
	SPtr<RenderContext>	_renderContext;
	RenderCommandBuffer _cmdBuf;
	RenderMesh			_renderMesh;

	SPtr<Texture2D>		_testTexture;
};

class EditorApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../../../../examples/Test101");

#if 1 // for quick testing (but not work for RenderDoc !!)
		CommandLine::runShell("Assets/Shaders/make.bat");
#endif

		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			renderDesc.apiType = Renderer::ApiType::OpenGL;
			Renderer::create(renderDesc);
		}

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1040, 880 };
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle("SGE Editor");
		}
	}

	virtual void onUpdate(float dt) override {
		_mainWin.onUpdate(dt);
	}

private:
	MainWin		_mainWin;
};

} // namespace sge


SGE_MAIN(sge::EditorApp)