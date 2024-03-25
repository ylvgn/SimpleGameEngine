#if SGE_OS_WINDOWS

#include "PW5_GDI.h"
#include "PW5_Win32Util.h"

namespace sge {
namespace GDI {

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
UINT ScopedHDC_NoHWND::setTextAlign(PW5_TextAlignmentOption flags /*= PW5_TextAlignmentOption::Left | PW5_TextAlignmentOption::Top*/) {
	// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
	return ::SetTextAlign(_hdc, enumInt(flags)); // if failed, return GDI_ERROR
}

int ScopedHDC_NoHWND::setMappingMode(PW5_MappingMode flag /*= PW5_MappingMode::None*/) {
	return ::SetMapMode(_hdc, enumInt(flag));
}

PW5_MappingMode ScopedHDC_NoHWND::getMappingMode() {
	using SRC = PW5_MappingMode;
	int mode = ::GetMapMode(_hdc);
	switch (mode) {
		case MM_TEXT:		return SRC::None;
		case MM_LOMETRIC:	return SRC::LowMetric;
		case MM_HIMETRIC:	return SRC::HighMetric;
		case MM_LOENGLISH:	return SRC::LowEnglish;
		case MM_HIENGLISH:	return SRC::HighEnglish;
		case MM_TWIPS:		return SRC::Twips;
		case MM_ISOTROPIC:	return SRC::Isotropic;
		case MM_ANISOTROPIC:return SRC::Anisotropic;
		default: throw SGE_ERROR("unsupprted mode = {}", mode);
	}
}

void ScopedHDC_::clearBg(PW5_StockLogicalObject_Brush flag /*= PW5_StockLogicalObject_Brush::White*/) {
	Vec2f oldViewportOrg;
	Vec2f oldWindowOrg;

	getViewportOrg(oldViewportOrg);
	getViewportOrg(oldWindowOrg);

	bool isNotLeftTop = oldViewportOrg != oldWindowOrg;
	if (isNotLeftTop) {
		setViewportOrg(0, 0);
		setWindowOrg(0, 0);
	}

	::RECT rc;
	::GetClientRect(_hwnd, &rc); //  GetClientRect (which is always in terms of device units)
	auto brush = GetStockBrush(enumInt(flag));
	GDI::fillRect(_hdc, rc, brush);

	if (isNotLeftTop) {
		setViewportOrg(oldViewportOrg);
		setWindowOrg(oldWindowOrg);
	}
}

template class ScopedExtCreatePen_Dash_Dot<static_cast<size_t>(PW5_PenStyle::Dot)>;
template class ScopedExtCreatePen_Dash_Dot<static_cast<size_t>(PW5_PenStyle::Dash)>;
template class ScopedExtCreatePen_Dash_Dot<static_cast<size_t>(PW5_PenStyle::DashDot)>;
template class ScopedExtCreatePen_Dash_Dot<static_cast<size_t>(PW5_PenStyle::DashDotDot)>;

ScopedCreateHatchBrush::ScopedCreateHatchBrush(HDC hdc, PW5_HatchStyle flag, const Color4f& c) {
	_hdc = hdc;
	_brush = ::CreateHatchBrush(enumInt(flag), GDI::COLORREF_make(c));
}

ScopedCreateBrush_Hatched::ScopedCreateBrush_Hatched(HDC hdc, PW5_HatchStyle flag, const Color4f& c) {
	::LOGBRUSH logBrush = {};
	logBrush.lbStyle = BS_HATCHED;
	logBrush.lbColor = GDI::COLORREF_make(c);
	logBrush.lbHatch = static_cast<ULONG_PTR>(flag);
	_internal_ctor(hdc, logBrush);
}

}

#endif