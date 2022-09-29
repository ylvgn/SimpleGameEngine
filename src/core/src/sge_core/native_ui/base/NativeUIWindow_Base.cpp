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

void NativeUIWindow_Base::onUINativeMouseCursor(UIMouseEvent& ev) {
	onUIMouseCursor(ev);
	setCursor(ev.cursor);
}

void NativeUIWindow_Base::onUINativeKeyboardEvent(UIKeyboardEvent& ev) {
	using Type = UIKeyboardEvent::Type;
	using State = UIKeyboardEvent::State;

	for (auto& k : _pressedkeyCodes) {
		if (ev.isUp(k)) {
			_pressedkeyCodes.erase(k);
		} else {
			ev.keyCodes[enumInt(k)] = State::Pressed;
		}
	}

	for (Type k = Type::None; k != Type::_End; k += 1) {
		if (_pressedkeyCodes.count(k) != 0) continue;
		if (ev.keyCodes[enumInt(k)] == State::Down) _pressedkeyCodes.emplace(k);
	}

	onUIKeyboardEvent(ev);
}

}