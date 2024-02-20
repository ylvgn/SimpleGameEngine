#include "NeheOGL 001/NeheOGL_Lesson001.h"
#include "NeheOGL 002/NeheOGL_Lesson002.h"
#include "NeheOGL 003/NeheOGL_Lesson003.h"
#include "NeheOGL 004/NeheOGL_Lesson004.h"
#include "NeheOGL 005/NeheOGL_Lesson005.h"
#include "NeheOGL 006/NeheOGL_Lesson006.h"

namespace sge {

class NeheOGLApp : public NativeUIApp {
	using Base = NativeUIApp;
	using NeheOGLMainWin = NeheOGL_Lesson006;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../examples/Test104");

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