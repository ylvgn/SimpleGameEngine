#pragma once

#include "nuklear.h"

namespace sge {
namespace NuklearUI {

struct NuklearUI_Util {
	NuklearUI_Util() = delete;

	static enum nk_buttons	mouseButton(UIMouseEventButton v);
	static enum nk_keys		keyCode(UIKeyboardEventKeyCode v);

	template<typename T> inline
	static void convert(struct nk_color& o, const sge::ColorRGBA<T>& i) {
		o.r = static_cast<nk_byte>(i.r);
		o.g = static_cast<nk_byte>(i.g);
		o.b = static_cast<nk_byte>(i.b);
		o.a = static_cast<nk_byte>(i.a);
	}

	template<typename T> inline
	static void convert(struct nk_colorf& o, const sge::ColorRGBA<T>& i) {
		o.r = static_cast<nk_byte>(i.r);
		o.g = static_cast<nk_byte>(i.g);
		o.b = static_cast<nk_byte>(i.b);
		o.a = static_cast<nk_byte>(i.a);
	}

	template<typename T> inline
	static void convert(struct nk_vec2& o, const sge::Vec2<T>& i) {
		o.x = static_cast<float>(i.x);
		o.y = static_cast<float>(i.y);
	}

	template<typename T> inline
	static void convert(struct nk_rect& o, const sge::Rect2<T>& i) {
		o.x = static_cast<float>(i.x);
		o.y = static_cast<float>(i.y);
		o.w = static_cast<float>(i.w);
		o.h = static_cast<float>(i.h);
	}

	template<typename T> inline
	static nk_color toNKColor(const sge::ColorRGBA<T>& i) {
		nk_color o;
		convert(o, i);
		return o;
	}
	
	template<typename T> inline
		static nk_colorf toNKColorf(const sge::ColorRGBA<T>& i) {
		nk_colorf o;
		convert(o, i);
		return o;
	}

	template<typename T> inline
	static struct nk_vec2 toVec2(const sge::Vec2<T>& i) {
		struct nk_vec2 o;
		convert(o, i);
		return o;
	}

	template<typename T> inline
	static struct nk_rect toRect(const sge::Rect2<T> i) {
		struct nk_rect o;
		convert(o, i);
		return o;
	}
};

using Util = NuklearUI_Util;

extern nk_context* g_ctx;

void createContext();
void destroyContext();
void onUIMouseEvent(UIMouseEvent& ev);
void onUIKeyboardEvent(UIKeyboardEvent& ev);
void render(float width, float height, float displayWidth, float displayHeight);
void demo();

#if 0
#pragma mark ================= Helper ====================
#endif
struct UIInput : public NonCopyable {
	UIInput()  { if(g_ctx) ::nk_input_begin(g_ctx); }
	~UIInput() { if(g_ctx) ::nk_input_end(g_ctx); }
};

class Box : public NonCopyable {
public:
	static constexpr float kTitleHeight = 20.f;

	Box(StrView			str,
		const Rect2f&	bounds,
		nk_flags		flags = NK_WINDOW_MOVABLE | NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)
	{
		_isOpen = ::nk_begin(g_ctx, str.data(), Util::toRect(bounds), flags);
	}
	~Box() { ::nk_end(g_ctx); }

	bool isOpen() const { return _isOpen; }

private:
	bool _isOpen;
};

class Label : public NonCopyable {
public:
	Label(StrView str, nk_flags alignment = NK_TEXT_LEFT) {
		::nk_label(g_ctx, str.data(), alignment);
	}
};

class ButtonLabel : public NonCopyable {
public:
	ButtonLabel(StrView str) {
		_isOpen = ::nk_button_label(g_ctx, str.data());
	}

	bool isOpen() const { return _isOpen; }

private:
	bool _isOpen;
};

class LayoutRowStatic : public NonCopyable {
public:
	static constexpr float kItemWMargin  = 10.f;
	static constexpr float kItemHMargin = 5.f;

	LayoutRowStatic(float height, float itemWidth, int cols = 1) {
		::nk_layout_row_static(g_ctx, height, static_cast<int>(itemWidth), cols);
	}
};

class BoxStaticVerticalLayout : public NonCopyable {
public:
	using Layout = LayoutRowStatic;

	BoxStaticVerticalLayout(StrView str, Vec2f pos, Vec2f itemSize, size_t itemCount) {
		const float& itemW	= itemSize.x;
		const float& itemH	= itemSize.y;
		Rect2f bounds = {
			pos.x, pos.y,
			itemW + Layout::kItemWMargin,
			itemCount * (itemH + Layout::kItemHMargin) + Box::kTitleHeight
		};

		_box	= eastl::make_unique<Box>(str, bounds);
		_layout = eastl::make_unique<Layout>(itemH, itemW);
	}

private:
	UPtr<Box>		_box;
	UPtr<Layout>	_layout;
};

} // NuklearUI
} // sge