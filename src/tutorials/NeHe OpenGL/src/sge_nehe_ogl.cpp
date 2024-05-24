#include "NeheOGL 001/NeHeOGL_Lesson001.h"
#include "NeheOGL 002/NeHeOGL_Lesson002.h"
#include "NeheOGL 003/NeHeOGL_Lesson003.h"
#include "NeheOGL 004/NeHeOGL_Lesson004.h"
#include "NeheOGL 005/NeHeOGL_Lesson005.h"
#include "NeheOGL 006/NeHeOGL_Lesson006.h"
#include "NeheOGL 007/NeHeOGL_Lesson007.h"
#include "NeheOGL 008/NeHeOGL_Lesson008.h"
#include "NeheOGL 009/NeHeOGL_Lesson009.h"

namespace sge {

class NeHeOGLApp : public NativeUIApp {
	using Base = NativeUIApp;
	using NeHeOGLMainWin = NeHeOGL_Lesson009;
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
			winDesc.rect = { 10, 10, 1376, 768 };
			_mainWin.create(winDesc);
		}
	}

private:
	NeHeOGLMainWin _mainWin;
};

}

int main(int argc, const TCHAR** argv) {
	sge::NeHeOGLApp app;
	sge::NeHeOGLApp::CreateDesc appDesc;
	app.run(appDesc);

	return 0;
}