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

TextMetrics::TextMetrics(const ::TEXTMETRIC& tm) {
	_set(tm);
}

void TextMetrics::_internal_init(HDC hdc) {
	::TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	_set(tm);
}

void TextMetrics::_set(const ::TEXTMETRIC& tm) {
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

UINT MyHDC_NoHWND::setTextAlign(TextAlignmentOption flags) {
	// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
	UINT align = g_flags2Bits(flags, PW5_Win32Util::getTextAlignmentOption);
	return ::SetTextAlign(_hdc, align); // if failed, return GDI_ERROR
}

void MyHDC::clearBg(StockLogicalObject flag /*= StockLogicalObject::White*/) {
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	int stock = PW5_Win32Util::getStockLogicalObject(flag);
	auto brush = static_cast<HBRUSH>(GetStockBrush(stock));
	GDI::fillRect(_hdc, rc, brush);
}

}

#endif