#include <sge_editor.h>
#include <sge_core/file/FilePath.h>
#include <sge_core/pointer/SPtr.h>

namespace sge {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	UPtr<RenderContext>	_renderContext;

	virtual void onCreate(CreateDesc& desc) {
		Base::onCreate(desc);

		{ // create renderer
			Renderer::CreateDesc rendererCreateDesc;
			Renderer::create(rendererCreateDesc);
		}

		{ // create render context
			RenderContext::CreateDesc renderContextDesc;
			renderContextDesc.window = this;
			_renderContext.reset(RenderContext::create(renderContextDesc));
		}
	}

	virtual void onCloseButton() {
		NativeUIApp::current()->quit(0);
	}

	virtual void onDraw() {
		Base::onDraw();

		if (_renderContext) {
			_renderContext->render();
		}
		drawNeeded(); // what is `drawNeeded` use for? ----------
	}
};

class EditorApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
		
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			auto workingDir = FilePath::getDir(exeFilePath);
			workingDir.append("/../../../../../../examples/Test101");
			setCurrentDir(workingDir);

			auto curDir = getCurrentDir();
			SGE_LOG("current dir={}", curDir);
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