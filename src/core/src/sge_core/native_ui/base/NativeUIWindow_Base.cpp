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
	using KeyCode	= UIKeyboardEvent::KeyCode;
	using Type		= UIKeyboardEvent::Type;

	const auto& keyCode	= ev.keyCode;
	auto& curType		= ev.type;

	if (keyCode != KeyCode::None)
	{
		const auto& lastType = _keyCodesMap[keyCode];
		switch (curType)
		{
			case Type::Down: {
				// HoldDown
				if (BitUtil::hasAny(lastType, Type::Down) || BitUtil::hasAny(lastType, Type::HoldDown)) {
					curType = Type::HoldDown;
				}
				_keyCodesMap[keyCode] = curType;
			} break;
			case Type::Up: {
				_keyCodesMap[keyCode] = curType;
			} break;
		}
	}

	ev.keyCodesMap = _keyCodesMap;

	onUIKeyboardEvent(ev);
}

}