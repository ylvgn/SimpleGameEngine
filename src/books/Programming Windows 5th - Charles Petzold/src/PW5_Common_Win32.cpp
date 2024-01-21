#include "PW5_Win32Util.h"

namespace sge {

#if 0
#pragma mark ========= TextMetrics ============
#endif
TextMetrics::TextMetrics(HWND hwnd) {
	ScopedHDC hdc(hwnd);
	_internal_init(hdc);
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

void TextMetrics::_internal_init(HDC hdc) {
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	_set(tm);
}

UINT ScopedHDCBase::setTextAlign(TextAlignment align) {
	// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
	int res = 0;
	int bits = static_cast<int>(align);
	while (bits) {
		int v = Math::lowbit(bits);
		res |= PW5_Win32Util::getTextAlignment(static_cast<TextAlignment>(v));
		bits -= v;
	}
	return ::SetTextAlign(_hdc, res); // if failed, return GDI_ERROR
}

} // namespace