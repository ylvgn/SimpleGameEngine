#include "PW5_NativeUI.h"

/* Chap01 MessageBox */
#include "Chap01/HelloMsg/PW5_HelloMsg.h"
#include "Chap01/HelloMsg/PW5_MyMessageBox.h"
/* Chap02 Unicode */
#include "Chap02/ScrnSize/PW5_ScrnSize.h"
/* Chap03 CreateWindow and WndProc */
#include "Chap03/HelloWin/PW5_HelloWin.h"
#include "Chap03/HelloWin/PW5_MyHelloWin.h"
#include "Chap03/HelloWin/PW5_MyMineSweeper.h"
/* Chap04 Client Rect and Scroll Bar */
#include "Chap04/SysMets1/PW5_MyDrawText.h"
#include "Chap04/SysMets1/PW5_MyTextMetrics.h"
#include "Chap04/SysMets1/PW5_SysMets1.h"
#include "Chap04/SysMets2/PW5_SysMets2.h"
#include "Chap04/SysMets2/PW5_MySysMets2.h"
#include "Chap04/SysMets3/PW5_SysMets3.h"
#include "Chap04/SysMets3/PW5_MySysMets3.h"

namespace sge {

class ProgWin5App : public NativeUIApp {
	using Base = NativeUIApp;
	using ProgWin5MainWin = PW5_MyMineSweeper;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../examples/Test103");

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
			_mainWin.setWindowTitle(SGE_STRINGIFY(PW5_, SGE_E));
		}
	}

private:
	ProgWin5MainWin _mainWin;
};

}

int main(int argc, const TCHAR** argv) {
	sge::ProgWin5App app;
	sge::ProgWin5App::CreateDesc appDesc;
	app.run(appDesc);

	return 0;
}