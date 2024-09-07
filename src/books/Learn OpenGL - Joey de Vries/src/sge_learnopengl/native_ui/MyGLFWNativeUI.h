#pragma once

#include "../MyCommon.h"

namespace sge {
#if 0
#pragma mark ========= MyGLFWUIKeyboardEvent ============
#endif
class MyGLFWNativeUIWindow;
struct MyGLFWUIKeyboardEvent {
	using KeyCode	= UIKeyboardEventKeyCode;

	MyGLFWUIKeyboardEvent(MyGLFWNativeUIWindow* win) : _win(win) { SGE_ASSERT(win != nullptr); }

	bool isUp			(KeyCode k)		const;
	bool isDown			(KeyCode k)		const;
	bool _glfwIsUp		(int glfwKey)	const;
	bool _glfwIsDown	(int glfwKey)	const;

private:
	const MyGLFWNativeUIWindow* _win = nullptr;
};

#if 0
#pragma mark ========= MyGLFWNativeUIWindow ============
#endif
class MyGLFWNativeUIWindow : public NativeUIWindow_Base {
	using Base = NativeUIWindow_Base;
	using This = MyGLFWNativeUIWindow;
public:
	using UIKeyboardEvent = MyGLFWUIKeyboardEvent;

	static constexpr int kKeyCodeCount = GLFW_KEY_LAST + 1;

	struct CreateDesc : Base::CreateDesc {
		CreateDesc()
			: major(3)
			, minor(3)
			, profile(Profile::Core)
		{}

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

	bool getKeyIsPress		(KeyCode v) const;
	bool getKeyIsRelease	(KeyCode v) const;
	bool getKeyIsRepeat		(KeyCode v) const;
	bool getKeyIsDown		(KeyCode v) const { return getKeyIsPress(v) || getKeyIsRepeat(v); }

	bool _glfwWindowShouldClose()							{ return glfwWindowShouldClose(_glfwWin); }
	void _glfwGetFramebufferSize(int& width, int& height)	{ glfwGetFramebufferSize(_glfwWin, &width, &height); }
	void _glfwGetWindowPos(int& xpos, int& ypos)			{ glfwGetWindowPos(_glfwWin, &xpos, &ypos); }
	void _glfwSetWindowShouldClose(int isClose)				{ glfwSetWindowShouldClose(_glfwWin, isClose); }

friend struct MyGLFWUIKeyboardEvent;
protected:
	virtual void onSetWindowTitle	(StrView title)		override { _glfwSetWindowTitle(title); }
	virtual void onSetWindowPos		(const Vec2f& pos)	override;
	virtual void onSetWindowSize	(const Vec2f& size) override;

	bool _getKeyIsPress		(int v) const;
	bool _getKeyIsRelease	(int v) const;
	bool _getKeyIsRepeat	(int v) const;
	bool _getKeyIsDown		(int v) const { return _getKeyIsPress(v) || _getKeyIsRepeat(v); }

	virtual void onCreate(CreateDesc& desc);
	virtual void onRender() {}
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) {}

	virtual void onFramebufferSizeCallback(int width, int height) {}
	virtual void onWindowPosCallback(int xpos, int ypos) {}

	GLFWwindow* _glfwWin = nullptr;

private:
	virtual UPtr<NativeUIScrollInfo_Base> onCreateScrollBar(NativeUIScrollInfo_CreateDesc& desc) { return nullptr; }; // TODO

	void _glfwSetWindowTitle(StrView title);

	void _handleNativeInputEvent();
	void _handleNativeUIKeyboardEvent();

	virtual void onUINativeKeyboardEvent(UIKeyboardEvent& ev) { onUIKeyboardEvent(ev); }

	int		_toGLFWKeyCode(KeyCode v) const;
	bool	_getKeyCodeState(int glfwKeyCode, int state) const;

	static void s_glfwSetFramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void s_glfwSetWindowPosCallback(GLFWwindow* window, int xpos, int ypos);

	SGE_INLINE static This* s_getThis(GLFWwindow* window) {
		return reinterpret_cast<This*>(glfwGetWindowUserPointer(window));
	}
};

#if 0
#pragma mark ========= MyGLFWNativeUIApp ============
#endif
class MyGLFWNativeUIApp : public NativeUIApp_Base {
	using Base = NativeUIApp_Base;
public:
	MyGLFWNativeUIApp() noexcept	{ SGE_ASSERT(_current == nullptr); _current = this; }
	~MyGLFWNativeUIApp() noexcept	{ _current = nullptr; }

	static MyGLFWNativeUIApp* current() { return _current; }
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onRun() override;
	virtual void onQuit() override;

	UPtr<MyGLFWNativeUIWindow> _mainWin;

private:
	static MyGLFWNativeUIApp* _current;

};

}