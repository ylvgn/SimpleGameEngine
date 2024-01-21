#pragma once

#include <sge_core.h>

namespace sge {

#define TextAlignment_ENUM_LIST(E) \
	E(Left,) \
	E(Right,) \
	E(Top,) \
	E(Bottom,) \
	E(Center,) \
	E(BaseLine,) \
//----
SGE_ENUM_CLASS(TextAlignment, u8)
SGE_ENUM_ALL_OPERATOR(TextAlignment)

template<class T> constexpr
void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

}