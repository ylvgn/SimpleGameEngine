#pragma once

#include <sge_core.h>

namespace sge {

template<class T> constexpr
void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

}