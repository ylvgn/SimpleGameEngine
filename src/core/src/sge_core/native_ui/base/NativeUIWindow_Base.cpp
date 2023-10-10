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

	if (ev.keyCode != KeyCode::None) {

	#define E(SGE_T) \
		if (_keyCodesMap[KeyCode::SGE_T] == Type::Down) { \
			if (!ev.hasAnyModifierKey(Modifier::SGE_T)) { \
				_keyCodesMap[KeyCode::SGE_T] = Type::Up; \
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
			#define E(SGE_T) \
				case KeyCode::SGE_T: { \
					if (ev.hasAnyModifierKey(Modifier::L##SGE_T)) _keyCodesMap[KeyCode::L##SGE_T] = Type::Down; \
					if (ev.hasAnyModifierKey(Modifier::R##SGE_T)) _keyCodesMap[KeyCode::R##SGE_T] = Type::Down; \
				} break; \
			// ----
			switch (ev.keyCode) {
				E(Ctrl)
				E(Shift)
				E(Alt)
				E(Cmd)
			}
			#undef E
		}

		switch (ev.type)
		{
			case Type::Down: { _keyCodesMap[ev.keyCode] = ev.type; } break;
			case Type::Up:	 { _keyCodesMap[ev.keyCode] = ev.type; } break;
		}
	}

	ev.keyCodesMap = _keyCodesMap;
	if (ev.type == Type::Char || ev.type == Type::Up) {
		onUIKeyboardEvent(ev);
	} else if (!ev.IsCharKeyDown(ev.keyCode)) {
		onUIKeyboardEvent(ev);
	}

	// reset Up -> None
	for (auto& kv : _keyCodesMap) {
		if (kv.second == Type::Up) {
			_keyCodesMap[kv.first] = Type::None;
		}
	}
}

}