#include <sge_editor.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		SGE_DUMP_VAR(sizeof(Vertex_PosColor));

		desc.ownDC = true;
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}
#if 1
		_loadTestMesh();
#else
		_createTestMesh();
#endif
	}

	virtual void onCloseButton() override {
		NativeUIApp::current()->quit(0);
	}

	void _createTestMesh() {
		EditMesh editMesh;

		float d = 0.5f;
		editMesh.pos.emplace_back( 0, d, 0);
		editMesh.pos.emplace_back( d,-d, 0);
		editMesh.pos.emplace_back(-d,-d, 0);

		editMesh.color.emplace_back(255, 0, 0, 255);
		editMesh.color.emplace_back(0, 255, 0, 255);
		editMesh.color.emplace_back(0, 0, 255, 255);

		_renderMesh.create(editMesh);
	}

	void _loadTestMesh() {
		EditMesh editMesh;

		WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
		EditMesh::Util::addColors(editMesh, Color4b(255, 255, 255, 255));

		_renderMesh.create(editMesh);
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
//		SGE_DUMP_VAR(NativeUIApp::current()->frameCount());

		if (!_renderContext) return;

		_renderContext->setFrameBufferSize(clientRect().size);

		_renderContext->beginRender();

		_cmdBuf.reset(_renderContext);
		_cmdBuf.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });

		for (auto& sm : _renderMesh.subMeshes()) {
			auto* cmd = _cmdBuf.addDrawCall();
#if _DEBUG
			cmd->debugLoc = SGE_LOC;
#endif
			cmd->primitive		= RenderPrimitiveType::Triangles;
			cmd->vertexLayout	= _renderMesh.vertexLayout();
			cmd->vertexBuffer	= sm.vertexBuffer();
			cmd->vertexCount	= sm.vertexCount();
			cmd->indexCount		= sm.indexCount();
			cmd->indexBuffer	= sm.indexBuffer();
			cmd->indexType		= sm.indexType();
		}

		_cmdBuf.swapBuffers();

		_renderContext->commit(_cmdBuf);

		_renderContext->endRender();
	}

	SPtr<RenderContext>	_renderContext;
	RenderCommandBuffer _cmdBuf;
	RenderMesh			_renderMesh;
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

		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			renderDesc.apiType = Renderer::ApiType::GL3;
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

}

int main() {
	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;
	app.run(desc);

	return 0;
}