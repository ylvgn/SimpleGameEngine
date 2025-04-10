#include "NativeUIWindow_Base.h"
#include <sge_core/base/BitUtil.h>

#include "NativeUIScrollInfo_Base.h"

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

	onUIKeyboardEvent(ev);
}

void NativeUIWindow_Base::onUINativeScrollBarEvent(UIScrollBarEvent& ev) {
	onUIScrollBarEvent(ev);
}

void NativeUIWindow_Base::setWindowTitle(StrView title) {
	onSetNativeWindowTitle(title);
}

void NativeUIWindow_Base::setWorldPos(const Vec2& pos) {
	if (_worldRect.pos == pos) return;
	onSetNativeWorldPos(pos);
	onSetWorldPos(pos);
}

void NativeUIWindow_Base::setSize(const Vec2& size) {
	if (_worldRect.size == size) return;
	onSetNativeSize(size);
	onSetSize(size);
}

void NativeUIWindow_Base::setCursor(UIMouseCursor cursor) {
	onSetNativeCursor(cursor);
}

UPtr<NativeUIScrollInfo_Base> NativeUIWindow_Base::createScrollBar(NativeUIScrollInfo_CreateDesc& desc) {
	return onCreateScrollBar(desc);
}

}