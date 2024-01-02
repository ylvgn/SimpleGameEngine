#include "PW5_NativeUI.h"

namespace sge {

PW5_NativeUIApp* PW5_NativeUIApp::s_instance = nullptr;

PW5_NativeUIApp::PW5_NativeUIApp() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = this;
}

PW5_NativeUIApp::~PW5_NativeUIApp() {
	SGE_ASSERT(s_instance == this);
	s_instance = nullptr;
}

void PW5_NativeUIWindow::onCloseButton() {
	PW5_NativeUIApp::instance()->quit(0);
}

}