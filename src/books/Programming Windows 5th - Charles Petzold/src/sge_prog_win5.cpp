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
/* Chap05 Device Context and Basic Drawing */
#include "Chap05/DevCaps1/PW5_DevCaps1.h"
#include "Chap05/SineWave/PW5_SineWave.h"
#include "Chap05/LineDemo/PW5_LineDemo.h"
#include "Chap05/Bezier/PW5_Bezier.h"
#include "Chap05/AltWind/PW5_AltWind.h"
#include "Chap05/WhatSize/PW5_MyMappingMode.h"
#include "Chap05/WhatSize/PW5_WhatSize.h"
#include "Chap05/RandRect/PW5_MyRectangle.h"
#include "Chap05/RandRect/PW5_RandRect.h"
#include "Chap05/Clover/PW5_MyRgn.h"
#include "Chap05/Clover/PW5_Clover.h"
/* Chap06 Keyboard */
//#include "Chap06/SysMets4/PW5_SysMets4.h" already impl into PW5_MyDefineMarkWindow

namespace sge {

class ProgWin5App : public NativeUIApp {
	using Base = NativeUIApp;
	using ProgWin5MainWin = PW5_MySysMets3;
protected:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test103");

		Base::onCreate(desc);

		{ // create window
			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			winDesc.rect = { 10, 10, 720, 540 };
			_mainWin.create(winDesc);
		}
	}

	virtual void onUpdate(float dt) override {
		_mainWin.update(dt);
	}

private:
	ProgWin5MainWin _mainWin;
};

} // namespace sge


SGE_MAIN(sge::ProgWin5App)