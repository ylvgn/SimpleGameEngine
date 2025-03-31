#include "PW5_GDI__Draw.h"
#include "PW5_GDI__Brush.h"

namespace sge {
namespace GDI {

	inline int fillRect(const ::HDC& hdc, const ::RECT& ltrb, const Color4b& c) {
		ScopedCreateSolidBrush brush(hdc, c);
		return FillRect(hdc, &ltrb, brush);
	}

	inline int fillRect(const ::HDC& hdc, const Rect2f& xywh, const Color4b& c) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return GDI::fillRect(hdc, ltrb, c);
	}

	inline int fillRect(const ::HDC& hdc, const ::RECT& ltrb, StockObj_Brush flag) {
		return FillRect(hdc, &ltrb, GDI::getStockObject(flag));
	}

	inline int fillRect(const ::HDC& hdc, const Rect2f& xywh, StockObj_Brush flag) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return GDI::fillRect(hdc, ltrb, flag);
	}

} // namespace GDI	
} // namespace sge