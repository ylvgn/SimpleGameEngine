#if SGE_OS_WINDOWS

#include "PW5_GDI.h"
#include "PW5_Win32Util.h"

namespace sge {
namespace GDI {

	void drawText(HDC hdc, int left, int top, int right, int bottom, StrView str, DTFlag flags) {
		UINT fDT = g_flags2Bits(flags, PW5_Win32Util::getDrawTextFormatFlag);
		GDI::drawText(hdc, left, top, right, bottom, str, fDT);
	}

	void drawPoint(HDC hdc, int x, int y, const Color4f& c, int ptSize) {
		ScopedCreateSolidBrush brush(hdc, c);
		int halfSize = ptSize / 2;
		::RECT rc { x - halfSize, y - halfSize, x + halfSize, y + halfSize };
		::FillRect(hdc, &rc, brush);
	}

	void drawDashedLine(HDC hdc, int fromX, int fromY, int toX, int toY, const Color4f& c) {
		ScopedExtCreatePen_Dash scoped(hdc, c);
		drawLine(hdc, fromX, fromY, toX, toY);
	}
} // namespace GDI
} // namespace sge


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

#if 0
#pragma mark ========= HDC ============
#endif
UINT ScopedHDC_NoHWND::setTextAlign(PW5_TextAlignmentOption flags) {
	// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
	UINT align = g_flags2Bits(flags, PW5_Win32Util::getTextAlignmentOption);
	return ::SetTextAlign(_hdc, align); // if failed, return GDI_ERROR
}

void ScopedHDC::clearBg(PW5_StockLogicalObject_Brush flag /*= PW5_StockLogicalObject_Brush::White*/) {
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	auto brush = GetStockBrush(PW5_Win32Util::getStockLogicalObject_Brush(flag));
	GDI::fillRect(_hdc, rc, brush);
}

template class ScopedExtCreatePen_Dash_Dot<PW5_Win32Util::getPenStyle(PW5_PenStyle::Dot)>;
template class ScopedExtCreatePen_Dash_Dot<PW5_Win32Util::getPenStyle(PW5_PenStyle::Dash)>;
template class ScopedExtCreatePen_Dash_Dot<PW5_Win32Util::getPenStyle(PW5_PenStyle::DashDot)>;
template class ScopedExtCreatePen_Dash_Dot<PW5_Win32Util::getPenStyle(PW5_PenStyle::DashDotDot)>;

ScopedCreateHatchBrush::ScopedCreateHatchBrush(HDC hdc, PW5_HatchStyle v, const Color4f& c) {
	_hdc = hdc;
	_brush = ::CreateHatchBrush(PW5_Win32Util::getHatchStyle(v), GDI::COLORREF_make(c));
}

ScopedCreateBrush_Hatched::ScopedCreateBrush_Hatched(HDC hdc, PW5_HatchStyle v, const Color4f& c) {
	::LOGBRUSH logBrush = {};
	logBrush.lbStyle = BS_HATCHED;
	logBrush.lbColor = GDI::COLORREF_make(c);
	logBrush.lbHatch = PW5_Win32Util::getHatchStyle(v);
	_internal_ctor(hdc, logBrush);
}

}

#endif