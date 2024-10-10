#include "NeHeOGL 001/NeHeOGL_Lesson001.h"
#include "NeHeOGL 002/NeHeOGL_Lesson002.h"
#include "NeHeOGL 003/NeHeOGL_Lesson003.h"
#include "NeHeOGL 004/NeHeOGL_Lesson004.h"
#include "NeHeOGL 005/NeHeOGL_Lesson005.h"
#include "NeHeOGL 006/NeHeOGL_Lesson006.h"
#include "NeHeOGL 007/NeHeOGL_Lesson007.h"
#include "NeHeOGL 008/NeHeOGL_Lesson008.h"
#include "NeHeOGL 009/NeHeOGL_Lesson009.h"
#include "NeHeOGL 010/NeHeOGL_Lesson010.h"
#include "NeHeOGL 011/NeHeOGL_Lesson011.h"
#include "NeHeOGL 012/NeHeOGL_Lesson012.h"

namespace sge {

class NeHeOGLApp : public NativeUIApp {
	using Base = NativeUIApp;
	using NeHeOGLMainWin = NeHeOGL_Lesson012;
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

} // namespace sge


SGE_MAIN(sge::NeHeOGLApp)