#include "NativeUI.h"

namespace sge {

NativeUIApp* NativeUIApp::_current = nullptr;

NativeUIApp* NativeUIApp::current() {
	return _current;
}

NativeUIApp::NativeUIApp() {
	SGE_ASSERT(_current == nullptr);
	_current = this;
}

NativeUIApp::~NativeUIApp() {
	SGE_ASSERT(_current == this);
	_current = nullptr;
}

}
