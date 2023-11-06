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
		switch (ev.type) {
			case Type::Down:	{ _keyCodesMap[ev.keyCode] = ev.type; } break;
			case Type::Up:		{ _keyCodesMap[ev.keyCode] = ev.type; } break;
		}
	}

	if (ev.type == Type::Char && _keyCode != KeyCode::None) {
		_keyCodesMap[_keyCode] = ev.type;
	}

	ev.pressedKeyCodes = _keyCodesMap;
	_keyCode = ev.keyCode;

	onUIKeyboardEvent(ev);

	// reset
	for (auto& kv : _keyCodesMap) {
		if (kv.second == Type::Up || ev.type == Type::Char) {
			_keyCodesMap[kv.first] = Type::None;
		}
	}
}

}