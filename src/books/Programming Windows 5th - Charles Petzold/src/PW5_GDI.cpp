#if SGE_OS_WINDOWS

#include "PW5_GDI.h"
#include "PW5_Win32Util.h"

namespace sge {
namespace GDI {

	void drawText(HDC hdc, int left, int top, int right, int bottom, StrView str, DTFlag flags) {
		UINT fDT = g_flags2Bits(flags, PW5_Win32Util::getPW5_DrawTextFormatFlag);
		GDI::drawText(hdc, left, top, right, bottom, str, fDT);
	}

	void drawPoint(HDC hdc, int x, int y, const Color4f& c, int pointSize) {
		ScopedCreateSolidBrush brush(hdc, c);
		int halfSize = pointSize / 2;
		::RECT rc { x - halfSize, y - halfSize, x + halfSize, y + halfSize };
		::FillRect(hdc, &rc, brush);
	}

	void drawDashedLine(HDC hdc, int fromX, int fromY, int toX, int toY, const Color4f& c) {
		LOGBRUSH brush;
		brush.lbColor = COLORREF_make(c);
		brush.lbStyle = PS_SOLID;

		ScopedExtCreatePen scoped(hdc, brush, PW5_PenStyle::Dash);
		drawLine(hdc, fromX, fromY, toX, toY);
	}

	void drawDashedLine(HDC hdc, const Vec2f& from, const Vec2f& to, const Color4f& c) {
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		drawDashedLine(hdc, iFrom.x, iFrom.y, iTo.x, iTo.y, c);
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
UINT MyHDC_NoHWND::setTextAlign(PW5_TextAlignmentOption flags) {
	// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
	UINT align = g_flags2Bits(flags, PW5_Win32Util::getPW5_TextAlignmentOption);
	return ::SetTextAlign(_hdc, align); // if failed, return GDI_ERROR
}

void MyHDC::clearBg(PW5_StockLogicalObject_Brush flag /*= PW5_StockLogicalObject_Brush::White*/) {
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	auto brush = GetStockBrush(PW5_Win32Util::getPW5_StockLogicalObject_Brush(flag));
	GDI::fillRect(_hdc, rc, brush);
}

ScopedExtCreatePen::ScopedExtCreatePen(HDC hdc, const LOGBRUSH& brush, PW5_PenStyle flag /*= PW5_PenStyle::Solid*/) {
	_hdc = hdc;
	DWORD dwStyle = PW5_Win32Util::getPW5_PenStyle(flag);
	_pen = ::ExtCreatePen(dwStyle, 1, &brush, 0, nullptr);
	SelectPen(_hdc, _pen);
}

ScopedExtCreatePen::~ScopedExtCreatePen() {
	DeletePen(_pen);
	auto pen = GetStockPen(PW5_Win32Util::getPW5_StockLogicalObject_Pen(PW5_StockLogicalObject_Pen::Black));
	SelectPen(_hdc, pen);
}

}

#endif