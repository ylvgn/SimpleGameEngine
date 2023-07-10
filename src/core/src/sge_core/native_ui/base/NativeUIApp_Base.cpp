#include "NativeUIApp_Base.h"

namespace sge {

int NativeUIApp_Base::run(CreateDesc& desc) {
	SGE_ASSERT(desc.fps > 0);
	_frameRate = 1.f / desc.fps;
	onCreate(desc);
	onRun();
	return _exitCode;
}

void NativeUIApp_Base::update(float dt) {
	_deltaTime += dt;
	if ((_deltaTime < _frameRate) && (_frameRate - _deltaTime > 0.002f)) {
		return;
	}
	onUpdate(_deltaTime);
	_deltaTime = 0;
}

void NativeUIApp_Base::quit(int exitCode) {
	_exitCode = exitCode;
	onQuit();
}

}