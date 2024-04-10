#include "PW5_GDI__Text.h"
#include "PW5_GDI__HDC.h"

namespace sge {

void MyTextMetrics::create(const ::HWND& hwnd) {
	ScopedGetDC hdc(hwnd);
	create(hdc);
}

void MyTextMetrics::create(const ::HDC& hdc) {
	::TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	create(tm);
}

}