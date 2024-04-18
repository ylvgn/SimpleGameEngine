#include "NativeUIApp_Base.h"
#include <sge_core/math/Math.h>

namespace sge {

int NativeUIApp_Base::run(CreateDesc& desc) {
	onCreate(desc);
	onRun();

	return _exitCode;
}

void NativeUIApp_Base::update(float dt) {
	_deltaTime += dt;

	// avoid overfrequency calls
	constexpr static float kToleranceEpison = 0.015f;
	if (_targetFrequency - _deltaTime > kToleranceEpison) {
		return;
	}

	if (_deltaTime > _fastForwardMinSec) {
		int n = static_cast<int>(Math::floor(_deltaTime / kFastForwardFrequency));
		_deltaTime = _deltaTime - n * kFastForwardFrequency;
		float doubleFrequency = 2 * kFastForwardFrequency;
		while (n > 0) {
			onUpdate(n == 1 ? kFastForwardFrequency : doubleFrequency);
			n -= 2;
		}
		// avoid overfrequency calls
		if (_targetFrequency - _deltaTime > kToleranceEpison) {
			return;
		}
	}
	
	onUpdate(_deltaTime);
	_deltaTime = 0;
}

void NativeUIApp_Base::quit(int exitCode) {
	_exitCode = exitCode;
	onQuit();
}

void NativeUIApp_Base::setFps(int fps) {
	_fps				= fps;
	_targetFrequency	= 1.f / _fps;
	_fastForwardMinSec	= _targetFrequency * 20 + kFastForwardFrequency;
	_acceptableMaxSec	= _targetFrequency * 0.2f;
}

}