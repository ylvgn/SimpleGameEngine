#include "PW5_GDI__Text.h"
#include "PW5_GDI__HDC.h"

namespace sge {

MyTextMetrics::MyTextMetrics(const ::TEXTMETRIC& r) {
	create(r);
}

void MyTextMetrics::create(const ::TEXTMETRIC& tm) {
	height					= tm.tmHeight;
	ascent					= tm.tmAscent;
	descent					= tm.tmDescent;
	internalLeading			= tm.tmInternalLeading;
	maxCharWidth			= tm.tmMaxCharWidth;
	externalLeading			= tm.tmExternalLeading;
	aveCharWidth			= tm.tmAveCharWidth;
	isFixedPitch			= (tm.tmPitchAndFamily & 1) == 0;
	aveCharHeight			= height + externalLeading;
	aveCharWidthUpperCase	= isFixedPitch 
							? aveCharWidth 
							: static_cast<int>(1.5f * aveCharWidth);
}

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