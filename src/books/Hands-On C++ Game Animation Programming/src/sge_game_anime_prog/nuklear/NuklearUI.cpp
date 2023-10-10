#include "NuklearUI.h"

NK_API struct nk_context* nk_win32_init(void);
NK_API void nk_win32_shutdown(void);
NK_API void nk_input_motion(struct nk_context* ctx, int x, int y);
NK_API void nk_input_button(struct nk_context* ctx, enum nk_buttons id, int x, int y, int down);
NK_API void nk_input_key(struct nk_context* ctx, enum nk_keys key, int down);
NK_API void nk_win32_render(int width, int height, int display_width, int display_height);
NK_API void nk_demo();

namespace sge {
namespace NuklearUI {

nk_context* NuklearUI::g_ctx = nullptr;

float NuklearUI::scaleFactor = 1.5f;

void createContext() {
	SGE_ASSERT(g_ctx == nullptr);
	g_ctx = nk_win32_init();
}

void destroyContext() {
	if (g_ctx) {
		nk_win32_shutdown();
		g_ctx = nullptr;
	}
}

bool onUIMouseEvent(UIMouseEvent& ev) {
	using Button	= UIMouseEventButton;
	using Type		= UIMouseEventType;

	Vec2i pos = Vec2i::s_cast(ev.pos / NuklearUI::scaleFactor);

	switch (ev.type) {
		case Type::Move: {
			nk_input_motion(g_ctx, pos.x, pos.y);
		} break;
		case Type::Down: {
			nk_input_button(g_ctx, Util::mouseButton(ev.button), pos.x, pos.y, 1);
		} break;
		case Type::Up: {
			nk_input_button(g_ctx, Util::mouseButton(ev.button), pos.x, pos.y, 0);
		} break;
		case Type::Scroll: {
			nk_input_scroll(g_ctx, Util::toNKVec2(ev.scroll));
		} break;
	}

	return windowIsAnyHovered();
}

void onUIKeyboardEvent(UIKeyboardEvent& ev) {
	using Modifier	= UIEventModifier;
	using Type		= UIKeyCodeEventType;
	using Modifier	= UIEventModifier;
	using KeyCode	= UIKeyboardEventKeyCode;

	bool ctrl = BitUtil::hasAny(ev.modifier, Modifier::Ctrl);
	int  down = ev.isDown() ? 1 : 0;
	switch (ev.keyCode) {
		case KeyCode::LeftArrow: {
			if (ctrl) nk_input_key(g_ctx, NK_KEY_TEXT_WORD_LEFT, down);
			else nk_input_key(g_ctx, NK_KEY_LEFT, down);
			ev.keyCodesMap.erase(ev.keyCode);
		} break;
		case KeyCode::RightArrow: {
			if (ctrl) nk_input_key(g_ctx, NK_KEY_TEXT_WORD_RIGHT, down);
			else nk_input_key(g_ctx, NK_KEY_LEFT, down);
			ev.keyCodesMap.erase(ev.keyCode);
		} break;
		case KeyCode::Home: {
			nk_input_key(g_ctx, NK_KEY_TEXT_START, down);
			nk_input_key(g_ctx, NK_KEY_SCROLL_START, down);
			ev.keyCodesMap.erase(ev.keyCode);
		} break;
		case KeyCode::End: {
			nk_input_key(g_ctx, NK_KEY_TEXT_END, down);
			nk_input_key(g_ctx, NK_KEY_SCROLL_END, down);
			ev.keyCodesMap.erase(ev.keyCode);
		} break;

		case KeyCode::C: {
			if (ctrl) nk_input_key(g_ctx, NK_KEY_COPY, down);
		} break;
		case KeyCode::V: {
			if (ctrl) nk_input_key(g_ctx, NK_KEY_PASTE, down);
		} break;
		case KeyCode::X: {
			if (ctrl) nk_input_key(g_ctx, NK_KEY_CUT, down);
		} break;
		case KeyCode::Z: {
			if (ctrl) nk_input_key(g_ctx, NK_KEY_TEXT_UNDO, down);
		} break;
		case KeyCode::R: {
			if (ctrl) nk_input_key(g_ctx, NK_KEY_TEXT_REDO, down);
		} break;
	}

	if (ev.isChar()) {
		nk_input_unicode(g_ctx, ev.charCode);
	}

	for (auto& kv : ev.keyCodesMap) {
		const auto& keyCode = kv.first;
		const auto& type	= kv.second;

		switch (type) {
			case Type::Down: {
				nk_input_key(g_ctx, Util::keyCode(keyCode), 1);
			} break;
			case Type::Up: {
				nk_input_key(g_ctx, Util::keyCode(keyCode), 0);
			} break;
		}
	}
}

void render(const Vec2f& viewportSize) {
	nk_win32_render(static_cast<int>(viewportSize.x / NuklearUI::scaleFactor),
					static_cast<int>(viewportSize.y / NuklearUI::scaleFactor),
					static_cast<int>(viewportSize.x),
					static_cast<int>(viewportSize.y));
}

void demo()	{
	nk_demo();
}

enum nk_buttons NuklearUI_Util::mouseButton(UIMouseEventButton v) {
	using Button = UIMouseEventButton;

	switch (v) {
		case Button::Left:		return nk_buttons::NK_BUTTON_LEFT;
		case Button::Right:		return nk_buttons::NK_BUTTON_RIGHT;
		case Button::Middle:	return nk_buttons::NK_BUTTON_MIDDLE;
	}
	return nk_buttons::NK_BUTTON_LEFT;
}

enum nk_keys NuklearUI_Util::keyCode(UIKeyboardEventKeyCode v) {
	using KeyCode = UIKeyboardEventKeyCode;

	switch (v) {
		case KeyCode::Shift:		return nk_keys::NK_KEY_SHIFT;
		case KeyCode::Ctrl:			return nk_keys::NK_KEY_CTRL;
		case KeyCode::Delete:		return nk_keys::NK_KEY_DEL;
		case KeyCode::Enter:		return nk_keys::NK_KEY_ENTER;
		case KeyCode::Tab:			return nk_keys::NK_KEY_TAB;
		case KeyCode::UpArrow:		return nk_keys::NK_KEY_UP;
		case KeyCode::DownArrow:	return nk_keys::NK_KEY_DOWN;
		case KeyCode::LeftArrow:	return nk_keys::NK_KEY_LEFT;
		case KeyCode::RightArrow:	return nk_keys::NK_KEY_RIGHT;
		case KeyCode::Backspace:	return nk_keys::NK_KEY_BACKSPACE;
		case KeyCode::Home:			return nk_keys::NK_KEY_SCROLL_START;
		case KeyCode::End:			return nk_keys::NK_KEY_SCROLL_END;
		case KeyCode::PageDown:		return nk_keys::NK_KEY_SCROLL_DOWN;
		case KeyCode::PageUp:		return nk_keys::NK_KEY_SCROLL_UP;
	}
	return NK_KEY_NONE;
}
 
} // NuklearUI
} // sge