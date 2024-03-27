#if SGE_OS_WINDOWS

#include "PW5_GDI.h"
#include "PW5_Win32Util.h"

namespace sge {
namespace GDI {

	void drawPoint(const ::HDC& hdc, int x, int y, const Color4f& c, int ptSize) {
		ScopedCreateSolidBrush brush(hdc, c);
		int halfSize = ptSize / 2;
		::RECT ltrb {
			x - halfSize,
			y - halfSize,
			x + halfSize,
			y + halfSize
		};
		::FillRect(hdc, &ltrb, brush);
	}

	void drawDashedLine(const ::HDC& hdc, int fromX, int fromY, int toX, int toY, const Color4f& c) {
		ScopedExtCreatePen_Dash scoped(hdc, c);
		drawLine(hdc, fromX, fromY, toX, toY);
	}
} // namespace GDI
} // namespace sge


namespace sge {
#if 0
#pragma mark ========= TextMetrics ============
#endif
TextMetrics::TextMetrics(::HWND hwnd) {
	ScopedGetDC hdc(hwnd);
	_internal_init(hdc);
}

TextMetrics::TextMetrics(::HDC hdc) {
	_internal_init(hdc);
}

TextMetrics::TextMetrics(const ::TEXTMETRIC& tm) {
	_set(tm);
}

void TextMetrics::_internal_init(::HDC hdc) {
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

}

#endif