#include <sge_editor.h>

#include <sge_render.h>
#include <sge_render/mesh/RenderMesh.h>
#include <sge_render/mesh/WavefrontObjLoader.h>

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

		{
			// test draw triangle ----------------------------
			EditMesh editMesh;

			editMesh.pos.emplace_back(0.0f, 0.5f, 0.0f);
			editMesh.pos.emplace_back(0.5f, -0.5f, 0.0f);
			editMesh.pos.emplace_back(-0.5f, -0.5f, 0.0f);

			editMesh.color.emplace_back(255, 0, 0, 255);
			editMesh.color.emplace_back(0, 255, 0, 255);
			editMesh.color.emplace_back(0, 0, 255, 255);

			_renderMesh.create(editMesh);
		}
	}

	virtual void onCloseButton() {
		NativeUIApp::current()->quit(0);
	}

	virtual void onDraw() {
		Base::onDraw();
		if (!_renderContext) return;

		_renderContext->setFrameBufferSize(clientRect().size);

		_renderContext->beginRender();

		_cmdBuf.reset();
		_cmdBuf.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });
		_cmdBuf.drawMesh(SGE_LOC, _renderMesh);
		_cmdBuf.swapBuffers();

		_renderContext->commit(_cmdBuf);

		_renderContext->endRender();

		drawNeeded(); // what is `drawNeeded` use for? ----------
	}

	SPtr<RenderContext>	_renderContext;
	RenderCommandBuffer _cmdBuf;
	RenderMesh	_renderMesh;
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