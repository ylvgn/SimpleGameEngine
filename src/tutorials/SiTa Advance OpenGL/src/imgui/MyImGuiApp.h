#pragma once

#include "MyImGui_Common.h"

namespace sge {

class MyImGuiUIWindow;

class MyImGuiApp : public NativeUIApp {
	using This = MyImGuiApp;
	using Base = NativeUIApp;
public:
	virtual ~MyImGuiApp() = default;

	SGE_INLINE static This* current() { return static_cast<This*>(Base::current()); }

	virtual void onUpdate(float dt) override;

protected:
	MyImGuiUIWindow* _mainWin = nullptr;
};

} // namespace sge