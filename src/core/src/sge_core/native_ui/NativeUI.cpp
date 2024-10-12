#include "NativeUI.h"

namespace sge {

NativeUIApp* NativeUIApp::_current = nullptr;

NativeUIApp* NativeUIApp::current() {
	return _current;
}

NativeUIApp::NativeUIApp() noexcept {
	SGE_ASSERT(_current == nullptr);
	_current = this;
}

NativeUIApp::~NativeUIApp() noexcept {
	SGE_ASSERT(_current == this);
	_current = nullptr;
}

} // namespace sge
