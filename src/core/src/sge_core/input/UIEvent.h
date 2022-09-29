#pragma once

#include <sge_core/math/Rect2.h>

namespace sge {

enum class UIMouseEventType {
	None,
	Up,
	Down,
	Move,
	Scroll,
};
SGE_ENUM_ALL_OPERATOR(UIMouseEventType)

enum class UIMouseEventButton {
	None,
	Left	= 1 << 0,
	Middle	= 1 << 1,
	Right	= 1 << 2,
	Button4 = 1 << 3,
	Button5 = 1 << 4,
};
SGE_ENUM_ALL_OPERATOR(UIMouseEventButton)

enum class UIEventModifier {
	None,
	Shift	= 1 << 0,
	Ctrl	= 1 << 1,
	Alt		= 1 << 2,
	Cmd		= 1 << 3,
	Fn		= 1 << 4,
};
SGE_ENUM_ALL_OPERATOR(UIEventModifier)

enum class UIMouseCursorType {
	None,
	Arrow	 = 1 << 0,
	IBeam	 = 1 << 1,
	SizeAll	 = 1 << 2,
	SizeWE	 = 1 << 3,
	SizeNS	 = 1 << 4,
	SizeNESW = 1 << 5,
	SizeNWSE = 1 << 6,
	Hand	 = 1 << 7,
	No		 = 1 << 8,
};
SGE_ENUM_ALL_OPERATOR(UIMouseCursorType)

struct UIMouseEvent {
	using Type		= UIMouseEventType;
	using Modifier	= UIEventModifier;
	using Button	= UIMouseEventButton;
	using Cursor	= UIMouseCursorType;

	bool isUp() const		{ return type == Type::Up; }
	bool isDown() const		{ return type == Type::Down; }
	bool isMove() const		{ return type == Type::Move; }
	bool isScroll() const	{ return type == Type::Scroll; }

	bool isDragging() const { return type == Type::Move && pressedButtons != Button::None; }

	Type		type			= Type::None;
	Modifier	modifier		= Modifier::None;
	Button		button			= Button::None;
	Button		pressedButtons	= Button::None;
	Cursor      cursor			= Cursor::Arrow;

	Vec2f		pos{0,0};
	Vec2f		deltaPos{0,0};
	Vec2f		scroll{0,0};
};

#define UIKeyboardEventType_ENUM_LIST(_E) \
	_E(None,) \
	_E(Backspace,) \
	_E(Delete,) \
	_E(Tab,) \
	_E(Clear,) \
	_E(Enter,) \
	_E(Pause,) \
	_E(Escape,) \
	_E(Space,) \
	_E(Keypad0,) \
	_E(Keypad1,) \
	_E(Keypad2,) \
	_E(Keypad3,) \
	_E(Keypad4,) \
	_E(Keypad5,) \
	_E(Keypad6,) \
	_E(Keypad7,) \
	_E(Keypad8,) \
	_E(Keypad9,) \
	_E(KeypadPeriod,) \
	_E(KeypadDivide,) \
	_E(KeypadMultiply,) \
	_E(KeypadMinus,) \
	_E(KeypadPlus,) \
	_E(KeypadEnter,) \
	_E(KeypadEquals,) \
	_E(UpArrow,) \
	_E(DownArrow,) \
	_E(RightArrow,) \
	_E(LeftArrow,) \
	_E(Insert,) \
	_E(Home,) \
	_E(End,) \
	_E(PageUp,) \
	_E(PageDown,) \
	_E(F1,) \
	_E(F2,) \
	_E(F3,) \
	_E(F4,) \
	_E(F5,) \
	_E(F6,) \
	_E(F7,) \
	_E(F8,) \
	_E(F9,) \
	_E(F10,) \
	_E(F11,) \
	_E(F12,) \
	_E(F13,) \
	_E(F14,) \
	_E(F15,) \
	_E(Alpha0,) \
	_E(Alpha1,) \
	_E(Alpha2,) \
	_E(Alpha3,) \
	_E(Alpha4,) \
	_E(Alpha5,) \
	_E(Alpha6,) \
	_E(Alpha7,) \
	_E(Alpha8,) \
	_E(Alpha9,) \
	_E(Exclaim,) \
	_E(DoubleQuote,) \
	_E(Hash,) \
	_E(Dollar,) \
	_E(Percent,) \
	_E(Ampersand,) \
	_E(Quote,) \
	_E(LeftParen,) \
	_E(RightParen,) \
	_E(Asterisk,) \
	_E(Plus,) \
	_E(Comma,) \
	_E(Minus,) \
	_E(Period,) \
	_E(Slash,) \
	_E(Colon,) \
	_E(Semicolon,) \
	_E(Less,) \
	_E(Equals,) \
	_E(Greater,) \
	_E(Question,) \
	_E(At,) \
	_E(LeftBracket,) \
	_E(Backslash,) \
	_E(RightBracket,) \
	_E(Caret,) \
	_E(Underscore,) \
	_E(BackQuote,) \
	_E(A,) \
	_E(B,) \
	_E(C,) \
	_E(D,) \
	_E(E,) \
	_E(F,) \
	_E(G,) \
	_E(H,) \
	_E(I,) \
	_E(J,) \
	_E(K,) \
	_E(L,) \
	_E(M,) \
	_E(N,) \
	_E(O,) \
	_E(P,) \
	_E(Q,) \
	_E(R,) \
	_E(S,) \
	_E(T,) \
	_E(U,) \
	_E(V,) \
	_E(W,) \
	_E(X,) \
	_E(Y,) \
	_E(Z,) \
	_E(LeftCurlyBracket,) \
	_E(Pipe,) \
	_E(RightCurlyBracket,) \
	_E(Tilde,) \
	_E(Numlock,) \
	_E(CapsLock,) \
	_E(ScrollLock,) \
	_E(RightShift,) \
	_E(LeftShift,) \
	_E(RightCtrl,) \
	_E(LeftCtrl,) \
	_E(RightAlt,) \
	_E(LeftAlt,) \
	_E(LeftCommand,) \
	_E(_End,) \
//----
SGE_ENUM_CLASS(UIKeyboardEventType, u8)
SGE_ENUM_ALL_OPERATOR(UIKeyboardEventType)

enum class UIKeycodeStateType {
	None,
	Up,
	Down,
	Pressed,
};
SGE_ENUM_ALL_OPERATOR(UIKeycodeStateType)

struct UIKeyboardEvent {
	UIKeyboardEvent() {
		keyCodes.resize(enumInt(Type::_End));
	}

	using Type = UIKeyboardEventType;
	using State = UIKeycodeStateType;

	bool isUp(Type keyCode)			const { return keyCodes[enumInt(keyCode)] == State::Up; }
	bool isDown(Type keyCode)		const { return keyCodes[enumInt(keyCode)] == State::Down; }
	bool isPressed(Type keyCode)	const { return keyCodes[enumInt(keyCode)] == State::Pressed; }

	Vector<State, enumInt(Type::_End)> keyCodes;
};

}