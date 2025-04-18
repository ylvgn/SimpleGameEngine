#pragma once

#include "../native_ui/my_nativeui.h"

namespace sge {

class MyImGuiUIWindow : public NonCopyable {
public:
    struct CreateDesc : public NativeUIWindow_Base::CreateDesc {};

    MyImGuiUIWindow(CreateDesc& desc);
	~MyImGuiUIWindow();

    MyOpenGLNativeUIWindow*	getNativeWindow();
    void setWindowTitle(StrView title);

	void update(float dt);

protected:
	virtual bool onWindowCloseButton() { return true; }

	virtual bool onUIMouseEvent   (UIMouseEvent& ev)	{ return false; }
    virtual bool onUIKeyboardEvent(UIKeyboardEvent& ev) { return false; }

			void _onWindowActive(bool b);
	virtual void  onWindowActive(bool b) {}

	virtual void onUpdate(float dt) {}
	virtual void onRender()			{}

private:
    class Impl;
    Impl* _impl = nullptr;

    bool _windowActive : 1;
};

} // namespace sge
