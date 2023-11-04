#pragma once

#include "nuklear.h"

namespace sge {
namespace NuklearUI {

extern nk_context* g_ctx;
extern float g_scaleFactor;

struct NuklearUI_Util {
	NuklearUI_Util() = delete;

	static enum nk_buttons	mouseButton(UIMouseEventButton v);
	static enum nk_keys		keyCode(UIKeyboardEventKeyCode v);

	template<typename T> inline
	static void convert(struct nk_color& o, const sge::ColorRGBA<T>& i) {
		o.r = static_cast<nk_byte>(i.r * 255);
		o.g = static_cast<nk_byte>(i.g * 255);
		o.b = static_cast<nk_byte>(i.b * 255);
		o.a = static_cast<nk_byte>(i.a * 255);
	}

	template<typename T> inline
	static void convert(struct nk_colorf& o, const sge::ColorRGBA<T>& i) {
		o.r = static_cast<nk_byte>(i.r * 255);
		o.g = static_cast<nk_byte>(i.g * 255);
		o.b = static_cast<nk_byte>(i.b * 255);
		o.a = static_cast<nk_byte>(i.a * 255);
	}

	template<typename T> inline
	static void convert(struct nk_vec2& o, const sge::Vec2<T>& i) {
		o.x = static_cast<float>(i.x);
		o.y = static_cast<float>(i.y);
	}

	template<typename T> inline
	static void convert(sge::Vec2<T>& o, const struct nk_vec2& i) {
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
	static void convertNKSize(Vec2<T>& o)		{ o /= NuklearUI::g_scaleFactor; }
	inline static void convertNKSize(float& o)	{ o /= NuklearUI::g_scaleFactor; }

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
	static struct nk_vec2 toNKVec2(const sge::Vec2<T>& i) {
		struct nk_vec2 o;
		convert(o, i);
		return o;
	}

	template<typename T> inline
	static struct nk_rect toNKRect(const sge::Rect2<T> i) {
		struct nk_rect o;
		convert(o, i);
		return o;
	}

	template<typename T> inline
	static sge::Vec2<T> toVec2(const struct nk_vec2& i) {
		sge::Vec2<T> o;
		convert(o, i);
		return o;
	}

	template<typename T> inline
	static sge::Vec2<T> toNKSize(const Vec2<T>& i)	{
		sge::Vec2<T> o(i);
		convertNKSize(o);
		return o;
	}

	inline static float toNKSize(const float& i) {
		float o = i;
		convertNKSize(o);
		return o;
	}
};

using Util = NuklearUI_Util;

void createContext();
void destroyContext();
bool onUIMouseEvent(UIMouseEvent& ev);
void onUIKeyboardEvent(UIKeyboardEvent& ev);
void render(const Vec2f& viewportSize);
void demo();

inline void WindowSetBounds  (StrView name, const Rect2f& bounds)	{ ::nk_window_set_bounds  (g_ctx, name.data(), Util::toNKRect(bounds)); }
inline void WindowSetPosition(StrView name, const Vec2f& pos)		{ ::nk_window_set_position(g_ctx, name.data(), Util::toNKVec2(pos)); }

inline bool windowIsAnyHovered()		{ return ::nk_window_is_any_hovered(g_ctx); }
inline sge::Vec2f windowGetPosition()	{ return Util::toVec2<float>(nk_window_get_position(g_ctx)); }
inline sge::Vec2f windowGetSize()		{ return Util::toVec2<float>(nk_window_get_size(g_ctx)); }

#if 0
#pragma mark ================= helper function ====================
#endif
inline void Label(StrView str, nk_flags alignment = NK_TEXT_LEFT) {
	::nk_label(g_ctx, str.data(), alignment);
}

inline bool SliderInt(int* value,
	int min_value = std::numeric_limits<int>::lowest(),
	int max_value = std::numeric_limits<int>::max(),
	int value_step = 0.1f)
{
	return ::nk_slider_int(g_ctx, min_value, value, max_value, value_step);
}

inline bool SliderFloat(float* value,
						float min_value = std::numeric_limits<float>::lowest(),
						float max_value = std::numeric_limits<float>::max(),
						float value_step = 0.001f)
{
	return ::nk_slider_float(g_ctx, min_value, value, max_value, value_step);
}

inline bool SliderFloat01(float* value, float value_step = 0.001f) {
	return SliderFloat(value, 0, 1, value_step);
}

inline void LayoutRowPush(float ratio_or_width) {
	::nk_layout_row_push(g_ctx, ratio_or_width);
}

inline int Progress(size_t* cur,
					size_t  max,
					int is_modifyable = NK_FIXED)
{
	return ::nk_progress(g_ctx, static_cast<nk_size*>(cur), static_cast<nk_size>(max), is_modifyable);
}

inline void LayoutRowDynamic(float height, int cols = 1) {
	::nk_layout_row_dynamic(g_ctx, height, cols);
}

class UIInput : public NonCopyable {
public:
	UIInput()	{ if (g_ctx) ::nk_input_begin(g_ctx);	}
	~UIInput()	{ if (g_ctx) ::nk_input_end(g_ctx);		}
};

class Window : public NonCopyable {
public:
	static constexpr const float kTitleHeight = 25.f;

	static constexpr const nk_flags kDefaultStyle   = NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR;
	static constexpr const nk_flags kMovableStyle   = kDefaultStyle   | NK_WINDOW_MOVABLE;
	static constexpr const nk_flags kMinizableStyle = kDefaultStyle   | NK_WINDOW_MINIMIZABLE;
	static constexpr const nk_flags kCloseableStyle = kDefaultStyle   | NK_WINDOW_CLOSABLE;
	static constexpr const nk_flags kScalableStyle  = kDefaultStyle   | NK_WINDOW_SCALABLE;

	Window(	const Rect2f& bounds,
			StrView title = "",
			nk_flags flags = kDefaultStyle)
	{
		auto rect = Util::toNKRect(bounds);
		rect.x /= g_scaleFactor;
		rect.y /= g_scaleFactor;
		if (flags & NK_WINDOW_TITLE) {
			rect.h += kTitleHeight;
		}
		_isOpen = ::nk_begin(g_ctx, title.data(), rect, flags);
	}
	~Window() { ::nk_end(g_ctx); }

	bool isOpen() const { return _isOpen; }

private:
	bool _isOpen : 1;
};

class ButtonLabel : public NonCopyable {
public:
	ButtonLabel(StrView str) {
		_isOpen = ::nk_button_label(g_ctx, str.data());
	}

	bool isOpen() const { return _isOpen; }

private:
	bool _isOpen : 1;
};

class LayoutRowStatic : public NonCopyable {
public:
	static constexpr const float kItemWMargin = 10.f;
	static constexpr const float kItemHMargin = 5.f;

	LayoutRowStatic(float height,
					float item_width,
					int cols = 1)
	{
		::nk_layout_row_static(g_ctx, height, static_cast<int>(item_width), cols);
	}
};

class LayoutRow : public NonCopyable {
public:
	LayoutRow(	enum nk_layout_format fmt,
				float height,
				int cols,
				Span<const float> ratio)
	{
		::nk_layout_row(g_ctx, fmt, height, cols, ratio.data());
	}
};

class LayoutRowBegin : public NonCopyable {
public:
	LayoutRowBegin(	enum nk_layout_format fmt,
					float row_height,
					int cols = 1)
	{
		::nk_layout_row_begin(g_ctx, fmt, row_height, cols);
	}
	~LayoutRowBegin() { ::nk_layout_row_end(g_ctx); }
};

class Combo : public NonCopyable {
public:
	Combo(	Span<const String> items,
			int cur_selected,
			int item_height,
			const Vec2f& size)
	{	
		int count = static_cast<int>(items.size());
		for (int i = 0; i < count; ++i) {
			_names[i] = items[i].data();
		}
		_selectedIndex = ::nk_combo(g_ctx, _names, count, cur_selected, item_height, Util::toNKVec2(size));
	}

	inline int selectedIndex() const { return _selectedIndex; }

private:
	int			_selectedIndex;
	const char* _names[200];
};

class CheckboxLabel : public NonCopyable {
public:
	CheckboxLabel(StrView label, bool isActive) :
		_isActive(static_cast<int>(isActive))
	{
		_isOpen = ::nk_checkbox_label(g_ctx, label.data(), &_isActive);
	}

	inline bool isOpen()	const { return _isOpen; }
	inline bool isActive()	const { return static_cast<bool>(_isActive); }

private:
	bool	_isOpen : 1;
	int		_isActive;
};

struct ScopedStyleTextColor : public NonCopyable {
	ScopedStyleTextColor(const Color4f& newColor)
		: _oldColor(g_ctx->style.text.color)
	{
		set(newColor);
	}

	ScopedStyleTextColor(const nk_color& newColor)
		: _oldColor(g_ctx->style.text.color)
	{
		set(newColor);
	}

	~ScopedStyleTextColor() { detect(); }

	inline void set(const nk_color& v) { g_ctx->style.text.color = v; }
	inline void set(const Color4f&  v) { set(Util::toNKColor(v)); }


	void detect() {
		if (_oldColor.has_value()) {
			g_ctx->style.text.color = _oldColor.value();
			_oldColor.reset();
		}
	}

private:
	Opt<nk_color> _oldColor;
};

} // NuklearUI
} // sge