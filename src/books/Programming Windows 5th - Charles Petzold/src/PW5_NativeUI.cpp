#include "PW5_NativeUI.h"

namespace sge {

void PW5_NativeUIWindow::onCloseButton() {
	NativeUIApp::current()->quit(0);
}

}