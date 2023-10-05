#pragma once

#include <sge_core/math/Rect2.h>
#include <sge_core/base/BitUtil.h>

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
	LShift	= 1 << 5,
	RShift	= 1 << 6,
	LCtrl	= 1 << 7,
	RCtrl	= 1 << 8,
	LAlt	= 1 << 9,
	RAlt	= 1 << 10,
	LCmd	= 1 << 11,
	RCmd	= 1 << 12,
};
SGE_ENUM_ALL_OPERATOR(UIEventModifier)

enum class UIMouseCursor {
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
SGE_ENUM_ALL_OPERATOR(UIMouseCursor)

struct UIMouseEvent {
	using Type		= UIMouseEventType;
	using Modifier	= UIEventModifier;
	using Button	= UIMouseEventButton;

	bool isUp()		const	{ return type == Type::Up; }
	bool isDown()	const	{ return type == Type::Down; }
	bool isMove()	const	{ return type == Type::Move; }
	bool isScroll() const	{ return type == Type::Scroll; }

	bool isDragging() const { return type == Type::Move && pressedButtons != Button::None; }

	Type		type			= Type::None;
	Modifier	modifier		= Modifier::None;
	Button		button			= Button::None;
	Button		pressedButtons	= Button::None;

	Vec2f		pos{0,0};
	Vec2f		deltaPos{0,0};
	Vec2f		scroll{0,0};
};

#define UIKeyboardEventKeyCode_ENUM_LIST(_E) \
	_E(None,) \
	_E(Space,) \
	_E(Escape,) \
	_E(BackQuote,) \
	_E(Tab,) \
	_E(CapsLock,) \
	\
	_E(Shift,)	_E(LShift,)	_E(RShift,)	\
	_E(Ctrl,)	_E(LCtrl,)	_E(RCtrl,) \
	_E(Alt,)	_E(LAlt,)	_E(RAlt,) \
	_E(Cmd,)	_E(LCmd,)	_E(RCmd,) \
	_E(Menu,) \
	\
	_E(F1,) _E(F2,) _E(F3,) _E(F4,)  _E(F5,)  _E(F6,) \
	_E(F7,) _E(F8,) _E(F9,) _E(F10,) _E(F11,) _E(F12,) \
	\
	_E(Alpha0,) _E(Alpha1,) _E(Alpha2,) _E(Alpha3,) _E(Alpha4,) \
	_E(Alpha5,) _E(Alpha6,) _E(Alpha7,) _E(Alpha8,) _E(Alpha9,) \
	\
	_E(A,) _E(B,) _E(C,) _E(D,) _E(E,) _E(F,) _E(G,) _E(H,) _E(I,) _E(J,) _E(K,) _E(L,) _E(M,) \
	_E(N,) _E(O,) _E(P,) _E(Q,) _E(R,) _E(S,) _E(T,) _E(U,) _E(V,) _E(W,) _E(X,) _E(Y,) _E(Z,) \
	\
	_E(Hyphen,)			_E(Equals,)			_E(Backspace,) \
	_E(LeftBracket,)	_E(RightBracket,)	_E(Backslash,) \
	_E(Semicolon,)		_E(Quote,)			_E(Enter,) \
	_E(Comma,)			_E(Period,)			_E(Slash,) \
	\
	_E(Insert,)	 _E(Delete,) \
	_E(Home,)	 _E(End,) \
	_E(PageUp,)	 _E(PageDown,) \
	\
	_E(UpArrow,) _E(DownArrow,) _E(LeftArrow,) _E(RightArrow,) \
	\
	_E(PrintScreen,) _E(ScrollLock,) _E(Pause,) \
	_E(NumLock,) _E(KeypadDivide,) _E(KeypadMultiply,) _E(KeypadMinus,) _E(KeypadPlus,) _E(KeypadPeriod,) \
	_E(Keypad0,) _E(Keypad1,) _E(Keypad2,) _E(Keypad3,) _E(Keypad4,) \
	_E(Keypad5,) _E(Keypad6,) _E(Keypad7,) _E(Keypad8,) _E(Keypad9,) \
	_E(_End,) \
//----
SGE_ENUM_CLASS(UIKeyboardEventKeyCode, u32)

#define UIKeyCodeEventType_ENUM_LIST(_E) \
	_E(None,) \
	_E(Up,) \
	_E(Down,) \
	_E(Char,) \
	_E(HoldDown,) \
// ----
SGE_ENUM_CLASS(UIKeyCodeEventType, u8)
SGE_ENUM_ALL_OPERATOR(UIKeyCodeEventType)

struct UIKeyboardEvent {
	using KeyCode		= UIKeyboardEventKeyCode;
	using Type			= UIKeyCodeEventType;
	using Modifier		= UIEventModifier;

	bool isUp(KeyCode k)			const { return getType(k) == Type::Up; }
	bool isDown(KeyCode k)			const { return getType(k) == Type::Down; }
	bool isHoldDown(KeyCode k)		const { return getType(k) == Type::HoldDown; }
	bool isChar(KeyCode k)			const { return getType(k) == Type::Char; }
	bool isDownWhatever(KeyCode k)	const { return isHoldDown(k) || isDown(k); }

	bool isUp()						const { return type == Type::Up; }
	bool isDown()					const { return type == Type::Down; }
	bool isHoldDown()				const { return type == Type::HoldDown; }
	bool isChar()					const { return type == Type::Char; }
	bool isDownWhatever()			const { return isHoldDown() || isDown(); }

	StrView data()					const { return charCodeStr; }

	Type getType(KeyCode k) const {
		auto it = keyCodesMap.find(k);
		return it == keyCodesMap.end() ? Type::None : it->second;
	}

	bool isModifierKey()				const { return modifier != Modifier::None; }
	bool hasAnyModifierKey(Modifier k)	const { return BitUtil::hasAny(modifier, k); }

	Type getModifierKeyType(Modifier k) const {
		switch (k) {
			case Modifier::Shift:	return getType(KeyCode::Shift);
			case Modifier::Ctrl:	return getType(KeyCode::Ctrl);
			case Modifier::Alt:		return getType(KeyCode::Alt);
			case Modifier::Cmd:		return getType(KeyCode::Cmd);
			case Modifier::LShift:	return getType(KeyCode::LShift);
			case Modifier::RShift:	return getType(KeyCode::RShift);
			case Modifier::LCtrl:	return getType(KeyCode::LCtrl);
			case Modifier::RCtrl:	return getType(KeyCode::RCtrl);
			case Modifier::LAlt:	return getType(KeyCode::LAlt);
			case Modifier::RAlt:	return getType(KeyCode::RAlt);
		}
		return Type::None;
	}

	bool IsModifierKeyDown(KeyCode k) const {
		switch (k) {
			case KeyCode::Ctrl:
			case KeyCode::LCtrl:
			case KeyCode::RCtrl:
			case KeyCode::Shift:
			case KeyCode::LShift:
			case KeyCode::RShift:
			case KeyCode::Alt:
			case KeyCode::LAlt:
			case KeyCode::RAlt:
			case KeyCode::Cmd:
			case KeyCode::LCmd:
			case KeyCode::RCmd: {
				return isDownWhatever(k);
			} break;
			default: throw SGE_ERROR("not support {}", k);
		}
		return false;
	}

	Type				type	 = Type::None;
	KeyCode				keyCode  = KeyCode::None;
	Modifier			modifier = Modifier::None;
	u32					charCode = 0;
	String				charCodeStr;

	Map<KeyCode, Type>	keyCodesMap;
};

}