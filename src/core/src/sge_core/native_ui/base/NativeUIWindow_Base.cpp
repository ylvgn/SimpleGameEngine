#include "NativeUIWindow_Base.h"
#include <sge_core/base/BitUtil.h>

namespace sge {

void NativeUIWindow_Base::onUINativeMouseEvent(UIMouseEvent& ev) {
	using Button = UIMouseEventButton;
	using Type   = UIMouseEventType;

	switch (ev.type) {
		case Type::Down: { BitUtil::set(  _pressedMouseButtons, ev.button); } break;
		case Type::Up:   { BitUtil::unset(_pressedMouseButtons, ev.button); } break;
	}

	ev.pressedButtons = _pressedMouseButtons;

	ev.deltaPos = ev.pos - _mousePos;
	_mousePos = ev.pos;

	onUIMouseEvent(ev);
}

void NativeUIWindow_Base::onUINativeKeyboardEvent(UIKeyboardEvent& ev) {
	using KeyCode		= UIKeyboardEvent::KeyCode;
	using Type			= UIKeyboardEvent::Type;
	using Modifier		= UIKeyboardEvent::Modifier;

	const auto& curKeyCode	= ev.keyCode;
	auto& curType			= ev.type;
	const auto& modifier	= ev.modifier;

	if (curKeyCode != KeyCode::None) {

	#define E(T) \
		if (_keyCodesMap[KeyCode::T] == Type::Down) { \
			if (!BitUtil::hasAny(modifier, Modifier::T)) { \
				_keyCodesMap[KeyCode::T] = Type::Up; \
			} \
		} \
	//---- Modifier Key Down -> Up
		E(LCtrl)
		E(LShift)
		E(LAlt)
		E(RCtrl)
		E(RShift)
		E(RAlt)
	#undef E

		if (ev.isModifierKey()) {
			switch (curKeyCode) {
				case KeyCode::Ctrl: {
					if (BitUtil::hasAny(modifier, Modifier::LCtrl)) _keyCodesMap[KeyCode::LCtrl] = Type::Down;
					if (BitUtil::hasAny(modifier, Modifier::RCtrl)) _keyCodesMap[KeyCode::RCtrl] = Type::Down;
				}break;
				case KeyCode::Shift: {
					if (BitUtil::hasAny(modifier, Modifier::LShift)) _keyCodesMap[KeyCode::LShift] = Type::Down;
					if (BitUtil::hasAny(modifier, Modifier::RShift)) _keyCodesMap[KeyCode::RShift] = Type::Down;
				}break;
				case KeyCode::Alt: {
					if (BitUtil::hasAny(modifier, Modifier::LAlt)) _keyCodesMap[KeyCode::LAlt] = Type::Down;
					if (BitUtil::hasAny(modifier, Modifier::RAlt)) _keyCodesMap[KeyCode::RAlt] = Type::Down;
				}break;
			}
		}

		const auto& lastType = _keyCodesMap[curKeyCode];
		switch (curType)
		{
			case Type::Down: {
				// HoldDown
				if (BitUtil::hasAny(lastType, Type::Down) || BitUtil::hasAny(lastType, Type::HoldDown)) {
					curType = Type::HoldDown;
				}
				_keyCodesMap[curKeyCode] = curType;
			} break;
			case Type::Up: {
				_keyCodesMap[curKeyCode] = curType;
			} break;
		}
	}

	ev.keyCodesMap = _keyCodesMap;

	onUIKeyboardEvent(ev);

	// reset Up -> None
	for (auto& kv : _keyCodesMap) {
		if (kv.second == Type::Up) {
			_keyCodesMap[kv.first] = Type::None;
		}
	}
}

}