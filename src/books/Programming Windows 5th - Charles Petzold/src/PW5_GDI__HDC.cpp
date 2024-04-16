#include "PW5_GDI__HDC.h"
#include "PW5_GDI__Brush.h"

namespace sge {

void ScopedHDC_NoHWND::drawPoint(int x, int y, const Color4b& c, int ptSize) const {
	GDI::drawPoint(_hdc, x, y, c,ptSize);
}
void ScopedHDC_NoHWND::drawPoint(const ::POINT& pt, const Color4b& c, int ptSize) const {
	GDI::drawPoint(_hdc, pt, c,ptSize);
}
void ScopedHDC_NoHWND::drawPoint(const Vec2f& pt, const Color4b& c, int ptSize)	const {
	GDI::drawPoint(_hdc, pt, c,ptSize);
}
void ScopedHDC_NoHWND::drawPoint(const Vec2i& pt, const Color4b& c, int ptSize)	const {
	GDI::drawPoint(_hdc, pt, c, ptSize);
}

void ScopedHDC_::clearBg(const Color4b& solidColor) {
	if (solidColor == GDI::kbWhite || solidColor == GDI::kbBlack) {
		clearBg(solidColor == GDI::kbWhite ? StockObj_Brush::White : StockObj_Brush::Black);
		return;
	}
	ScopedCreateSolidBrush brush(_hdc, solidColor);
	clearBg(brush);
}

}