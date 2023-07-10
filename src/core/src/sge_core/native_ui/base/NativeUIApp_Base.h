#pragma once
#include <sge_core/app/AppBase.h>

namespace sge {

class NativeUIApp_Base : public AppBase {
public:
	struct CreateDesc {
		int fps = 60;
	};

			int  run(CreateDesc& desc);
			void update(float dt);
			void quit(int returnCode);
	virtual void willQuit() {}

protected:
	virtual void onCreate(CreateDesc& desc) {}
	virtual void onRun() {}
	virtual void onUpdate(float dt) {};
	virtual	void onQuit() {}

	u64   _tickCount = 0;
	float _frameRate = 1.f / 60.f;
	float _deltaTime = 0;
	int   _exitCode  = 0;
};

}