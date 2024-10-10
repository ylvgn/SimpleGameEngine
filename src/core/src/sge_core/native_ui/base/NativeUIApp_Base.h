#pragma once
#include <sge_core/app/AppBase.h>

namespace sge {

class NativeUIApp_Base : public AppBase {
	using Base = AppBase;
public:
	struct CreateDesc {
	};

			int  run(CreateDesc& desc);
			void update(float dt);
			void quit(int returnCode);
	virtual void willQuit() {}

	void	setFps(int fps);

	int		fps()			const { return _fps; }
	u64		frameCount()	const { return _frameCount; }

protected:
	virtual void onCreate(CreateDesc& desc) {}
	virtual void onRun() {}
	virtual void onUpdate(float dt) {};
	virtual	void onQuit() {}

	constexpr static int	kFastForwardFps			= 60;
	constexpr static float	kFastForwardFrequency	= 1.f / kFastForwardFps;

	u64		_tickCount			= 0;
	u64		_frameCount			= 0;
	float	_deltaTime			= 0;

	int		_fps				= 60;
	float	_targetFrequency	= 1.f / _fps;
	float	_fastForwardMinSec	= _targetFrequency * 20 + kFastForwardFrequency;
	float	_acceptableMaxSec	= _targetFrequency * 0.2f;

	int		_exitCode			= 0;
};

} // namespace sge