#pragma once

#include <sge_core/math/Rect2.h>
#include <sge_core/input/UIEvent.h>

namespace sge {

class NativeUIScrollInfo_Base;

class NativeUIWindow_Base : public NonCopyable {
public:
	using KeyCode		= UIKeyboardEventKeyCode;
	using KeyCodeState	= UIKeyCodeEventType;
	using ScrollInfo	= NativeUIScrollInfo_Base;

	static const int kKeyCodeCount = 256;

	struct CreateDesc {
		CreateDesc()
			: titleBar(true)
			, isMainWindow(false)
			, visible(true)
			, resizable(true)
			, closeButton(true)
			, minButton(true)
			, maxButton(true)
			, centerToScreen(true)
			, alwaysOnTop(false)
			, vScrollBar(false)
			, hScrollBar(false)
			, ownDC(false)
		{}

		enum class Type {
			None,
			NormalWindow,
			ToolWindow,
			PopupWindow,
		};

		Type		type = Type::NormalWindow;
		Rect2f		rect {10, 10, 640, 480};

		bool		titleBar		: 1;
		bool		isMainWindow	: 1;
		bool		visible			: 1;
		bool		resizable		: 1;
		bool		closeButton		: 1;
		bool		minButton		: 1;
		bool		maxButton		: 1;
		bool		centerToScreen	: 1;
		bool		alwaysOnTop		: 1;
		bool		vScrollBar		: 1;
		bool		hScrollBar		: 1;
		bool		ownDC			: 1;
	};

	void create			(CreateDesc& desc)		{ onCreate(desc); }
	void setWindowTitle	(StrView title)			{ onSetWindowTitle(title); }
	void setWindowSize	(const Rect2i& xywh)	{ onSetWindowSize(xywh); }
	void setCursor		(UIMouseCursor cursor)	{ onSetCursor(cursor); }
	void scrollWindow	(const Vec2i& delta)	{ onScrollWindow(delta); }

	void drawNeeded() { onDrawNeeded(); }

	const Rect2f& clientRect() const { return _clientRect; }

	virtual void onCloseButton() {}
	virtual void onActive(bool isActive) {}
	virtual void onDraw() {}

	virtual void onUINativeMouseEvent(UIMouseEvent& ev);
	virtual void onUIMouseEvent(UIMouseEvent& ev) {}

	virtual void onUINativeKeyboardEvent(UIKeyboardEvent& ev);
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) {}

	virtual void onUINativeScrollBarEvent(UIScrollBarEvent& ev);
	virtual void onUIScrollBarEvent(UIScrollBarEvent& ev) {}

	ScrollInfo* createScrollBar() { return onCreateScrollBar(); }

protected:
	virtual void onCreate(CreateDesc& desc) {}
	virtual void onSetWindowTitle(StrView title) {}
	virtual void onSetWindowSize(const Rect2i& xywh) {};
	virtual void onClientRectChanged(const Rect2f& rc) { _clientRect = rc; }
	virtual void onDrawNeeded() {}
	virtual void onSetCursor(UIMouseCursor type) {}
	virtual void onScrollWindow(const Vec2i& delta) {}

	virtual ScrollInfo* onCreateScrollBar() = 0;

	Rect2f	_clientRect {0,0,0,0};

	UIMouseEventButton	_pressedMouseButtons = UIMouseEventButton::None;
	Vec2f				_mousePos{0,0};
	
	Vector<UIKeyboardEvent::Type, kKeyCodeCount> _pressedKeyCodes;

	Vec2i _scrollBarPos{0,0};
};

}