#pragma once

#include "../MyCommon.h"

namespace sge {

#define GLFWKeyCode_ENUM_LIST(_E) \
	_E(None, = GLFW_KEY_UNKNOWN) \
	_E(Escape, = GLFW_KEY_ESCAPE) \
	_E(BackQuote, = GLFW_KEY_GRAVE_ACCENT) \
	_E(Tab, = GLFW_KEY_TAB) \
	_E(CapsLock, = GLFW_KEY_CAPS_LOCK) \
	\
	_E(Shift, = GLFW_KEY_LEFT_SHIFT)  /* special handle as left */ \
	_E(LShift, = GLFW_KEY_LEFT_SHIFT) \
	_E(RShift, = GLFW_KEY_RIGHT_SHIFT) \
	_E(Ctrl, = GLFW_KEY_LEFT_CONTROL) /* special handle as left */ \
	_E(LCtrl, = GLFW_KEY_LEFT_CONTROL) \
	_E(RCtrl, = GLFW_KEY_RIGHT_CONTROL) \
	_E(Cmd, = GLFW_KEY_LEFT_SUPER) /* special handle as left */ \
	_E(LCmd, = GLFW_KEY_LEFT_SUPER) \
	_E(RCmd, = GLFW_KEY_RIGHT_SUPER) \
	_E(Alt, = GLFW_KEY_LEFT_ALT) /* special handle as left */ \
	_E(LAlt, = GLFW_KEY_LEFT_ALT) \
	_E(RAlt, = GLFW_KEY_RIGHT_ALT) \
	\
	_E(F1, = GLFW_KEY_F1) \
	_E(F2, = GLFW_KEY_F2) \
	_E(F3, = GLFW_KEY_F3) \
	_E(F4, = GLFW_KEY_F4) \
	_E(F5, = GLFW_KEY_F5) \
	_E(F6, = GLFW_KEY_F6) \
	_E(F7, = GLFW_KEY_F7) \
	_E(F8, = GLFW_KEY_F8) \
	_E(F9, = GLFW_KEY_F9) \
	_E(F10, = GLFW_KEY_F10) \
	_E(F11, = GLFW_KEY_F11) \
	_E(F12, = GLFW_KEY_F12) \
	_E(F13, = GLFW_KEY_F13) \
	_E(F14, = GLFW_KEY_F14) \
	_E(F15, = GLFW_KEY_F15) \
	_E(F16, = GLFW_KEY_F16) \
	_E(F17, = GLFW_KEY_F17) \
	_E(F18, = GLFW_KEY_F18) \
	_E(F19, = GLFW_KEY_F19) \
	_E(F20, = GLFW_KEY_F20) \
	_E(F21, = GLFW_KEY_F21) \
	_E(F22, = GLFW_KEY_F22) \
	_E(F23, = GLFW_KEY_F23) \
	_E(F24, = GLFW_KEY_F24) \
	\
	_E(Alpha0, = GLFW_KEY_0) \
	_E(Alpha1, = GLFW_KEY_1) \
	_E(Alpha2, = GLFW_KEY_2) \
	_E(Alpha3, = GLFW_KEY_3) \
	_E(Alpha4, = GLFW_KEY_4) \
	_E(Alpha5, = GLFW_KEY_5) \
	_E(Alpha6, = GLFW_KEY_6) \
	_E(Alpha7, = GLFW_KEY_7) \
	_E(Alpha8, = GLFW_KEY_8) \
	_E(Alpha9, = GLFW_KEY_9) \
	\
	_E(A, = GLFW_KEY_A) \
	_E(B, = GLFW_KEY_B) \
	_E(C, = GLFW_KEY_C) \
	_E(D, = GLFW_KEY_D) \
	_E(E, = GLFW_KEY_E) \
	_E(F, = GLFW_KEY_F) \
	_E(G, = GLFW_KEY_G) \
	_E(H, = GLFW_KEY_H) \
	_E(I, = GLFW_KEY_I) \
	_E(J, = GLFW_KEY_J) \
	_E(K, = GLFW_KEY_K) \
	_E(L, = GLFW_KEY_L) \
	_E(M, = GLFW_KEY_M) \
	_E(N, = GLFW_KEY_N) \
	_E(O, = GLFW_KEY_O) \
	_E(P, = GLFW_KEY_P) \
	_E(Q, = GLFW_KEY_Q) \
	_E(R, = GLFW_KEY_R) \
	_E(S, = GLFW_KEY_S) \
	_E(T, = GLFW_KEY_T) \
	_E(U, = GLFW_KEY_U) \
	_E(V, = GLFW_KEY_V) \
	_E(W, = GLFW_KEY_W) \
	_E(X, = GLFW_KEY_X) \
	_E(Y, = GLFW_KEY_Y) \
	_E(Z, = GLFW_KEY_Z) \
	\
	_E(Hyphen, = GLFW_KEY_MINUS) \
	_E(Equals, = GLFW_KEY_EQUAL) \
	_E(Backspace, = GLFW_KEY_BACKSPACE) \
	_E(LeftBracket, = GLFW_KEY_LEFT_BRACKET) \
	_E(RightBracket, = GLFW_KEY_RIGHT_BRACKET) \
	_E(Backslash, = GLFW_KEY_BACKSLASH) \
	_E(Semicolon, = GLFW_KEY_SEMICOLON) \
	_E(Quote, = GLFW_KEY_APOSTROPHE) \
	_E(Enter, = GLFW_KEY_ENTER) \
	_E(Comma, = GLFW_KEY_COMMA) \
	_E(Period, = GLFW_KEY_PERIOD) \
	_E(Slash, = GLFW_KEY_SLASH) \
	_E(Space, = GLFW_KEY_SPACE) \
	\
	_E(Insert, = GLFW_KEY_INSERT) \
	_E(Delete, = GLFW_KEY_DELETE) \
	_E(Home, = GLFW_KEY_HOME) \
	_E(End, = GLFW_KEY_END) \
	_E(PageUp, = GLFW_KEY_PAGE_UP) \
	_E(PageDown, = GLFW_KEY_PAGE_DOWN) \
	\
	_E(UpArrow, = GLFW_KEY_UP) \
	_E(DownArrow, = GLFW_KEY_DOWN) \
	_E(LeftArrow, = GLFW_KEY_LEFT) \
	_E(RightArrow, = GLFW_KEY_RIGHT) \
	\
	_E(PrintScreen, = GLFW_KEY_PRINT_SCREEN) \
	_E(ScrollLock, = GLFW_KEY_SCROLL_LOCK) \
	_E(Pause, = GLFW_KEY_PAUSE) \
	_E(NumLock, = GLFW_KEY_NUM_LOCK) \
	_E(KeypadDivide, = GLFW_KEY_KP_DIVIDE) \
	_E(KeypadMultiply, = GLFW_KEY_KP_MULTIPLY) \
	_E(KeypadMinus, = GLFW_KEY_KP_SUBTRACT) \
	_E(KeypadPlus, = GLFW_KEY_KP_ADD) \
	_E(KeypadPeriod, = GLFW_KEY_KP_DECIMAL) \
	\
	_E(Keypad0, = GLFW_KEY_KP_0) \
	_E(Keypad1, = GLFW_KEY_KP_1) \
	_E(Keypad2, = GLFW_KEY_KP_2) \
	_E(Keypad3, = GLFW_KEY_KP_3) \
	_E(Keypad4, = GLFW_KEY_KP_4) \
	_E(Keypad5, = GLFW_KEY_KP_5) \
	_E(Keypad6, = GLFW_KEY_KP_6) \
	_E(Keypad7, = GLFW_KEY_KP_7) \
	_E(Keypad8, = GLFW_KEY_KP_8) \
	_E(Keypad9, = GLFW_KEY_KP_9) \
	\
	_E(KeypadEnter, = GLFW_KEY_KP_ENTER) /* special */ \
	_E(KeypadEqual, = GLFW_KEY_KP_EQUAL) /* special */ \
	_E(Menu, = GLFW_KEY_MENU) /* special */ \
	\
	_E(_End,) \
//----
SGE_ENUM_DECLARE(GLFWKeyCode, i32)
SGE_ENUM_ALL_OPERATOR(GLFWKeyCode)


#if 0
#pragma mark ========= MyGLFWUIKeyboardEvent ============
#endif
struct MyGLFWUIKeyboardEvent {
	using KeyCode = UIKeyboardEventKeyCode;

	MyGLFWUIKeyboardEvent(GLFWwindow* win);

	bool isUp			(KeyCode v)		const;
	bool isDown			(KeyCode v)		const;

	bool _glfwIsUp		(int glfwKey)	const;
	bool _glfwIsDown	(int glfwKey)	const;

private:
	int		_GLFW_KEY(KeyCode v) const;
	bool	_isState(int glfwKey, int expectedState) const;

	GLFWwindow* _win = nullptr;
}; // MyGLFWUIKeyboardEvent

} // namespace sge