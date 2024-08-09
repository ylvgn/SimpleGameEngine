#include <sge_editor.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:

	using MyTestVertexT = VertexT_Color<Color4f, 1, VertexT_Pos<Tuple4f>>;

	virtual void onCreate(CreateDesc& desc) override {
		SGE_DUMP_VAR(sizeof(MyTestVertexT));
		SGE_DUMP_VAR(memberOffset(&MyTestVertexT::pos));
		SGE_DUMP_VAR(memberOffset(&MyTestVertexT::color));
		VertexLayoutManager::instance()->registerLayout<MyTestVertexT>();

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

		_createMyRenderMesh(editMesh);
	}

	void _loadTestMesh() {
		EditMesh editMesh;

		WavefrontObjLoader::readFile(editMesh, "Assets/Mesh/test.obj");
		editMesh.addColors({ 255, 0, 255, 255 }); // the current shader need color

		_createMyRenderMesh(editMesh);
	}

	void _createMyRenderMesh(EditMesh& editMesh) {
		size_t vc = editMesh.pos.size();

		_renderMesh.setSubMeshCount(1);
		_renderMesh.setVertexLayout(MyTestVertexT::s_layout());
		auto subMeshes = _renderMesh.subMeshes();
		auto& subMesh = subMeshes[0];
		subMesh.setVertexCount(vc);

		for (int i = 0; i < vc; ++i) {
			auto& pos	= editMesh.pos[i];
			auto& color = editMesh.color[i];
			auto* dst	= subMesh.vertex<MyTestVertexT>(i);

			(*dst).pos.set(pos.x, pos.y, pos.z, 1);
			(*dst).color[0].set(
				static_cast<float>(color.r / 255),
				static_cast<float>(color.g / 255),
				static_cast<float>(color.b / 255),
				static_cast<float>(color.a / 255)
			);
			++dst;
		}
		subMesh.setVertexBuffer();

		if (!editMesh.indices.empty())
			subMesh.setIndexData(editMesh.indices);
	}

	virtual void onDraw() override {
		Base::onDraw();
		if (!_renderContext) return;

		_renderContext->setFrameBufferSize(clientRect().size);

		_renderContext->beginRender();

		_cmdBuf.reset(_renderContext);
		_cmdBuf.clearFrameBuffers();
		{
			auto* cmd = _cmdBuf.addDrawCall();
			auto submeshes = _renderMesh.subMeshes();
			auto& submesh = submeshes[0];
#if _DEBUG
			cmd->debugLoc = SGE_LOC;
#endif
			cmd->primitive		= RenderPrimitiveType::Triangles;
			cmd->vertexLayout	= _renderMesh.vertexLayout();
			cmd->vertexBuffer	= submesh.vertexBuffer();
			cmd->vertexCount	= submesh.vertexCount();
			cmd->indexCount		= submesh.indexCount();
			cmd->indexBuffer	= submesh.indexBuffer();
			cmd->indexType		= submesh.indexType();
		}
		_cmdBuf.swapBuffers();

		_renderContext->commit(_cmdBuf);

		_renderContext->endRender();
		drawNeeded();
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