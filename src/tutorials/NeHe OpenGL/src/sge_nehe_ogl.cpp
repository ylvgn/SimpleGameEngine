#include "NeheOGL 001/NeheOGL_Lesson001.h"

namespace sge {

class NeheOGLApp : public NativeUIApp {
	using Base = NativeUIApp;
	using NeheOGLMainWin = NeheOGL_Lesson001;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../examples/Test104");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 1500, 900 };
			_mainWin.create(winDesc);
		}
	}

private:
	NeheOGLMainWin _mainWin;
};

}

int main(int argc, const TCHAR** argv) {
	sge::NeheOGLApp app;
	sge::NeheOGLApp::CreateDesc appDesc;
	app.run(appDesc);

	return 0;
}