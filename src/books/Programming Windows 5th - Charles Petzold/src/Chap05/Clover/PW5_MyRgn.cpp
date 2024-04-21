#if SGE_OS_WINDOWS

#include "PW5_MyRgn.h"

namespace sge {

void PW5_MyRgn::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);
}

void PW5_MyRgn::onDraw() {
//	_example1();
//	_example2();
	_example3();
}

void PW5_MyRgn::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	ScopedSelectObject_RectRgn rgn (hdc, 100, 100, 200, 200);
	Rect2f rc { 100,100,50,500 };
	GDI::fillRect(hdc, rc, GDI::kbRed);

	hdc.textOut(100, 102, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

void PW5_MyRgn::_example2() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	ScopedSelectObject_EllipticRgn rgn (hdc, 100, 100, 200, 200);
	Rect2f rc { 100,100,50,500 };
	GDI::fillRect(hdc, rc, GDI::kbRed);

	hdc.textOut(100, 102, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

void PW5_MyRgn::_example3() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	ScopedSelectObject_RoundRectRgn rgn (hdc, 100, 100, 200, 200, 10,20);
	Rect2f rc { 100,100,50,500 };
	GDI::fillRect(hdc, rc, GDI::kbRed);

	hdc.textOut(100, 102, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

}

#endif // SGE_OS_WINDOWS