#if SGE_OS_WINDOWS

#include "PW5_GDI.h"
#include "PW5_Win32Util.h"

namespace sge {
namespace GDI {
	void drawDashedLine(HDC hdc, int fromX, int fromY, int toX, int toY, const Color4f& c/*= kBlack256*/) {
		LOGBRUSH brush;
		brush.lbColor = COLORREF_make(c);
		brush.lbStyle = PS_SOLID;

		ScopedExtCreatePen scoped(hdc, brush, PenStyle::Dash);
		drawLine(hdc, fromX, fromY, toX, toY);
	}

	void drawDashedLine(HDC hdc, const Vec2f& from, const Vec2f& to, const Color4f& c/*= kBlack256*/) {
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
UINT MyHDC_NoHWND::setTextAlign(TextAlignmentOption flags) {
	// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
	UINT align = g_flags2Bits(flags, PW5_Win32Util::getTextAlignmentOption);
	return ::SetTextAlign(_hdc, align); // if failed, return GDI_ERROR
}

void MyHDC_NoHWND::drawText(::RECT* rc, StrView	str, DTFlag flags) const {
	UINT fDT = g_flags2Bits(flags, PW5_Win32Util::getDrawTextFormatFlag);
	GDI::drawText(_hdc, rc, str, fDT);
}

void MyHDC::clearBg(StockLogicalObject_Brush flag /*= StockLogicalObject_Brush::White*/) {
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	auto brush = GetStockBrush(PW5_Win32Util::getStockLogicalObject_Brush(flag));
	GDI::fillRect(_hdc, rc, brush);
}

ScopedExtCreatePen::ScopedExtCreatePen(HDC hdc, const LOGBRUSH& brush, PenStyle flag /*= PenStyle::Solid*/) {
	_hdc = hdc;
	DWORD dwStyle = PW5_Win32Util::getPenStyle(flag);
	_pen = ::ExtCreatePen(dwStyle, 1, &brush, 0, nullptr);
	SelectPen(_hdc, _pen);
}

ScopedExtCreatePen::~ScopedExtCreatePen() {
	DeletePen(_pen);
	auto pen = GetStockPen(PW5_Win32Util::getStockLogicalObject_Pen(StockLogicalObject_Pen::Black));
	SelectPen(_hdc, pen);
}

}

#endif