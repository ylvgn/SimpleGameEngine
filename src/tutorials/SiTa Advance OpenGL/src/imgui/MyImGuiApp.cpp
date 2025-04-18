#include "MyImGuiApp.h"
#include "MyImGuiWindow.h"

namespace sge {

void MyImGuiApp::onUpdate(float dt) {
	if (!_mainWin) return;
	_mainWin->update(dt);
}

} // namespace sge