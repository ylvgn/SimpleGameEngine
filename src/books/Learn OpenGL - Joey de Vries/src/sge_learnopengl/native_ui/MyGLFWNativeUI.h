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
	using KeyCode			= UIKeyboardEventKeyCode;
	using UIKeyboardEvent	= MyGLFWUIKeyboardEvent;

	static constexpr int kKeyCodeCount = GLFW_KEY_LAST + 1;

	struct CreateDesc : Base::CreateDesc {
		enum class Profile : int {
			None	= GLFW_OPENGL_ANY_PROFILE,
			Core	= GLFW_OPENGL_CORE_PROFILE,
			Compat	= GLFW_OPENGL_COMPAT_PROFILE,
		};

		int		major	= -1;
		int		minor	= -1;
		Profile profile = Profile::None;
	};

	~MyGLFWNativeUIWindow();

	void create(CreateDesc& desc) { onCreate(desc); }; // !!<---- function overload
	void update(float dt);

	bool _glfwWindowShouldClose()							{ return glfwWindowShouldClose(_glfwWin); }
	void _glfwGetFramebufferSize(int& width, int& height)	{ glfwGetFramebufferSize(_glfwWin, &width, &height); }
	void _glfwGetWindowPos(int& xpos, int& ypos)			{ glfwGetWindowPos(_glfwWin, &xpos, &ypos); }
	void _glfwSetWindowShouldClose(int isClose)				{ glfwSetWindowShouldClose(_glfwWin, isClose); }

protected:
	virtual void onCloseButton		()					override { _glfwSetWindowShouldClose(true); }
	virtual void onSetWindowTitle	(StrView title)		override { _glfwSetWindowTitle(title); }
	virtual void onSetWindowPos		(const Vec2f& pos)	override { glfw::setWindowPos(_glfwWin, pos); }
	virtual void onSetWindowSize	(const Vec2f& size) override { glfw::setWindowSize(_glfwWin, size); }

	virtual void onCreate(CreateDesc& desc);				// !!<---- function overload
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) {}  // !!<---- function overload

	virtual void onFramebufferSizeCallback(int width, int height) {}
	virtual void onWindowPosCallback(int xpos, int ypos) {}

	virtual void onRender() {}

private:
	virtual UPtr<NativeUIScrollInfo_Base> onCreateScrollBar(NativeUIScrollInfo_CreateDesc& desc) { return nullptr; }; // TODO need to removed

	void _glfwSetWindowTitle(StrView title) { glfw::setWindowTitle(_glfwWin, title); }

	void _handleNativeInputEvent();
	void _handleNativeUIKeyboardEvent();

	static void s_glfwSetFramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void s_glfwSetWindowPosCallback(GLFWwindow* window, int xpos, int ypos);

	SGE_INLINE static This* s_getThis(GLFWwindow* window) {
		return reinterpret_cast<This*>(glfwGetWindowUserPointer(window));
	}

	GLFWwindow* _glfwWin = nullptr;
};

#if 0
#pragma mark ========= MyGLFWNativeUIApp ============
#endif
class MyGLFWNativeUIApp : public NativeUIApp {
	using Base = NativeUIApp;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onRun()					override;
	virtual void onUpdate(float dt)			override;
	virtual void onQuit()					override;

	UPtr<MyGLFWNativeUIWindow> _mainWin;
};

} // namespace sge