#pragma once

#include "MyOpenGLNativeUIWindow_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

class MyOpenGLNativeUIWindow_Win32 : public MyOpenGLNativeUIWindow_Base {
	using Base = MyOpenGLNativeUIWindow_Base;
public:
	virtual ~MyOpenGLNativeUIWindow_Win32() { destroy(); }
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDestroy() override;
	virtual void onSwapBuffers() override;

	virtual void onBeginRender() override;
	virtual void onEndRender() override;

private:
	void _makeCurrent();

	HDC		_dc = nullptr;
	HGLRC	_rc = nullptr;
};

}
#endif