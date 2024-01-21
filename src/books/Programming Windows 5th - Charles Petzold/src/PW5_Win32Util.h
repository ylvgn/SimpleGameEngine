#pragma once
#include "PW5_Common_Win32.h"

namespace sge {
	
struct PW5_Win32Util {
	PW5_Win32Util() = delete;

	static int PW5_Win32Util::getTextAlignment(TextAlignment v);
};

inline
int PW5_Win32Util::getTextAlignment(TextAlignment v) {
	using SRC = TextAlignment;
	switch (v) {
		case SRC::Left:		return TA_LEFT;
		case SRC::Right:	return TA_RIGHT;
		case SRC::Top:		return TA_TOP;
		case SRC::Bottom:	return TA_BOTTOM;
		case SRC::Center:	return TA_CENTER;
		case SRC::BaseLine: return TA_BASELINE;
		default:			throw  SGE_ERROR("unsupported TextAlignment");
	}
}

}