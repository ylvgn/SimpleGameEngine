#if SGE_OS_WINDOWS

#include "PW5_GDI.h"
#include "PW5_Win32Util.h"

namespace sge {
#if 0
#pragma mark ========= TextMetrics ============
#endif
TextMetrics::TextMetrics(HWND hwnd) {
	ScopedGetDC hdc(hwnd);
	_internal_init(hdc);
}

TextMetrics::TextMetrics(HDC hdc) {
	_internal_init(hdc);
}

TextMetrics::TextMetrics(const TEXTMETRIC& tm) {
	_set(tm);
}

void TextMetrics::_internal_init(HDC hdc) {
	TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	_set(tm);
}

void TextMetrics::_set(const TEXTMETRIC& tm) {
	height					= tm.tmHeight;
	ascent					= tm.tmAscent;
	descent					= tm.tmDescent;
	internalLeading			= tm.tmInternalLeading;
	maxCharWidth			= tm.tmMaxCharWidth;
	externalLeading			= tm.tmExternalLeading;
	aveCharWidth			= tm.tmAveCharWidth;

	isFixedPitch			= (tm.tmPitchAndFamily & 1) == 0;
	aveCharHeight			= height + externalLeading;
	aveCharWidthUpperCase   = isFixedPitch ? aveCharWidth : static_cast<int>(1.5f * aveCharWidth);
}

UINT MyHDC::setTextAlign(TextAlignment align) {
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

}

#endif