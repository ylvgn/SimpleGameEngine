#include <sge_editor.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	using TestVertex = VertexT_Color<Color4f, 1, VertexT_Pos<Tuple4f>>;

	virtual void onCreate(CreateDesc& desc) {
		SGE_DUMP_VAR(sizeof(TestVertex));
		SGE_DUMP_VAR(memberOffset(&TestVertex::pos));
		SGE_DUMP_VAR(memberOffset(&TestVertex::color));
		VertexLayoutManager::instance()->registerLayout<TestVertex>();

		desc.ownDC = true;
		Base::onCreate(desc);
		auto* renderer = Renderer::instance();

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext = renderer->createContext(renderContextDesc);
		}

		_createTestMesh();
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

		_vertexCount = editMesh.pos.size();

		Vector<TestVertex> vertexData;
		_vertexLayout = TestVertex::s_layout();
		vertexData.resize(_vertexCount);

		auto* dst = vertexData.begin();
		for (int i = 0; i < _vertexCount; i++) {
			auto& pos = editMesh.pos[i];
			auto& color = editMesh.color[i];
			dst->pos.set(pos.x, pos.y, pos.z, 1);
			dst->color[0].set(
				static_cast<float>(color.r / 255),
				static_cast<float>(color.g / 255),
				static_cast<float>(color.b / 255),
				static_cast<float>(color.a / 255)
			);
			dst++;
		}
		SGE_ASSERT(dst == vertexData.end());

		RenderGpuBuffer::CreateDesc desc;
		desc.type = RenderGpuBufferType::Vertex;
		desc.bufferSize = _vertexCount * _vertexLayout->stride;
		_vertexBuffer = Renderer::instance()->createGpuBuffer(desc);
		_vertexBuffer->uploadToGpu(spanCast<u8>(vertexData.span()));
	}

	virtual void onCloseButton() {
		NativeUIApp::current()->quit(0);
	}

	virtual void onDraw() {
		Base::onDraw();
		if (!_renderContext) return;

		_renderContext->setFrameBufferSize(clientRect().size);

		_renderContext->beginRender();

		_cmdBuf.reset(_renderContext);
		_cmdBuf.clearFrameBuffers();
		{
			auto* cmd = _cmdBuf.addDrawCall();
#if _DEBUG
			cmd->debugLoc = SGE_LOC;
#endif
			cmd->primitive		= RenderPrimitiveType::Triangles;
			cmd->vertexLayout	= _vertexLayout;
			cmd->vertexBuffer	= _vertexBuffer;
			cmd->vertexCount	= _vertexCount;
		}
		_cmdBuf.swapBuffers();

		_renderContext->commit(_cmdBuf);

		_renderContext->endRender();
		drawNeeded();
	}

	SPtr<RenderContext>	_renderContext;
	RenderCommandBuffer _cmdBuf;

	const VertexLayout*		_vertexLayout = nullptr;
	SPtr<RenderGpuBuffer>	_vertexBuffer;
	size_t					_vertexCount = 0;
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