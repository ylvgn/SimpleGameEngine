#pragma once

#include "MyOpenGLNativeUIWindow_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

class MyOpenGLNativeUIWindow_Win32 : public MyOpenGLNativeUIWindow_Base {
	using Base = MyOpenGLNativeUIWindow_Base;
public:
	virtual ~MyOpenGLNativeUIWindow_Win32() { destroyGL(); }
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onInitGL() override;
	virtual void onDestroyGL() override;
	virtual void onSwapBuffers() override;

private:
	void _makeCurrent();

	HDC		_dc = nullptr;
	HGLRC	_rc = nullptr;
};

}
#endif