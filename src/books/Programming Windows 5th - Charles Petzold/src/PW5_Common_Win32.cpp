#include "PW5_Common_Win32.h"

namespace sge {

TextMetrics::TextMetrics(HWND hwnd) {
	ScopedHDC hdc(hwnd); _internal_init(hdc);
}

TextMetrics::TextMetrics(HDC hdc) {
	_internal_init(hdc);
}

TextMetrics::TextMetrics(const TEXTMETRIC& tm) {
	_set(tm);
}

void TextMetrics::_set(const TEXTMETRIC& tm) {
	_height					= tm.tmHeight;
	_ascent					= tm.tmAscent;
	_descent				= tm.tmDescent;
	_internalLeading		= tm.tmInternalLeading;
	_maxCharWidth			= tm.tmMaxCharWidth;
	_externalLeading		= tm.tmExternalLeading;
	_aveCharWidth			= tm.tmAveCharWidth;

	_isFixedPitch			= (tm.tmPitchAndFamily & 1) == 0;
	_aveCharHeight			= _height + _externalLeading;
	_aveUpperCaseCharHeight = _isFixedPitch ? _aveCharWidth : static_cast<int>(1.5f * _aveCharWidth);
}

}