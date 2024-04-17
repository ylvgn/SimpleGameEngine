#include "NativeUIApp_Base.h"

namespace sge {

int NativeUIApp_Base::run(CreateDesc& desc) {
	_frameRate		= 1.f / desc.fps;
	_toleranceDt	= desc.toleranceFrameCount * _frameRate;

	onCreate(desc);
	onRun();

	return _exitCode;
}

void NativeUIApp_Base::update(float dt) {
	_deltaTime += dt;

	if (_deltaTime < _frameRate) {
		return;
	}

	constexpr static int	kSupplyFps			= 60;
	constexpr static float	kSupplyFrameRate	= 1.f / kSupplyFps;

	if (_deltaTime > _toleranceDt + kSupplyFrameRate) {
		int n = static_cast<int>(_deltaTime / kSupplyFrameRate);
		_deltaTime = _deltaTime - n * kSupplyFrameRate;
		float supplyDt = 2 * kSupplyFrameRate;
		while (n > 0) {
			onUpdate(n == 1 ? kSupplyFrameRate : supplyDt);
			n -= 2;
		}
		if (_deltaTime < _frameRate) return;
	}
	
	onUpdate(_deltaTime);
	_deltaTime = 0;
}

void NativeUIApp_Base::quit(int exitCode) {
	_exitCode = exitCode;
	onQuit();
}

}