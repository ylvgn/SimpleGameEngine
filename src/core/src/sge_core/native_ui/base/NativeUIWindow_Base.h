#pragma once

#include <sge_core/math/Rect2.h>
#include <sge_core/input/UIEvent.h>

namespace sge {

class NativeUIScrollInfo_Base;
struct NativeUIScrollInfo_CreateDesc;

class NativeUIWindow_Base : public NonCopyable {
public:
	using Vec2			= Vec2f;
	using Rect2			= Rect2f;
	using KeyCode		= UIKeyboardEventKeyCode;
	using KeyCodeState	= UIKeyCodeEventType;

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
		bool		ownDC			: 1; // TODO need removed (only need it when init OpenGL)
	};

			void create	(CreateDesc& desc)		{ onCreate(desc); }

			void setWindowTitle	(StrView title);
	virtual void onSetNativeWindowTitle(StrView title) = 0;

			void setWorldPos(const Vec2& pos);
			void setWorldPos(float x, float y)	{ setWorldPos(Vec2(x, y)); }
	virtual void onSetNativeWorldPos(const Vec2& pos) = 0;
	virtual void onSetWorldPos(const Vec2& pos) { _worldRect.pos = pos; }

			void setSize(const Vec2& size);
			void setSize(float w, float h)		{ setSize(Vec2(w, h)); }
	virtual void onSetSize(const Vec2& size)	{ _worldRect.size = size; }
	virtual void onSetNativeSize(const Vec2& size) = 0;

	void setWorldRect(const Rect2& rect)		{ setWorldPos(rect.pos); setSize(rect.size); }

			void setCursor(UIMouseCursor cursor);
	virtual void onSetNativeCursor(UIMouseCursor type) = 0;

	void scrollWindow (const Vec2i& delta)	{ onScrollWindow(delta); }

	void drawNeeded() { onDrawNeeded(); }

	const Rect2f& clientRect() const { return _clientRect; } // TODO may no need to store, use Rect2f(GetClientRect()) instead.

	virtual void onCloseButton() {}
	virtual void onActive(bool isActive) {}
	virtual void onDraw() {}

	virtual void onUINativeMouseEvent(UIMouseEvent& ev);
	virtual void onUIMouseEvent(UIMouseEvent& ev) {}

	virtual void onUINativeKeyboardEvent(UIKeyboardEvent& ev);
	virtual void onUIKeyboardEvent(UIKeyboardEvent& ev) {}

	virtual void onUINativeScrollBarEvent(UIScrollBarEvent& ev);
	virtual void onUIScrollBarEvent(UIScrollBarEvent& ev) {}

	UPtr<NativeUIScrollInfo_Base> createScrollBar(NativeUIScrollInfo_CreateDesc& desc);

protected:
	virtual void onCreate(CreateDesc& desc) {}
	virtual void onClientRectChanged(const Rect2f& rc)	{ _clientRect = rc; }
	virtual void onDrawNeeded() {}
	virtual void onScrollWindow(const Vec2i& delta) {}

	virtual UPtr<NativeUIScrollInfo_Base> onCreateScrollBar(NativeUIScrollInfo_CreateDesc& desc) = 0;

	Rect2	_worldRect  {0};
	Rect2	_clientRect {0};

	UIMouseEventButton	_pressedMouseButtons = UIMouseEventButton::None;
	Vec2f				_mousePos {0,0};
	
	Vector<UIKeyboardEvent::Type, kKeyCodeCount> _pressedKeyCodes;
};

}