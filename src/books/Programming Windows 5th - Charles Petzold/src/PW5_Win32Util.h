#pragma once
#include "PW5_Common_Win32.h"

namespace sge {
	
struct PW5_Win32Util {
	PW5_Win32Util() = delete;

	static int PW5_Win32Util::getScrollBarConstants(ScrollBarConstants v);
};

inline
int PW5_Win32Util::getScrollBarConstants(ScrollBarConstants v) {
	using SRC = ScrollBarConstants;
	switch (v) {
		case SRC::Horizontal:	return SB_HORZ;
		case SRC::Vertical:		return SB_VERT;
		case SRC::Control:		return SB_CTL;
		case SRC::Both:			return SB_BOTH;
		default: throw SGE_ERROR("unsupported ScrollBarConstants");
	}
}

}