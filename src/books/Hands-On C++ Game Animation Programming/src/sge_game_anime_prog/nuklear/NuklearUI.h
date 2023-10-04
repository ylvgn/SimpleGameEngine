#pragma once

#include "nuklear.h"

namespace sge {
namespace NuklearUI {

static nk_context* s_ctx = nullptr;

void createContext();
void destroyContext();

void render(int width,
	int height,
	int displayWidth,
	int displayHeight);

void handleWindowEvent(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam,
	float scaleFactor);

void demo();

struct InputMouseAndKeyBoard : public NonCopyable {
	InputMouseAndKeyBoard()  { ::nk_input_begin(s_ctx); }
	~InputMouseAndKeyBoard() { ::nk_input_end  (s_ctx);	}
};

struct Box : public NonCopyable {
	~Box() { ::nk_end(s_ctx); }

	Box(StrView title,
		struct nk_rect bounds,
		nk_flags flags)
	{
		::nk_begin(s_ctx, title.data(), bounds, flags);
	}
};

struct Label : public NonCopyable {
	Label(StrView str, nk_flags alignment) {
		::nk_label(s_ctx, str.data(), alignment);
	}
};


template<typename T> inline
static void convert(nk_color& o, const sge::ColorRGBA<T>& i) {
	o.r = static_cast<nk_byte>(i.r);
	o.g = static_cast<nk_byte>(i.g);
	o.b = static_cast<nk_byte>(i.b);
	o.a = static_cast<nk_byte>(i.a);
}

template<typename T> inline
	static void convert(nk_colorf& o, const sge::ColorRGBA<T>& i) {
	o.r = static_cast<nk_byte>(i.r);
	o.g = static_cast<nk_byte>(i.g);
	o.b = static_cast<nk_byte>(i.b);
	o.a = static_cast<nk_byte>(i.a);
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

} // NuklearUI
} // sge