#pragma once

#include "../MyCommon.h"

namespace sge {

struct MyGLFWUIKeyboardEvent;

#if 0
#pragma mark ========= MyGLFWNativeUIWindow ============
#endif
class MyGLFWNativeUIWindow : public NativeUIWindow_Base {
	using Base = NativeUIWindow_Base;
	using This = MyGLFWNativeUIWindow;
public:
	using KeyCode = UIKeyboardEventKeyCode;
	using UIKeyboardEvent = MyGLFWUIKeyboardEvent;

	static constexpr int kKeyCodeCount = GLFW_KEY_LAST + 1;

	struct CreateDesc : Base::CreateDesc {
		CreateDesc();

		enum class Profile : int {
			None	= GLFW_OPENGL_ANY_PROFILE,
			Core	= GLFW_OPENGL_CORE_PROFILE,
			Compat	= GLFW_OPENGL_COMPAT_PROFILE,
		};

		int		major;
		int		minor;
		Profile profile;
	};

	~MyGLFWNativeUIWindow() { destroy(); }

	void create(CreateDesc& desc) { onCreate(desc); };

	void destroy();
	void update();

	bool _glfwWindowShouldClose()							{ return glfwWindowShouldClose(_glfwWin); }
	void _glfwGetFramebufferSize(int& width, int& height)	{ glfwGetFramebufferSize(_glfwWin, &width, &height); }
	void _glfwGetWindowPos(int& xpos, int& ypos)			{ glfwGetWindowPos(_glfwWin, &xpos, &ypos); }
	void _glfwSetWindowShouldClose(int isClose)				{ glfwSetWindowShouldClose(_glfwWin, isClose); }

protected:
	virtual void onSetWindowTitle	(StrView title)		override { _glfwSetWindowTitle(title); }
	virtual void onSetWindowPos		(const Vec2f& pos)	override;
	virtual void onSetWindowSize	(const Vec2f& size) override;

	virtual void onCreate(CreateDesc& desc);
	virtual void onRender() {}
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) {}

	virtual void onFramebufferSizeCallback(int width, int height) {}
	virtual void onWindowPosCallback(int xpos, int ypos) {}

	GLFWwindow* _glfwWin = nullptr;

private:
	virtual UPtr<NativeUIScrollInfo_Base> onCreateScrollBar(NativeUIScrollInfo_CreateDesc& desc) { return nullptr; }; // TODO

	void _glfwSetWindowTitle(StrView title);
	void _glfwSwapBuffers() { glfwSwapBuffers(_glfwWin); }
	void _glfwPollEvents()	{ glfwPollEvents(); }

	void _handleNativeInputEvent();
	void _handleNativeUIKeyboardEvent();

	virtual void onUINativeKeyboardEvent(UIKeyboardEvent& ev) { onUIKeyboardEvent(ev); }

	static void s_glfwSetFramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void s_glfwSetWindowPosCallback(GLFWwindow* window, int xpos, int ypos);

	SGE_INLINE static This* s_getThis(GLFWwindow* window) {
		return reinterpret_cast<This*>(glfwGetWindowUserPointer(window));
	}
};

#if 0
#pragma mark ========= MyGLFWNativeUIApp ============
#endif
class MyGLFWNativeUIApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onRun() override;
	virtual void onQuit() override;

	UPtr<MyGLFWNativeUIWindow> _mainWin;
};

} // namespace sge